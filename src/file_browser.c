/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2009, 2010 Sébastien Wilmet
 *
 * LaTeXila is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LaTeXila is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LaTeXila.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for strcmp
#include <gtk/gtk.h>

#include "main.h"
#include "config.h"
#include "print.h"
#include "file_browser.h"
#include "callbacks.h"
#include "external_commands.h"

static void fill_list_store_with_dir (const gchar *directory);
static void cb_go_to_home_dir (GtkButton *button, gpointer user_data);
static void cb_go_to_parent_dir (GtkButton *button, gpointer user_data);
static void cb_jump_dir_current_doc (GtkButton *button, gpointer user_data);
static void cb_file_browser_row_activated (GtkTreeView *tree_view,
		GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
static gint sort_list_alphabetical_order (gconstpointer a, gconstpointer b);
static void cb_combo_parent_dir_changed (GtkComboBox *combo_box,
		gpointer user_data);

// list of files and directories of the current directory
static GtkListStore *list_store;
// for the list of directories showed in a combobox
static GtkListStore *parent_dir_store;
static GtkComboBox *combo;

void
init_file_browser (GtkWidget *vbox)
{
	/* mini-toolbar */
	GtkWidget *hbox = gtk_hbox_new (TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	// go to the home user directory
	{
		GtkWidget *home_button = gtk_button_new ();
		gtk_button_set_relief (GTK_BUTTON (home_button), GTK_RELIEF_NONE);
		GtkWidget *home_icon = gtk_image_new_from_stock (GTK_STOCK_HOME,
				GTK_ICON_SIZE_BUTTON);
		gtk_container_add (GTK_CONTAINER (home_button), home_icon);
		gtk_widget_set_tooltip_text (home_button, _("Go to the home directory"));
		g_signal_connect (G_OBJECT (home_button), "clicked",
				G_CALLBACK (cb_go_to_home_dir), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), home_button, TRUE, TRUE, 0);
	}

	// go to the parent directory
	{
		GtkWidget *parent_dir_button = gtk_button_new ();
		gtk_button_set_relief (GTK_BUTTON (parent_dir_button), GTK_RELIEF_NONE);
		GtkWidget *parent_dir_icon = gtk_image_new_from_stock (GTK_STOCK_GO_UP,
				GTK_ICON_SIZE_BUTTON);
		gtk_container_add (GTK_CONTAINER (parent_dir_button), parent_dir_icon);
		gtk_widget_set_tooltip_text (parent_dir_button, _("Go to the parent directory"));
		g_signal_connect (G_OBJECT (parent_dir_button), "clicked",
				G_CALLBACK (cb_go_to_parent_dir), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), parent_dir_button, TRUE, TRUE, 0);
	}

	// jump to the directory of the current document
	{
		GtkWidget *jump_button = gtk_button_new ();
		gtk_button_set_relief (GTK_BUTTON (jump_button), GTK_RELIEF_NONE);
		GtkWidget *jump_icon = gtk_image_new_from_stock (GTK_STOCK_JUMP_TO,
				GTK_ICON_SIZE_BUTTON);
		gtk_container_add (GTK_CONTAINER (jump_button), jump_icon);
		gtk_widget_set_tooltip_text (jump_button,
				_("Go to the directory of the current document"));
		g_signal_connect (G_OBJECT (jump_button), "clicked",
				G_CALLBACK (cb_jump_dir_current_doc), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), jump_button, TRUE, TRUE, 0);
	}

	// refresh
	{
		GtkWidget *refresh_button = gtk_button_new ();
		gtk_button_set_relief (GTK_BUTTON (refresh_button), GTK_RELIEF_NONE);
		GtkWidget *refresh_icon = gtk_image_new_from_stock (GTK_STOCK_REFRESH,
				GTK_ICON_SIZE_BUTTON);
		gtk_container_add (GTK_CONTAINER (refresh_button), refresh_icon);
		gtk_widget_set_tooltip_text (refresh_button, _("Refresh"));
		g_signal_connect (G_OBJECT (refresh_button), "clicked",
				G_CALLBACK (cb_file_browser_refresh), NULL);
		gtk_box_pack_start (GTK_BOX (hbox), refresh_button, TRUE, TRUE, 0);
	}

	/* list of parent directories */
	{
		parent_dir_store = gtk_list_store_new (N_COLS_PARENT_DIR,
				G_TYPE_STRING,	// indentation (spaces)
				G_TYPE_STRING,	// stock-id of a pixbuf
				G_TYPE_STRING,	// directory name
				G_TYPE_STRING	// full path
				);

		GtkWidget *combo_box =
			gtk_combo_box_new_with_model (GTK_TREE_MODEL (parent_dir_store));
		g_object_unref (parent_dir_store);
		combo = GTK_COMBO_BOX (combo_box);

		g_signal_connect (G_OBJECT (combo_box), "changed",
				G_CALLBACK (cb_combo_parent_dir_changed), NULL);

		// indentation
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box), renderer, FALSE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box), renderer,
				"text", COL_PARENT_DIR_INDENT, NULL);

		// pixbuf
		renderer = gtk_cell_renderer_pixbuf_new ();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box), renderer, FALSE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box), renderer,
				"stock-id", COL_PARENT_DIR_PIXBUF, NULL);

		// directory
		renderer = gtk_cell_renderer_text_new ();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box), renderer, TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box), renderer,
				"text", COL_PARENT_DIR_DIRECTORY, NULL);
		// omit characters at the end of the text if not enough place
		g_object_set (renderer, "ellipsize-set", TRUE,
				"ellipsize", PANGO_ELLIPSIZE_END, NULL);

		gtk_box_pack_start (GTK_BOX (vbox), combo_box, FALSE, FALSE, 0);
	}

	/* list of files and directories */
	{
		list_store = gtk_list_store_new (N_COLS_FILE_BROWSER,
				G_TYPE_STRING, // stock-id of a pixbuf
				G_TYPE_STRING  // file
				);

		fill_list_store_with_dir (NULL);

		GtkWidget *tree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (list_store));
		g_object_unref (list_store);
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree_view), FALSE);

		// dubble-click on a row will open the file
		g_signal_connect (G_OBJECT (tree_view), "row-activated",
				G_CALLBACK (cb_file_browser_row_activated), NULL);

		GtkTreeViewColumn *column = gtk_tree_view_column_new ();
		gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);

		// show the icon
		GtkCellRenderer *renderer1 = gtk_cell_renderer_pixbuf_new ();
		gtk_tree_view_column_pack_start (column, renderer1, FALSE);
		gtk_tree_view_column_set_attributes (column, renderer1,
				"stock-id", COL_FILE_BROWSER_PIXBUF,
				NULL);

		// show the file name
		GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new ();
		gtk_tree_view_column_pack_start (column, renderer2, FALSE);
		gtk_tree_view_column_set_attributes (column, renderer2,
				"text", COL_FILE_BROWSER_FILE,
				NULL);

		// with a scrollbar
		GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (scrollbar), tree_view);

		gtk_box_pack_start (GTK_BOX (vbox), scrollbar, TRUE, TRUE, 0);
	}
}

void
cb_file_browser_refresh (GtkButton *button, gpointer user_data)
{
	fill_list_store_with_dir (NULL);
}


static void
fill_list_store_with_dir (const gchar *directory)
{
	// if directory is NULL, open the current directory
	GError *error = NULL;
	GDir *dir;
	if (directory == NULL)
		dir = g_dir_open (latexila.prefs.file_browser_dir, 0, &error);
	else
		dir = g_dir_open (directory, 0, &error);

	if (error != NULL)
	{
		print_warning ("File browser: %s", error->message);

		// warning dialog
		GtkWidget *dialog = gtk_message_dialog_new (latexila.main_window,
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_CLOSE,
				_("File Browser"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
				"%s", error->message);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		g_error_free (error);
		return;
	}

	gtk_list_store_clear (list_store);
	gtk_list_store_clear (parent_dir_store);

	if (directory != NULL)
	{
		g_free (latexila.prefs.file_browser_dir);
		latexila.prefs.file_browser_dir = g_strdup (directory);
	}

	/* append all the files contained in the directory */
	const gchar *read_name = NULL;
	GList *directory_list = NULL;
	GList *file_list = NULL;
	while ((read_name = g_dir_read_name (dir)) != NULL)
	{
		// not show hidden files
		if (read_name[0] == '.' && (! latexila.prefs.file_browser_show_all_files
					|| ! latexila.prefs.file_browser_show_hidden_files))
			continue;

		gchar *full_path = g_build_filename (latexila.prefs.file_browser_dir,
				read_name, NULL);

		// make a copy of read_name, else there are memory errors if the
		// directory is very big (/usr/bin for example)
		gchar *tmp = g_strdup (read_name);

		if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
			directory_list = g_list_prepend (directory_list, (gpointer) tmp);

		// show all files or only *.tex, *.pdf, *.dvi, *.ps and *.bib
		else if (latexila.prefs.file_browser_show_all_files
				|| g_str_has_suffix (full_path, ".tex")
				|| g_str_has_suffix (full_path, ".pdf")
				|| g_str_has_suffix (full_path, ".dvi")
				|| g_str_has_suffix (full_path, ".ps")
				|| g_str_has_suffix (full_path, ".bib"))
		{
			file_list = g_list_prepend (file_list, (gpointer) tmp);
		}

		g_free (full_path);
	}

	g_dir_close (dir);

	// sort the lists in alphabetical order
	directory_list = g_list_sort (directory_list, sort_list_alphabetical_order);
	file_list = g_list_sort (file_list, sort_list_alphabetical_order);

	GtkTreeIter iter;

	// traverse the directory list
	GList *current = directory_list;
	while (current != NULL)
	{
		gchar *directory = current->data;

		// append the directory to the list store
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (list_store, &iter,
				COL_FILE_BROWSER_PIXBUF, GTK_STOCK_DIRECTORY,
				COL_FILE_BROWSER_FILE, directory,
				-1);

		g_free (directory);
		current = g_list_next (current);
	}

	// traverse the file list
	current = file_list;
	while (current != NULL)
	{
		gchar *file = current->data;

		gchar *stock_id;
		if (g_str_has_suffix (file, ".tex"))
			stock_id = GTK_STOCK_EDIT;
		else if (g_str_has_suffix (file, ".pdf"))
			stock_id = "view_pdf";
		else if (g_str_has_suffix (file, ".dvi"))
			stock_id = "view_dvi";
		else if (g_str_has_suffix (file, ".ps"))
			stock_id = "view_ps";
		else
			stock_id = GTK_STOCK_FILE;


		// append the file to the list store
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (list_store, &iter,
				COL_FILE_BROWSER_PIXBUF, stock_id,
				COL_FILE_BROWSER_FILE, file,
				-1);

		g_free (file);
		current = g_list_next (current);
	}

	g_list_free (directory_list);
	g_list_free (file_list);

	/* fill the parent directory list */
	if (! g_path_is_absolute (latexila.prefs.file_browser_dir))
		return;

	GList *parent_dirs = NULL;
	parent_dirs = g_list_append (parent_dirs,
			g_strdup (latexila.prefs.file_browser_dir));
	gchar *path = latexila.prefs.file_browser_dir;

	while (TRUE)
	{
		gchar *parent = g_path_get_dirname (path);
		parent_dirs = g_list_prepend (parent_dirs, parent);
		path = parent;
		if (strcmp (parent, "/") == 0)
			break;
	}

	current = parent_dirs;
	gint i = 0;
	while (current != NULL)
	{
		gchar *current_dir = current->data;

		// basename
		gchar *basename;
		if (i == 0)
			basename = g_strdup (_("File System"));
		else
			basename = g_path_get_basename (current_dir);

		// indentation
		gchar *indent = g_strnfill (i * 2, ' ');

		// pixbuf
		gchar *stock_id;
		if (i == 0)
			stock_id = GTK_STOCK_HARDDISK;
		else if (strcmp (current_dir, g_get_home_dir ()) == 0)
			stock_id = GTK_STOCK_HOME;
		else
			stock_id = GTK_STOCK_DIRECTORY;

		gtk_list_store_append (parent_dir_store, &iter);
		gtk_list_store_set (parent_dir_store, &iter,
				COL_PARENT_DIR_INDENT, indent,
				COL_PARENT_DIR_PIXBUF, stock_id,
				COL_PARENT_DIR_DIRECTORY, basename,
				COL_PARENT_DIR_PATH, current_dir,
				-1);

		g_free (current_dir);
		g_free (basename);
		g_free (indent);
		current = g_list_next (current);
		i++;
	}

	// select the last parent directory
	gtk_combo_box_set_active_iter (combo, &iter);
}

static void
cb_go_to_home_dir (GtkButton *button, gpointer user_data)
{
	fill_list_store_with_dir (g_get_home_dir ());
}

static void
cb_go_to_parent_dir (GtkButton *button, gpointer user_data)
{
	gchar *path = g_path_get_dirname (latexila.prefs.file_browser_dir);
	fill_list_store_with_dir (path);
	g_free (path);
}

static void
cb_jump_dir_current_doc (GtkButton *button, gpointer user_data)
{
	if (latexila.active_doc == NULL || latexila.active_doc->path == NULL)
		return;

	gchar *path = g_path_get_dirname (latexila.active_doc->path);
	fill_list_store_with_dir (path);
	g_free (path);
}

static void
cb_file_browser_row_activated (GtkTreeView *tree_view, GtkTreePath *path,
		GtkTreeViewColumn *column, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL (list_store);
	gtk_tree_model_get_iter (model, &iter, path);
	
	gchar *file = NULL;
	gtk_tree_model_get (model, &iter,
			COL_FILE_BROWSER_FILE, &file,
			-1);

	gchar *full_path = g_build_filename (latexila.prefs.file_browser_dir,
			file, NULL);

	// open the directory
	if (g_file_test (full_path, G_FILE_TEST_IS_DIR))
		fill_list_store_with_dir (full_path);

	// view the document
	else if (g_str_has_suffix (full_path, ".pdf"))
		view_document (_("View PDF"), full_path);
	else if (g_str_has_suffix (full_path, ".dvi"))
		view_document (_("View DVI"), full_path);
	else if (g_str_has_suffix (full_path, ".ps"))
		view_document (_("View PS"), full_path);

	// open the file
	else
		open_new_document_without_uri (full_path);

	g_free (full_path);
}

static gint
sort_list_alphabetical_order (gconstpointer a, gconstpointer b)
{
	return g_utf8_collate ((char *) a, (char *) b);
}

static void
cb_combo_parent_dir_changed (GtkComboBox *combo_box, gpointer user_data)
{
	GtkTreeIter iter;
	if (gtk_combo_box_get_active_iter (combo_box, &iter))
	{
		gchar *path;
		GtkTreeModel *model = gtk_combo_box_get_model (combo_box);
		gtk_tree_model_get (model, &iter,
				COL_PARENT_DIR_PATH, &path,
				-1);

		// avoid infinite loop (this function is called in
		// fill_list_store_with_dir())
		if (strcmp (path, latexila.prefs.file_browser_dir) != 0)
			fill_list_store_with_dir (path);
		g_free (path);
	}
}
