/*
 * This file is part of GNOME LaTeX.
 *
 * From gedit-utils.c:
 * Copyright (C) 1998, 1999 - Alex Roberts, Evan Lawrence
 * Copyright (C) 2000, 2002 - Chema Celorio, Paolo Maggi
 * Copyright (C) 2003-2005 - Paolo Maggi
 *
 * Copyright (C) 2014-2020 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * GNOME LaTeX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNOME LaTeX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNOME LaTeX.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:utils
 * @title: LatexilaUtils
 * @short_description: Utility functions
 *
 * Various utility functions.
 */

#include "config.h"
#include "latexila-utils.h"
#include <string.h>
#include <tepl/tepl.h>
#include "latexila-synctex.h"

#if HAVE_DCONF_MIGRATION
#include "dh-dconf-migration.h"
#endif

/**
 * latexila_utils_register_icons:
 *
 * Register the GNOME LaTeX icons to the #GtkIconTheme as built-in icons. For
 * example the icon located at data/images/stock-icons/badbox.png in the
 * GNOME LaTeX git repository will be available with the icon name "badbox". The
 * "stock-icons" directory name is for historical reasons and should be changed
 * when stock icons are no longer used in GNOME LaTeX.
 *
 * TODO: prefix icon names with "glatex-", so the badbox.png would be
 * "glatex-badbox".
 */
void
latexila_utils_register_icons (void)
{
	gtk_icon_theme_add_resource_path (gtk_icon_theme_get_default (),
					  "/org/gnome/gnome-latex/stock-icons/");
}

/**
 * latexila_utils_get_pixbuf_from_icon_name:
 * @icon_name: an icon name.
 * @icon_size: an icon size.
 *
 * Returns: (nullable) (transfer full): the corresponding #GdkPixbuf.
 */
GdkPixbuf *
latexila_utils_get_pixbuf_from_icon_name (const gchar *icon_name,
					  GtkIconSize  icon_size)
{
	gint size;
	GdkPixbuf *pixbuf;
	GError *error = NULL;

	gtk_icon_size_lookup (icon_size, &size, NULL);

	pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
					   icon_name,
					   size,
					   0,
					   &error);

	if (error != NULL)
	{
		g_warning ("Error when loading icon \"%s\": %s", icon_name, error->message);
		g_error_free (error);
	}

	return pixbuf;
}

/**
 * latexila_utils_file_query_exists_async:
 * @file: a #GFile.
 * @cancellable: a #GCancellable.
 * @callback: the callback to call when the operation is finished.
 * @user_data: the data to pass to the callback function.
 *
 * The asynchronous version of g_file_query_exists(). When the operation is
 * finished, @callback will be called. You can then call
 * latexila_utils_file_query_exists_finish() to get the result of the operation.
 */
void
latexila_utils_file_query_exists_async (GFile               *file,
					GCancellable        *cancellable,
					GAsyncReadyCallback  callback,
					gpointer             user_data)
{
	g_file_query_info_async (file,
				 G_FILE_ATTRIBUTE_STANDARD_TYPE,
				 G_FILE_QUERY_INFO_NONE,
				 G_PRIORITY_DEFAULT,
				 cancellable,
				 callback,
				 user_data);
}

/**
 * latexila_utils_file_query_exists_finish:
 * @file: a #GFile.
 * @result: a #GAsyncResult.
 *
 * Finishes the operation started with latexila_utils_file_query_exists_async().
 * There is no output #GError parameter, so you should check if the operation
 * has been cancelled (in which case %FALSE will be returned).
 *
 * Returns: %TRUE if the file exists and the operation hasn't been cancelled,
 * %FALSE otherwise.
 */
gboolean
latexila_utils_file_query_exists_finish (GFile        *file,
					 GAsyncResult *result)
{
	GFileInfo *info = g_file_query_info_finish (file, result, NULL);

	if (info != NULL)
	{
		g_object_unref (info);
		return TRUE;
	}

	return FALSE;
}

static gboolean
default_document_viewer_is_evince (const gchar *uri)
{
	GFile *file;
	GAppInfo *app_info;
	const gchar *executable;
	gboolean ret;
	GError *error = NULL;

	file = g_file_new_for_uri (uri);
	app_info = g_file_query_default_handler (file, NULL, &error);
	g_object_unref (file);

	if (error != NULL)
	{
		g_warning ("Impossible to know if evince is the default document viewer: %s",
			   error->message);

		g_error_free (error);
		return FALSE;
	}

	executable = g_app_info_get_executable (app_info);
	ret = strstr (executable, "evince") != NULL;

	g_object_unref (app_info);
	return ret;
}

/**
 * latexila_utils_show_uri:
 * @widget: (nullable): the associated #GtkWidget, or %NULL.
 * @uri: the URI to show.
 * @timestamp: a timestamp.
 * @error: (out) (optional): a %NULL #GError, or %NULL.
 *
 * Shows the @uri. If the URI is a PDF file and if Evince is the default
 * document viewer, this function also connects the Evince window so the
 * backward search works (switch from the PDF to the source file).
 */
void
latexila_utils_show_uri (GtkWidget    *widget,
			 const gchar  *uri,
			 guint32       timestamp,
			 GError      **error)
{
	GtkWindow *parent = NULL;

	g_return_if_fail (widget == NULL || GTK_IS_WIDGET (widget));
	g_return_if_fail (uri != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	if (widget != NULL)
	{
		GtkWidget *toplevel;

		toplevel = gtk_widget_get_toplevel (widget);
		if (gtk_widget_is_toplevel (toplevel) &&
		    GTK_IS_WINDOW (toplevel))
		{
			parent = GTK_WINDOW (toplevel);
		}
	}

	if (gtk_show_uri_on_window (parent, uri, timestamp, error))
	{
		gchar *extension = tepl_utils_get_file_extension (uri);

		if (g_strcmp0 (extension, ".pdf") == 0 &&
		    default_document_viewer_is_evince (uri))
		{
			LatexilaSynctex *synctex = latexila_synctex_get_instance ();
			latexila_synctex_connect_evince_window (synctex, uri);
		}

		g_free (extension);
	}
}

/**
 * latexila_utils_get_dialog_component:
 * @title: the title of the dialog component.
 * @widget: the widget displayed below the title.
 *
 * Gets a #GtkDialog component. When a dialog contains several components, or
 * logical groups, this function is useful to attach the @widget with a @title.
 * The title will be in bold, left-aligned, and the widget will have a left
 * margin.
 *
 * Returns: (transfer floating): the dialog component containing the @title and
 * the @widget.
 */
GtkWidget *
latexila_utils_get_dialog_component (const gchar *title,
				     GtkWidget   *widget)
{
	GtkContainer *grid;
	GtkWidget *label;
	gchar *markup;

	grid = GTK_CONTAINER (gtk_grid_new ());
	gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
	gtk_grid_set_row_spacing (GTK_GRID (grid), 6);
	gtk_container_set_border_width (grid, 6);

	/* Title in bold, left-aligned. */
	label = gtk_label_new (NULL);
	markup = g_strdup_printf ("<b>%s</b>", title);
	gtk_label_set_markup (GTK_LABEL (label), markup);
	gtk_widget_set_halign (label, GTK_ALIGN_START);
	gtk_container_add (grid, label);

	/* Left margin for the widget. */
	gtk_widget_set_margin_start (widget, 12);
	gtk_container_add (grid, widget);

	g_free (markup);
	return GTK_WIDGET (grid);
}

/**
 * latexila_utils_join_widgets:
 * @widget_top: the #GtkWidget at the top.
 * @widget_bottom: the #GtkWidget at the bottom.
 *
 * Joins two widgets vertically, with junction sides.
 *
 * Returns: (transfer floating): a #GtkContainer containing @widget_top and
 * @widget_bottom.
 */
/* TODO try to simply add the GTK_STYLE_CLASS_LINKED class to the
 * GtkStyleContext of a GtkGrid.
 */
GtkWidget *
latexila_utils_join_widgets (GtkWidget *widget_top,
			     GtkWidget *widget_bottom)
{
	GtkStyleContext *context;
	GtkBox *vbox;

	g_return_val_if_fail (GTK_IS_WIDGET (widget_top), NULL);
	g_return_val_if_fail (GTK_IS_WIDGET (widget_bottom), NULL);

	context = gtk_widget_get_style_context (widget_top);
	gtk_style_context_set_junction_sides (context, GTK_JUNCTION_BOTTOM);

	context = gtk_widget_get_style_context (widget_bottom);
	gtk_style_context_set_junction_sides (context, GTK_JUNCTION_TOP);

	vbox = GTK_BOX (gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));
	gtk_box_pack_start (vbox, widget_top, TRUE, TRUE, 0);
	gtk_box_pack_start (vbox, widget_bottom, FALSE, FALSE, 0);

	return GTK_WIDGET (vbox);
}

static void
migrate_latexila_to_gnome_latex_gsettings (void)
{
#if HAVE_DCONF_MIGRATION
	DhDconfMigration *migration;
	gint i;

	const gchar *keys[] =
	{
		"preferences/editor/auto-save",
		"preferences/editor/auto-save-interval",
		"preferences/editor/bracket-matching",
		"preferences/editor/create-backup-copy",
		"preferences/editor/display-line-numbers",
		"preferences/editor/editor-font",
		"preferences/editor/forget-no-tabs",
		"preferences/editor/highlight-current-line",
		"preferences/editor/highlight-misspelled-words",
		"preferences/editor/insert-spaces",
		"preferences/editor/reopen-files",
		"preferences/editor/scheme",
		"preferences/editor/spell-checking-language",
		"preferences/editor/tabs-size",
		"preferences/editor/use-default-font",
		"preferences/file-browser/current-directory",
		"preferences/file-browser/show-build-files",
		"preferences/file-browser/show-hidden-files",
		"preferences/latex/automatic-clean",
		"preferences/latex/clean-extensions",
		"preferences/latex/disabled-default-build-tools",
		"preferences/latex/enabled-default-build-tools",
		"preferences/latex/interactive-completion",
		"preferences/latex/interactive-completion-num",
		"preferences/latex/no-confirm-clean",
		"preferences/ui/bottom-panel-visible",
		"preferences/ui/edit-toolbar-visible",
		"preferences/ui/main-toolbar-visible",
		"preferences/ui/show-build-badboxes",
		"preferences/ui/show-build-warnings",
		"preferences/ui/side-panel-component",
		"preferences/ui/side-panel-visible",
		"state/window/documents",
		"state/window/side-panel-size",
		"state/window/size",
		"state/window/state",
		"state/window/structure-paned-position",
		"state/window/vertical-paned-position",
		NULL
	};

	migration = _dh_dconf_migration_new ();

	for (i = 0; keys[i] != NULL; i++)
	{
		const gchar *cur_key = keys[i];
		gchar *cur_glatex_key;
		gchar *cur_latexila_key;

		cur_glatex_key = g_strconcat ("/org/gnome/gnome-latex/", cur_key, NULL);
		cur_latexila_key = g_strconcat ("/org/gnome/latexila/", cur_key, NULL);

		_dh_dconf_migration_migrate_key (migration,
						 cur_glatex_key,
						 cur_latexila_key,
						 NULL);

		g_free (cur_glatex_key);
		g_free (cur_latexila_key);
	}

	_dh_dconf_migration_sync_and_free (migration);
#else
	g_warning ("LaTeXila -> GNOME LaTeX dconf migration not supported.");
#endif
}

static void
migrate_latexila_to_gnome_latex_copy_file (GFile *latexila_file,
					   GFile *glatex_file)
{
	GError *error = NULL;

	tepl_utils_create_parent_directories (glatex_file, NULL, &error);
	if (error != NULL)
	{
		goto out;
	}

	g_file_copy (latexila_file,
		     glatex_file,
		     G_FILE_COPY_TARGET_DEFAULT_PERMS,
		     NULL, NULL, NULL,
		     &error);

	if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND) ||
	    g_error_matches (error, G_IO_ERROR, G_IO_ERROR_EXISTS))
	{
		g_clear_error (&error);
	}

out:
	if (error != NULL)
	{
		g_warning ("Error when migrating LaTeXila to GNOME LaTeX user data file: %s",
			   error->message);
		g_clear_error (&error);
	}
}

static void
migrate_latexila_to_gnome_latex_most_used_symbols (void)
{
	GFile *latexila_file;
	GFile *glatex_file;

	latexila_file = g_file_new_build_filename (g_get_user_data_dir (),
						   "latexila",
						   "most_used_symbols.xml",
						   NULL);

	glatex_file = g_file_new_build_filename (g_get_user_data_dir (),
						 "gnome-latex",
						 "most_used_symbols.xml",
						 NULL);

	migrate_latexila_to_gnome_latex_copy_file (latexila_file, glatex_file);

	g_object_unref (latexila_file);
	g_object_unref (glatex_file);
}

static void
migrate_latexila_to_gnome_latex_projects (void)
{
	GFile *latexila_file;
	GFile *glatex_file;

	latexila_file = g_file_new_build_filename (g_get_user_data_dir (),
						   "latexila",
						   "projects.xml",
						   NULL);

	glatex_file = g_file_new_build_filename (g_get_user_data_dir (),
						 "gnome-latex",
						 "projects.xml",
						 NULL);

	migrate_latexila_to_gnome_latex_copy_file (latexila_file, glatex_file);

	g_object_unref (latexila_file);
	g_object_unref (glatex_file);
}

static void
migrate_latexila_to_gnome_latex_personal_build_tools (void)
{
	GFile *latexila_file;
	GFile *glatex_file;

	latexila_file = g_file_new_build_filename (g_get_user_config_dir (),
						   "latexila",
						   "build_tools.xml",
						   NULL);

	glatex_file = g_file_new_build_filename (g_get_user_config_dir (),
						 "gnome-latex",
						 "build_tools.xml",
						 NULL);

	migrate_latexila_to_gnome_latex_copy_file (latexila_file, glatex_file);

	g_object_unref (latexila_file);
	g_object_unref (glatex_file);
}

static void
migrate_latexila_to_gnome_latex_personal_templates_tex_files (void)
{
	GFile *latexila_dir;
	GFile *glatex_dir;
	GFileEnumerator *enumerator;
	GError *error = NULL;

	latexila_dir = g_file_new_build_filename (g_get_user_data_dir (),
						  "latexila",
						  NULL);
	glatex_dir = g_file_new_build_filename (g_get_user_data_dir (),
						"gnome-latex",
						NULL);

	enumerator = g_file_enumerate_children (latexila_dir,
						G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
						G_FILE_QUERY_INFO_NONE,
						NULL,
						&error);

	if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
	{
		g_clear_error (&error);
		goto out;
	}

	if (enumerator == NULL || error != NULL)
	{
		goto out;
	}

	while (TRUE)
	{
		GFileInfo *child_file_info;
		GFile *child_file;
		const gchar *child_name;
		GFile *glatex_child_file;

		g_file_enumerator_iterate (enumerator, &child_file_info, &child_file, NULL, &error);
		if (child_file == NULL || error != NULL)
		{
			break;
		}

		child_name = g_file_info_get_display_name (child_file_info);
		if (child_name == NULL || !g_str_has_suffix (child_name, ".tex"))
		{
			continue;
		}

		glatex_child_file = g_file_get_child (glatex_dir, child_name);
		migrate_latexila_to_gnome_latex_copy_file (child_file, glatex_child_file);
		g_object_unref (glatex_child_file);
	}

out:
	if (error != NULL)
	{
		g_warning ("Error when migrating LaTeXila to GNOME LaTeX personal templates: %s",
			   error->message);
		g_clear_error (&error);
	}

	g_object_unref (latexila_dir);
	g_object_unref (glatex_dir);
	g_clear_object (&enumerator);
}

#define TEMPLATES_RC_FILE_OLD_GROUP_NAME "[LaTeXila]\n"
#define TEMPLATES_RC_FILE_NEW_GROUP_NAME "[Personal templates]\n"

static void
migrate_latexila_to_gnome_latex_personal_templates_rc_file (void)
{
	GFile *latexila_file;
	gchar *content = NULL;
	GFile *glatex_file = NULL;
	GFileOutputStream *output_stream = NULL;
	GError *error = NULL;

	/* Load old RC file. */
	latexila_file = g_file_new_build_filename (g_get_user_data_dir (),
						   "latexila",
						   "templatesrc",
						   NULL);

	g_file_load_contents (latexila_file, NULL, &content, NULL, NULL, &error);

	if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
	{
		g_clear_error (&error);
		goto out;
	}

	if (error != NULL || content == NULL)
	{
		goto out;
	}

	/* Modify group name. */
	if (g_str_has_prefix (content, TEMPLATES_RC_FILE_OLD_GROUP_NAME))
	{
		gchar *modified_content;

		modified_content = g_strconcat (TEMPLATES_RC_FILE_NEW_GROUP_NAME,
						content + strlen (TEMPLATES_RC_FILE_OLD_GROUP_NAME),
						NULL);
		g_free (content);
		content = modified_content;
	}

	/* Save to new location. */
	glatex_file = g_file_new_build_filename (g_get_user_data_dir (),
						 "gnome-latex",
						 "templatesrc",
						 NULL);

	output_stream = g_file_create (glatex_file, G_FILE_CREATE_NONE, NULL, &error);

	if (g_error_matches (error, G_IO_ERROR, G_IO_ERROR_EXISTS))
	{
		g_clear_error (&error);
		goto out;
	}

	if (error != NULL || output_stream == NULL)
	{
		goto out;
	}

	g_output_stream_write_all (G_OUTPUT_STREAM (output_stream),
				   content,
				   strlen (content),
				   NULL,
				   NULL,
				   &error);

out:
	if (error != NULL)
	{
		g_warning ("Error when migrating LaTeXila to GNOME LaTeX personal templates: %s",
			   error->message);
		g_clear_error (&error);
	}

	g_object_unref (latexila_file);
	g_free (content);
	g_clear_object (&glatex_file);
	g_clear_object (&output_stream);
}

static void
migrate_latexila_to_gnome_latex_personal_templates (void)
{
	migrate_latexila_to_gnome_latex_personal_templates_tex_files ();
	migrate_latexila_to_gnome_latex_personal_templates_rc_file ();
}

/**
 * latexila_utils_migrate_latexila_to_gnome_latex:
 *
 * Migrates the #GSettings values and user data/config files from LaTeXila to
 * GNOME LaTeX, so that users don't lose all their settings.
 *
 * TODO in >= 2025: delete this code.
 */
void
latexila_utils_migrate_latexila_to_gnome_latex (void)
{
	GSettings *settings;

	settings = g_settings_new ("org.gnome.gnome-latex");

	if (!g_settings_get_boolean (settings, "latexila-to-gnome-latex-migration-done"))
	{
		migrate_latexila_to_gnome_latex_gsettings ();
		migrate_latexila_to_gnome_latex_most_used_symbols ();
		migrate_latexila_to_gnome_latex_projects ();
		migrate_latexila_to_gnome_latex_personal_build_tools ();
		migrate_latexila_to_gnome_latex_personal_templates ();

		g_settings_set_boolean (settings, "latexila-to-gnome-latex-migration-done", TRUE);
	}

	g_object_unref (settings);
}
