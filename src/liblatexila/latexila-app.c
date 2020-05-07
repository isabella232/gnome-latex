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

struct _LatexilaAppPrivate
{
	GtkCssProvider *adwaita_css_provider;
};

/**
 * SECTION:app
 * @title: LatexilaApp
 * @short_description: Subclass of #GtkApplication
 */

G_DEFINE_TYPE_WITH_PRIVATE (LatexilaApp, latexila_app, GTK_TYPE_APPLICATION)

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
			       "logo-icon-name", "org.gnome.gnome-latex",
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
add_main_option_entries (LatexilaApp *app)
{
	const GOptionEntry options[] =
	{
		{ "version", 'V', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, NULL,
		  N_("Show the application’s version") },

		{ "new-window", '\0', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, NULL,
		  N_("Create a new top-level window in an existing instance of GNOME LaTeX") },

		{ "new-document", 'n', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, NULL,
		  N_("Create a new document in an existing instance of GNOME LaTeX") },

		{ NULL }
	};

	g_application_add_main_option_entries (G_APPLICATION (app), options);
}

static gint
latexila_app_handle_local_options (GApplication *app,
				   GVariantDict *options)
{
	GError *error = NULL;

	if (g_variant_dict_contains (options, "version"))
	{
		g_print ("%s %s\n", g_get_application_name (), PACKAGE_VERSION);
		return 0;
	}

	g_application_register (app, NULL, &error);
	if (error != NULL)
	{
		g_warning ("Failed to register the application: %s", error->message);
		g_clear_error (&error);
	}

	if (g_variant_dict_contains (options, "new-window"))
	{
		g_action_group_activate_action (G_ACTION_GROUP (app), "tepl-new-window", NULL);
	}

	if (g_variant_dict_contains (options, "new-document"))
	{
		g_action_group_activate_action (G_ACTION_GROUP (app), "new-document", NULL);
	}

	if (G_APPLICATION_CLASS (latexila_app_parent_class)->handle_local_options != NULL)
	{
		return G_APPLICATION_CLASS (latexila_app_parent_class)->handle_local_options (app, options);
	}

	return -1;
}

static void
update_theme (LatexilaApp *app)
{
	GtkSettings *settings = gtk_settings_get_default ();
	gchar *gtk_theme_name;

	g_object_get (settings,
		      "gtk-theme-name", &gtk_theme_name,
		      NULL);

	if (g_strcmp0 (gtk_theme_name, "Adwaita") == 0)
	{
		if (app->priv->adwaita_css_provider == NULL)
		{
			app->priv->adwaita_css_provider = gtk_css_provider_new ();
			gtk_css_provider_load_from_resource (app->priv->adwaita_css_provider,
							     "/org/gnome/gnome-latex/ui/gnome-latex.adwaita.css");
		}

		gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
							   GTK_STYLE_PROVIDER (app->priv->adwaita_css_provider),
							   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	}
	else if (app->priv->adwaita_css_provider != NULL)
	{
		gtk_style_context_remove_provider_for_screen (gdk_screen_get_default (),
							      GTK_STYLE_PROVIDER (app->priv->adwaita_css_provider));
		g_clear_object (&app->priv->adwaita_css_provider);
	}

	g_free (gtk_theme_name);
}

static void
gtk_theme_name_notify_cb (GtkSettings *settings,
			  GParamSpec  *pspec,
			  LatexilaApp *app)
{
	update_theme (app);
}

static void
setup_theme_extension (LatexilaApp *app)
{
	GtkSettings *settings = gtk_settings_get_default ();

	g_signal_connect_object (settings,
				 "notify::gtk-theme-name",
				 G_CALLBACK (gtk_theme_name_notify_cb),
				 app,
				 0);

	update_theme (app);
}

static void
latexila_app_startup (GApplication *g_app)
{
	LatexilaApp *app = LATEXILA_APP (g_app);

	if (G_APPLICATION_CLASS (latexila_app_parent_class)->startup != NULL)
	{
		G_APPLICATION_CLASS (latexila_app_parent_class)->startup (g_app);
	}

	latexila_utils_migrate_latexila_to_gnome_latex ();
	add_action_entries (app);
	latexila_latex_commands_add_action_infos (GTK_APPLICATION (app));
	latexila_utils_register_icons ();
	setup_theme_extension (app);
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
latexila_app_dispose (GObject *object)
{
	LatexilaApp *app = LATEXILA_APP (object);

	g_clear_object (&app->priv->adwaita_css_provider);

	G_OBJECT_CLASS (latexila_app_parent_class)->dispose (object);
}

static void
latexila_app_class_init (LatexilaAppClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GApplicationClass *gapp_class = G_APPLICATION_CLASS (klass);

	object_class->constructed = latexila_app_constructed;
	object_class->dispose = latexila_app_dispose;

	gapp_class->handle_local_options = latexila_app_handle_local_options;
	gapp_class->startup = latexila_app_startup;
}

static void
latexila_app_init (LatexilaApp *app)
{
	TeplApplication *tepl_app;

	app->priv = latexila_app_get_instance_private (app);

	g_application_set_flags (G_APPLICATION (app), G_APPLICATION_HANDLES_OPEN);
	g_set_application_name (PACKAGE_NAME);
	gtk_window_set_default_icon_name ("org.gnome.gnome-latex");

	add_main_option_entries (app);

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
