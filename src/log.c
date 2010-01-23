/*
 * This file is part of LaTeXila.
 *
 * Copyright © 2010 Sébastien Wilmet
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
#include <string.h>
#include <gtk/gtk.h>

#include "main.h"
#include "log.h"
#include "utils.h"
#include "callbacks.h"
#include "print.h"

static void cb_action_history_changed (GtkTreeSelection *selection,
		gpointer user_data);
static GtkListStore * get_new_output_list_store (void);
static void go_to_message (gboolean next, gint message_type);
static void scroll_to_iter (GtkTreeIter *iter, gboolean force);
static gboolean output_row_selection_func (GtkTreeSelection *selection,
		GtkTreeModel *model, GtkTreePath *path,
		gboolean path_currently_selected, gpointer data);
static void select_row (GtkTreeModel *model, GtkTreeIter *iter);
static void jump_to_file (void);
static void index_messages_by_type (GtkTreeModel *model);
static void set_previous_next_actions_sensitivity (void);
static void path_free (gpointer data);
static void index_free (gpointer data);

static GtkTreeView *history_view;
static GtkTreeView *output_view;

// used to scroll to the end and to flush not everytime (because it is slow)
static gint nb_lines = 0;

void
init_log_zone (GtkPaned *log_hpaned, GtkWidget *log_toolbar)
{
	/* action history */
	{
		GtkListStore *history_list_store = gtk_list_store_new (N_COLS_ACTION,
				G_TYPE_STRING, G_TYPE_POINTER);
		
		history_view = GTK_TREE_VIEW (gtk_tree_view_new_with_model (
					GTK_TREE_MODEL (history_list_store)));
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes (
				_("Action history"), renderer, "text", COL_ACTION_TITLE, NULL);	
		gtk_tree_view_append_column (history_view, column);
		
		GtkTreeSelection *select = gtk_tree_view_get_selection (history_view);
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (select), "changed",
				G_CALLBACK (cb_action_history_changed), NULL);

		// with a scrollbar
		GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (scrollbar), GTK_WIDGET (history_view));
		gtk_paned_add1 (log_hpaned, scrollbar);
	}
	
	/* log details */
	{
		GtkListStore *output_list_store = get_new_output_list_store ();

		output_view = GTK_TREE_VIEW (gtk_tree_view_new_with_model (
					GTK_TREE_MODEL (output_list_store)));
		g_object_unref (output_list_store);
		set_previous_next_actions_sensitivity ();

		// we can now show some text (output_view must be initialized)
		print_output_normal (_("Welcome to LaTeXila!"));

		gtk_tree_view_set_headers_visible (output_view, FALSE);
		gtk_tree_view_set_tooltip_column (output_view, COL_OUTPUT_FILENAME);

		// one column with 3 cell renderers (basename, line number, and message)
		GtkTreeViewColumn *column = gtk_tree_view_column_new ();
		gtk_tree_view_append_column (output_view, column);

		// basename
		GtkCellRenderer *renderer1 = gtk_cell_renderer_text_new ();
		g_object_set (renderer1, "weight-set", TRUE, NULL);
		gtk_tree_view_column_pack_start (column, renderer1, FALSE);
		gtk_tree_view_column_set_attributes (column, renderer1,
				"text", COL_OUTPUT_BASENAME,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"background", COL_OUTPUT_BG_COLOR,
				"background-set", COL_OUTPUT_BG_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);

		// line number
		GtkCellRenderer *renderer2 = gtk_cell_renderer_text_new ();
		g_object_set (renderer2, "weight-set", TRUE, NULL);
		gtk_tree_view_column_pack_start (column, renderer2, FALSE);
		gtk_tree_view_column_set_attributes (column, renderer2,
				"text", COL_OUTPUT_LINE_NUMBER,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"background", COL_OUTPUT_BG_COLOR,
				"background-set", COL_OUTPUT_BG_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);

		// message
		GtkCellRenderer *renderer3 = gtk_cell_renderer_text_new ();
		g_object_set (renderer3, "weight-set", TRUE, NULL);
		gtk_tree_view_column_pack_start (column, renderer3, FALSE);
		gtk_tree_view_column_set_attributes (column, renderer3,
				"text", COL_OUTPUT_MESSAGE,
				"foreground", COL_OUTPUT_COLOR,
				"foreground-set", COL_OUTPUT_COLOR_SET,
				"background", COL_OUTPUT_BG_COLOR,
				"background-set", COL_OUTPUT_BG_COLOR_SET,
				"weight", COL_OUTPUT_WEIGHT,
				NULL);

		// selection
		GtkTreeSelection *select = gtk_tree_view_get_selection (output_view);
		gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);

		// If the user click on a row, output_row_selection_func() will be
		// called, but no row will be selected (the function return FALSE
		// everytime). Instead, if the row can be "selected" (if the message
		// type is an error, a warning or a badbox, and if the filename is not
		// empty), the background and foreground colors are inverted.
		// The GtkTreePath of the row selected is stored as a data in the
		// GtkTreeModel (with g_object_set_data()).
		gtk_tree_selection_set_select_function (select,
				(GtkTreeSelectionFunc) output_row_selection_func, NULL, NULL);

		// with a scrollbar
		GtkWidget *scrollbar = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollbar),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add (GTK_CONTAINER (scrollbar), GTK_WIDGET (output_view));

		/* left: output view
		 * right: log toolbar (vertical)
		 */
		GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), scrollbar, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), log_toolbar, FALSE, FALSE, 0);
		gtk_paned_add2 (log_hpaned, hbox);
	}
}

static void
cb_action_history_changed (GtkTreeSelection *selection, gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *history_model;
	if (gtk_tree_selection_get_selected (selection, &history_model, &iter))
	{
		GtkTreeModel *output_model;
		gtk_tree_model_get (history_model, &iter,
				COL_ACTION_OUTPUT_STORE, &output_model,
				-1);
		gtk_tree_view_set_model (output_view, output_model);

		output_view_columns_autosize ();

		// scroll to the selected line
		GtkTreePath *path = g_object_get_data (G_OBJECT (output_model),
				"row-selected");
		if (path != NULL)
		{
			GtkTreeIter output_iter;
			gtk_tree_model_get_iter (output_model, &output_iter, path);
			scroll_to_iter (&output_iter, TRUE);
		}

		set_previous_next_actions_sensitivity ();
	}
}

static GtkListStore *
get_new_output_list_store (void)
{
	GtkListStore *output_list_store = gtk_list_store_new (N_COLS_OUTPUT,
			G_TYPE_STRING,		// basename
			G_TYPE_STRING,		// filename
			G_TYPE_STRING,		// line number
			G_TYPE_STRING,		// message
			G_TYPE_INT,			// message type
			G_TYPE_STRING,		// color
			G_TYPE_BOOLEAN,		// color set
			G_TYPE_STRING,		// background color
			G_TYPE_BOOLEAN,		// background color set
			G_TYPE_INT			// weight
			);
	return output_list_store;
}

void
add_action (const gchar *title, const gchar *command)
{
	static gint num = 1;

	GtkListStore *output_list_store = get_new_output_list_store ();
	gtk_tree_view_set_model (output_view, GTK_TREE_MODEL (output_list_store));
	set_previous_next_actions_sensitivity ();

	// print title and command to the new list store
	gchar *title_with_num = g_strdup_printf ("%d. %s", num, title);
	print_output_title (title_with_num);

    gchar *command_with_dolar = g_strdup_printf ("$ %s", command);
	print_output_info (command_with_dolar);
	g_free (command_with_dolar);

	// append a new entry to the history action list
	GtkTreeModel *history_tree_model = gtk_tree_view_get_model (history_view);
	GtkTreeIter iter;
	gtk_list_store_append (GTK_LIST_STORE (history_tree_model), &iter);
	gtk_list_store_set (GTK_LIST_STORE (history_tree_model), &iter,
			COL_ACTION_TITLE, title_with_num,
			COL_ACTION_OUTPUT_STORE, output_list_store,
			-1);

	// select the new entry
	GtkTreeSelection *selection = gtk_tree_view_get_selection (history_view);
	gtk_tree_selection_select_iter (selection, &iter);

	// scroll to the end
	GtkTreePath *path = gtk_tree_model_get_path (history_tree_model, &iter);
	gtk_tree_view_scroll_to_cell (history_view, path, NULL, FALSE, 0, 0);
	gtk_tree_path_free (path);

	// delete the first entry
	if (num > 5)
	{
		GtkTreeIter first;
		gtk_tree_model_get_iter_first (history_tree_model, &first);

		// free the output model
		GtkTreeModel *first_output_store;
		gtk_tree_model_get (history_tree_model, &first,
				COL_ACTION_OUTPUT_STORE, &first_output_store,
				-1);
		g_object_unref (first_output_store);

		gtk_list_store_remove (GTK_LIST_STORE (history_tree_model), &first);
	}

	num++;
	g_free (title_with_num);
}

static void
go_to_message (gboolean next, gint message_type)
{
	GtkTreeModel *model = gtk_tree_view_get_model (output_view);
	messages_index_t *index = g_object_get_data (G_OBJECT (model), "index");
	if (index == NULL)
		return;

	// set nb_messages and messages with the data from index for the right
	// message type
	gint nb_messages;
	gint *messages;
	switch (message_type)
	{
		case MESSAGE_TYPE_ERROR:
			nb_messages = index->nb_errors;
			messages = index->errors;
			break;

		case MESSAGE_TYPE_WARNING:
			nb_messages = index->nb_warnings;
			messages = index->warnings;
			break;

		case MESSAGE_TYPE_BADBOX:
			nb_messages = index->nb_badboxes;
			messages = index->badboxes;
			break;

		default:
			return;
	}

	if (nb_messages == 0)
		return;


	// find the row number to select
	gint row_to_select = -1;

	GtkTreePath *path = g_object_get_data (G_OBJECT (model), "row-selected");
	if (path != NULL)
	{
		gint *indices = gtk_tree_path_get_indices (path);
		gint indice = indices[0];
		for (gint i = 0 ; i < nb_messages ; i++)
		{
			if (next && messages[i] > indice)
			{
				row_to_select = messages[i];
				break;
			}
			else if (! next && messages[i] >= indice)
			{
				if (i == 0)
					return;
				row_to_select = messages[i-1];
				break;
			}
			// the row selected is after the last row of the type message_type
			else if (! next && i == nb_messages - 1)
			{
				row_to_select = messages[i];
				break;
			}
		}

		// not found
		if (row_to_select == -1)
			return;
	}

	// no row is selected, take the first
	else if (path == NULL && next)
		row_to_select = messages[0];
	else
		return;

	GtkTreePath *new_path = gtk_tree_path_new_from_indices (row_to_select, -1);
	GtkTreeIter iter;
	if (! gtk_tree_model_get_iter (model, &iter, new_path))
		return;

	select_row (model, &iter);
	scroll_to_iter (&iter, TRUE);
}

void
cb_go_previous_latex_error (void)
{
	go_to_message (FALSE, MESSAGE_TYPE_ERROR);
}

void
cb_go_previous_latex_warning (void)
{
	go_to_message (FALSE, MESSAGE_TYPE_WARNING);
}

void
cb_go_previous_latex_badbox (void)
{
	go_to_message (FALSE, MESSAGE_TYPE_BADBOX);
}

void
cb_go_next_latex_error (void)
{
	go_to_message (TRUE, MESSAGE_TYPE_ERROR);
}

void
cb_go_next_latex_warning (void)
{
	go_to_message (TRUE, MESSAGE_TYPE_WARNING);
}

void
cb_go_next_latex_badbox (void)
{
	go_to_message (TRUE, MESSAGE_TYPE_BADBOX);
}

void
set_history_sensitivity (gboolean sensitive)
{
	gtk_widget_set_sensitive (GTK_WIDGET (history_view), sensitive);
}

void
output_view_columns_autosize (void)
{
	gtk_tree_view_columns_autosize (output_view);
}

void
print_output_title (const gchar *title)
{
	// generally, the title is the first line in a new output, and is used only
	// once, so we reset the counter...
	nb_lines = 0;

	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, title,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_BG_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_BOLD,
			-1);
}

void
print_output_info (const gchar *info)
{
	nb_lines++;

	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, info,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_BG_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_iter (&iter, FALSE);
}

void
print_output_stats (gint nb_errors, gint nb_warnings, gint nb_badboxes)
{
	gchar *str = g_strdup_printf ("%d %s, %d %s, %d %s",
			nb_errors, nb_errors > 1 ? "errors" : "error",
			nb_warnings, nb_warnings > 1 ? "warnings" : "warning",
			nb_badboxes, nb_badboxes > 1 ? "badboxes" : "badbox");

	print_output_info (str);
	g_free (str);

	// init the index
	// we wait the exit code to fill it
	messages_index_t *index = g_malloc (sizeof (messages_index_t));
	index->nb_errors = nb_errors;
	index->nb_warnings = nb_warnings;
	index->nb_badboxes = nb_badboxes;
	index->errors = g_malloc (nb_errors * sizeof (gint));
	index->warnings = g_malloc (nb_warnings * sizeof (gint));
	index->badboxes = g_malloc (nb_badboxes * sizeof (gint));

	GtkTreeModel *model = gtk_tree_view_get_model (output_view);
	g_object_set_data_full (G_OBJECT (model), "index", index,
			(GDestroyNotify) index_free);
}

// if message != NULL the exit_code is not taken into account
void
print_output_exit (const gint exit_code, const gchar *message)
{
	nb_lines++;

	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, INFO_MESSAGE,
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, TRUE,
			COL_OUTPUT_BG_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);

	if (message != NULL)
	{
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, message,
				COL_OUTPUT_COLOR, COLOR_RED,
				-1);
	}
	else if (exit_code == 0)
	{
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, _("Done!"),
				COL_OUTPUT_COLOR, COLOR_GREEN,
				-1);
	}
	else
	{
		gchar *tmp = g_strdup_printf (_("Finished with exit code %d"), exit_code);
		gtk_list_store_set (list_store, &iter,
				COL_OUTPUT_MESSAGE, tmp,
				COL_OUTPUT_COLOR, COLOR_RED,
				-1);
		g_free (tmp);
	}

	// force the scrolling and the flush
	scroll_to_iter (&iter, TRUE);

	// it's normally the last line, so we can index the messages by type
	index_messages_by_type (GTK_TREE_MODEL (list_store));
}

void
print_output_message (const gchar *filename, const gint line_number,
		const gchar *message, gint message_type)
{
	// this function is used only in the filter, so there are less lines
	// but we want to flush more often
	nb_lines += 2;

	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));

	gchar *basename;
	if (filename != NULL)
		basename = g_path_get_basename (filename);
	else
		basename = g_strdup ("");

	gchar *line_number_str;
	if (line_number != NO_LINE)
		line_number_str = g_strdup_printf ("%d", line_number);
	else
		line_number_str = g_strdup ("");

	gchar *color;
	switch (message_type)
	{
		case MESSAGE_TYPE_ERROR:
			color = COLOR_RED;
			break;
		case MESSAGE_TYPE_WARNING:
			color = COLOR_ORANGE;
			break;
		case MESSAGE_TYPE_BADBOX:
			color = COLOR_BROWN;
			break;
		default:
			color = "black";
			break;
	}

	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, basename, 
			COL_OUTPUT_FILENAME, filename != NULL ? filename : "",
			COL_OUTPUT_LINE_NUMBER, line_number_str,
			COL_OUTPUT_MESSAGE, message,
			COL_OUTPUT_MESSAGE_TYPE, message_type,
			COL_OUTPUT_COLOR, color,
			COL_OUTPUT_COLOR_SET, TRUE,
			// we set the background color so when the row is selected we just
			// have to change the "set" flag
			COL_OUTPUT_BG_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_iter (&iter, FALSE);

	g_free (basename);
	g_free (line_number_str);
}

void
print_output_normal (const gchar *message)
{
	nb_lines++;

	GtkListStore *list_store =
		GTK_LIST_STORE (gtk_tree_view_get_model (output_view));
	GtkTreeIter iter;
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter,
			COL_OUTPUT_BASENAME, "", 
			COL_OUTPUT_LINE_NUMBER, "",
			COL_OUTPUT_MESSAGE, message,
			COL_OUTPUT_MESSAGE_TYPE, MESSAGE_TYPE_OTHER,
			COL_OUTPUT_COLOR_SET, FALSE,
			COL_OUTPUT_BG_COLOR_SET, FALSE,
			COL_OUTPUT_WEIGHT, WEIGHT_NORMAL,
			-1);
	scroll_to_iter (&iter, FALSE);
}

static void
scroll_to_iter (GtkTreeIter *iter, gboolean force)
{
	/* Flush the queue for the 50 first lines and then every 40 lines.
	 * This is for the fluidity of the output, without that the lines do not
	 * appear directly and it's ugly. But it is very slow, for a command that
	 * execute for example in 10 seconds, it could take 250 seconds (!) if we
	 * flush the queue at each line... But with commands that take 1
	 * second or so there is not a big difference.
	 */
	if (force || nb_lines < 50 || nb_lines % 40 == 0)
	{
		GtkTreeModel *tree_model = gtk_tree_view_get_model (output_view);
		GtkTreePath *path = gtk_tree_model_get_path (tree_model, iter);
		gtk_tree_view_scroll_to_cell (output_view, path, NULL, FALSE, 0, 0);
		gtk_tree_path_free (path);
		flush_queue ();
	}
}

static gboolean
output_row_selection_func (GtkTreeSelection *selection, GtkTreeModel *model,
		GtkTreePath *path, gboolean path_currently_selected, gpointer data)
{
	GtkTreeIter iter;
	if (gtk_tree_model_get_iter (model, &iter, path))
	{
		gint message_type;
		gchar *filename;
		gtk_tree_model_get (model, &iter,
				COL_OUTPUT_FILENAME, &filename,
				COL_OUTPUT_MESSAGE_TYPE, &message_type,
				-1);

		if (message_type != MESSAGE_TYPE_OTHER && filename != NULL
				&& strlen (filename) > 0)
			select_row (model, &iter);

		g_free (filename);
	}

	// rows will never be selected
	return FALSE;
}

static void
select_row (GtkTreeModel *model, GtkTreeIter *iter)
{
	GtkTreePath *current_path_selected = g_object_get_data (G_OBJECT (model),
			"row-selected");

	GtkTreePath *path = gtk_tree_model_get_path (model, iter);

	if (current_path_selected != NULL)
	{
		// if the row to select is not the same as the row already selected,
		// we must deselect this row
		if (gtk_tree_path_compare (current_path_selected, path) != 0)
		{
			GtkTreeIter current_iter_selected;
			gtk_tree_model_get_iter (model, &current_iter_selected,
					current_path_selected);

			// invert the colors
			gchar *bg_color;
			gtk_tree_model_get (model, &current_iter_selected,
					COL_OUTPUT_BG_COLOR, &bg_color,
					-1);
			gtk_list_store_set (GTK_LIST_STORE (model), &current_iter_selected,
					COL_OUTPUT_COLOR, bg_color,
					COL_OUTPUT_BG_COLOR, "white",
					COL_OUTPUT_BG_COLOR_SET, FALSE,
					-1);
			g_free (bg_color);
		}

		// the row is already selected
		else
		{
			gtk_tree_path_free (path);
			jump_to_file ();
			return;
		}
	}

	// invert the colors
	gchar *color;
	gtk_tree_model_get (model, iter,
			COL_OUTPUT_COLOR, &color,
			-1);
	gtk_list_store_set (GTK_LIST_STORE (model), iter,
			COL_OUTPUT_COLOR, "white",
			COL_OUTPUT_BG_COLOR, color,
			COL_OUTPUT_BG_COLOR_SET, TRUE,
			-1);
	g_free (color);

	g_object_set_data_full (G_OBJECT (model), "row-selected", path,
			(GDestroyNotify) path_free);
	jump_to_file ();
	set_previous_next_actions_sensitivity ();
}

static void
jump_to_file (void)
{
	GtkTreeModel *model = gtk_tree_view_get_model (output_view);
	GtkTreePath *path_selected = g_object_get_data (G_OBJECT (model),
			"row-selected");
	if (path_selected == NULL)
		return;

	GtkTreeIter iter;
	gtk_tree_model_get_iter (model, &iter, path_selected);

	gchar *filename = NULL;
	gchar *line_number = NULL;
	gint message_type;

	gtk_tree_model_get (model, &iter,
			COL_OUTPUT_FILENAME, &filename,
			COL_OUTPUT_LINE_NUMBER, &line_number,
			COL_OUTPUT_MESSAGE_TYPE, &message_type,
			-1);

	if (message_type != MESSAGE_TYPE_OTHER && filename != NULL
			&& strlen (filename) > 0)
	{
		// open the file (if the file is already opened, go to it)
		open_new_document_without_uri (filename);

		// we flush the queue here because without that, there is a bug when the
		// file is not already opened, scroll_to_cursor() doesn't work as
		// expected.
		flush_queue ();

		// go to line
		if (line_number != NULL && strlen (line_number) != 0
				&& latexila.active_doc != NULL)
		{
			gint num = strtol (line_number, NULL, 10);
			GtkTextIter iter_file;
			GtkTextBuffer *buffer = GTK_TEXT_BUFFER (
					latexila.active_doc->source_buffer);
			gtk_text_buffer_get_iter_at_line (buffer, &iter_file, --num);
			gtk_text_buffer_place_cursor (buffer, &iter_file);
			scroll_to_cursor ();
		}

		gtk_widget_grab_focus (latexila.active_doc->source_view);
	}

	g_free (filename);
	g_free (line_number);
}

static void
index_messages_by_type (GtkTreeModel *model)
{
	messages_index_t *index = g_object_get_data (G_OBJECT (model), "index");
	if (index == NULL)
		return;

	if (index->nb_errors == 0 && index->nb_warnings == 0
			&& index->nb_badboxes == 0)
		return;

	// the row number
	gint i = 0;

	// positions in the tables
	gint i_error = 0;
	gint i_warning = 0;
	gint i_badbox = 0;

	GtkTreeIter iter;
	gboolean valid = gtk_tree_model_get_iter_first (model, &iter);

	while (valid)
	{
		gint message_type;
		gtk_tree_model_get (model, &iter,
				COL_OUTPUT_MESSAGE_TYPE, &message_type,
				-1);

		switch (message_type)
		{
			case MESSAGE_TYPE_ERROR:
				index->errors[i_error] = i;
				i_error++;
				break;

			case MESSAGE_TYPE_WARNING:
				index->warnings[i_warning] = i;
				i_warning++;
				break;

			case MESSAGE_TYPE_BADBOX:
				index->badboxes[i_badbox] = i;
				i_badbox++;
				break;

			default:
				break;
		}

		valid = gtk_tree_model_iter_next (model, &iter);
		i++;
	}

	set_previous_next_actions_sensitivity ();
}

static void
set_previous_next_actions_sensitivity (void)
{
	GtkTreeModel *model = gtk_tree_view_get_model (output_view);
	messages_index_t *index = g_object_get_data (G_OBJECT (model), "index");

	if (index == NULL)
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_error, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_previous_warning, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_previous_badbox, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_error, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_warning, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_badbox, FALSE);
		return;
	}

	GtkTreePath *path = g_object_get_data (G_OBJECT (model), "row-selected");

	if (path == NULL)
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_error, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_previous_warning, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_previous_badbox, FALSE);

		gtk_action_set_sensitive (latexila.actions.go_next_error, index->nb_errors > 0);
		gtk_action_set_sensitive (latexila.actions.go_next_warning, index->nb_warnings > 0);
		gtk_action_set_sensitive (latexila.actions.go_next_badbox, index->nb_badboxes > 0);
		return;
	}

	gint *indices = gtk_tree_path_get_indices (path);
	gint indice = indices[0];

	// errors
	if (index->nb_errors > 0)
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_error,
				index->errors[0] < indice);
		gtk_action_set_sensitive (latexila.actions.go_next_error,
				index->errors[index->nb_errors - 1] > indice);
	}
	else
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_error, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_error, FALSE);
	}

	// warnings
	if (index->nb_warnings > 0)
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_warning,
				index->warnings[0] < indice);
		gtk_action_set_sensitive (latexila.actions.go_next_warning,
				index->warnings[index->nb_warnings - 1] > indice);
	}
	else
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_warning, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_warning, FALSE);
	}

	// badboxes
	if (index->nb_badboxes > 0)
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_badbox,
				index->badboxes[0] < indice);
		gtk_action_set_sensitive (latexila.actions.go_next_badbox,
				index->badboxes[index->nb_badboxes - 1] > indice);
	}
	else
	{
		gtk_action_set_sensitive (latexila.actions.go_previous_badbox, FALSE);
		gtk_action_set_sensitive (latexila.actions.go_next_badbox, FALSE);
	}
}

static void
path_free (gpointer data)
{
	gtk_tree_path_free ((GtkTreePath *) data);
}

static void
index_free (gpointer data)
{
	messages_index_t *index = (messages_index_t *) data;
	g_free (index->errors);
	g_free (index->warnings);
	g_free (index->badboxes);
	g_free (index);
}
