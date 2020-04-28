/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2020 - Sébastien Wilmet <swilmet@gnome.org>
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

#include "config.h"
#include "latexila-app.h"
#include <glib/gi18n.h>
#include <tepl/tepl.h>
#include "latexila-latex-commands.h"
#include "latexila-utils.h"

/**
 * SECTION:app
 * @title: LatexilaApp
 * @short_description: Subclass of #GtkApplication
 */

G_DEFINE_TYPE (LatexilaApp, latexila_app, GTK_TYPE_APPLICATION)

static void
about_activate_cb (GSimpleAction *about_action,
		   GVariant      *parameter,
		   gpointer       user_data)
{
	LatexilaApp *app = LATEXILA_APP (user_data);
	TeplApplication *tepl_app = tepl_application_get_from_gtk_application (GTK_APPLICATION (app));
	GtkApplicationWindow *active_main_window;

	const gchar *authors[] = {
		"Sébastien Wilmet <swilmet@gnome.org>",
		NULL
	};

	const gchar *artists[] = {
		"Eric Forgeot <e.forgeot@laposte.net>",
		"Sébastien Wilmet <swilmet@gnome.org>",
		"Alexander Wilms <f.alexander.wilms@gmail.com>",
		"The Kile Team http://kile.sourceforge.net/",
		"Gedit LaTeX Plugin https://wiki.gnome.org/Apps/Gedit/LaTeXPlugin",
		NULL
	};

	active_main_window = tepl_application_get_active_main_window (tepl_app);

	gtk_show_about_dialog (GTK_WINDOW (active_main_window),
			       "version", PACKAGE_VERSION,
			       "authors", authors,
			       "artists", artists,
			       "comments", _("GNOME LaTeX is a LaTeX editor for the GNOME desktop"),
			       "copyright", "Copyright 2009-2020 – Sébastien Wilmet",
			       "license-type", GTK_LICENSE_GPL_3_0,
			       "title", _("About GNOME LaTeX"),
			       "translator-credits", _("translator-credits"),
			       "website", "https://wiki.gnome.org/Apps/GNOME-LaTeX",
			       "logo-icon-name", "gnome-latex",
			       NULL);
}

static void
help_activate_cb (GSimpleAction *help_action,
		  GVariant      *parameter,
		  gpointer       user_data)
{
	LatexilaApp *app = LATEXILA_APP (user_data);
	TeplApplication *tepl_app = tepl_application_get_from_gtk_application (GTK_APPLICATION (app));
	GtkApplicationWindow *active_main_window;
	GError *error = NULL;

	active_main_window = tepl_application_get_active_main_window (tepl_app);

	gtk_show_uri_on_window (GTK_WINDOW (active_main_window),
				"help:gnome-latex",
				GDK_CURRENT_TIME,
				&error);

	if (error != NULL)
	{
		tepl_utils_show_warning_dialog (GTK_WINDOW (active_main_window),
						_("Impossible to open the documentation: %s"),
						error->message);
		g_clear_error (&error);
	}
}

static void
add_action_entries (LatexilaApp *app)
{
	const GActionEntry app_entries[] =
	{
		{ "about", about_activate_cb },
		{ "help", help_activate_cb },
	};

	amtk_action_map_add_action_entries_check_dups (G_ACTION_MAP (app),
						       app_entries,
						       G_N_ELEMENTS (app_entries),
						       app);
}

static void
latexila_app_startup (GApplication *app)
{
	if (G_APPLICATION_CLASS (latexila_app_parent_class)->startup != NULL)
	{
		G_APPLICATION_CLASS (latexila_app_parent_class)->startup (app);
	}

	latexila_utils_migrate_latexila_to_gnome_latex ();

	add_action_entries (LATEXILA_APP (app));
	latexila_latex_commands_add_action_infos (GTK_APPLICATION (app));

	latexila_utils_register_icons ();
}

static void
latexila_app_constructed (GObject *object)
{
	g_application_set_application_id (G_APPLICATION (object), "org.gnome.gnome-latex");

	if (G_OBJECT_CLASS (latexila_app_parent_class)->constructed != NULL)
	{
		G_OBJECT_CLASS (latexila_app_parent_class)->constructed (object);
	}
}

static void
latexila_app_class_init (LatexilaAppClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GApplicationClass *gapp_class = G_APPLICATION_CLASS (klass);

	object_class->constructed = latexila_app_constructed;

	gapp_class->startup = latexila_app_startup;
}

static void
latexila_app_init (LatexilaApp *app)
{
	TeplApplication *tepl_app;

	g_application_set_flags (G_APPLICATION (app), G_APPLICATION_HANDLES_OPEN);
	g_set_application_name (PACKAGE_NAME);
	gtk_window_set_default_icon_name ("gnome-latex");

	tepl_app = tepl_application_get_from_gtk_application (GTK_APPLICATION (app));
	tepl_application_handle_activate (tepl_app);
	tepl_application_handle_metadata (tepl_app);
}

/**
 * latexila_app_new:
 *
 * Returns: (transfer full): a new #LatexilaApp object.
 */
LatexilaApp *
latexila_app_new (void)
{
	return g_object_new (LATEXILA_TYPE_APP, NULL);
}
