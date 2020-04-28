/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2010-2020 Sébastien Wilmet
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
 *
 * Author: Sébastien Wilmet
 */

public class GlatexApp : Latexila.App
{
    static Gtk.CssProvider? _provider = null;

    private const GLib.ActionEntry[] _app_actions =
    {
        { "new-document", new_document_cb },
        { "preferences", preferences_cb },
        { "manage-build-tools", manage_build_tools_cb },
        { "quit", quit_cb }
    };

    public GlatexApp ()
    {
        startup.connect (startup_cb);
        open.connect (open_documents);
        shutdown.connect (shutdown_cb);
    }

    public static GlatexApp get_instance ()
    {
        return GLib.Application.get_default () as GlatexApp;
    }

    public MainWindow? get_active_main_window ()
    {
        Tepl.Application tepl_app = Tepl.Application.get_from_gtk_application (this);
        Gtk.ApplicationWindow? main_window = tepl_app.get_active_main_window ();

        if (main_window == null)
            return null;

        return main_window as MainWindow;
    }

    private void startup_cb ()
    {
        hold ();

        add_action_entries (_app_actions, this);

        setup_theme_extensions ();
        AppSettings.get_default ();
        support_backward_search ();

        release ();
    }

    private void shutdown_cb ()
    {
        hold ();
        Projects.get_default ().save ();
        MostUsedSymbols.get_default ().save ();
        release ();
    }

    private void new_document_cb ()
    {
        MainWindow? window = get_active_main_window ();
        if (window == null)
            window = create_window ();

        window.create_tab (true);
    }

    private void preferences_cb ()
    {
        PreferencesDialog.show_me (get_active_main_window ());
    }

    private void manage_build_tools_cb ()
    {
        new BuildToolsPreferences (get_active_main_window ());
    }

    private void quit_cb ()
    {
        hold ();

        bool cont = true;
        while (cont)
        {
            MainWindow? main_window = get_active_main_window ();
            if (main_window == null)
                break;

            main_window.present ();
            cont = main_window.quit ();
        }

        if (cont)
        {
            while (this.active_window != null)
                this.active_window.destroy ();
        }

        release ();
    }

    private void setup_theme_extensions ()
    {
        Gtk.Settings settings = Gtk.Settings.get_default ();
        settings.notify["gtk-theme-name"].connect (update_theme);
        update_theme ();
    }

    private void update_theme ()
    {
        Gtk.Settings settings = Gtk.Settings.get_default ();
        Gdk.Screen screen = Gdk.Screen.get_default ();

        if (settings.gtk_theme_name == "Adwaita")
        {
            if (_provider == null)
            {
                _provider = new Gtk.CssProvider ();
                File file = File.new_for_uri ("resource:///org/gnome/gnome-latex/ui/gnome-latex.adwaita.css");
                try
                {
                    _provider.load_from_file (file);
                }
                catch (Error e)
                {
                    warning ("Cannot load CSS: %s", e.message);
                }
            }

            Gtk.StyleContext.add_provider_for_screen (screen, _provider,
                Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
        }
        else if (_provider != null)
        {
            Gtk.StyleContext.remove_provider_for_screen (screen, _provider);
            _provider = null;
        }
    }

    // Get all the documents currently opened.
    public Gee.List<Document> get_documents ()
    {
        Gee.List<Document> all_documents = new Gee.LinkedList<Document> ();
        foreach (Gtk.Window window in get_windows ())
        {
            if (window is MainWindow)
            {
                MainWindow main_window = window as MainWindow;
                all_documents.add_all (main_window.get_documents ());
            }
        }

        return all_documents;
    }

    // Get all the document views.
    public Gee.List<DocumentView> get_views ()
    {
        Gee.List<DocumentView> all_views = new Gee.LinkedList<DocumentView> ();
        foreach (Gtk.Window window in get_windows ())
        {
            if (window is MainWindow)
            {
                MainWindow main_window = window as MainWindow;
                all_views.add_all (main_window.get_views ());
            }
        }

        return all_views;
    }

    public MainWindow create_window ()
    {
        Tepl.AbstractFactory factory = Tepl.AbstractFactory.get_singleton ();

        return factory.create_main_window (this) as MainWindow;
    }

    public void open_documents (File[] files)
    {
        MainWindow? main_window = get_active_main_window ();
        if (main_window == null)
            main_window = create_window ();

        bool jump_to = true;
        foreach (File file in files)
        {
            main_window.open_document (file, jump_to);
            jump_to = false;
        }

        main_window.present ();
    }

    private void support_backward_search ()
    {
        Latexila.Synctex synctex = Latexila.Synctex.get_instance ();

        synctex.backward_search.connect ((tex_uri, line, timestamp) =>
        {
            File tex_file = File.new_for_uri (tex_uri);
            if (!tex_file.query_exists ())
            {
                warning (@"Backward search: the file \"$tex_uri\" doesn't exist.");
                return;
            }

            // TODO choose the right MainWindow, if tex_file is already opened
            // in another window.
            MainWindow? main_window = get_active_main_window ();
            if (main_window != null)
            {
                main_window.jump_to_file_position (tex_file, line, line);
                main_window.present_with_time (timestamp);
            }
        });
    }
}
