/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2017-2020 Sébastien Wilmet
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

public class Factory : Tepl.AbstractFactory
{
    public Factory ()
    {
    }

    public override unowned Gtk.ApplicationWindow? create_main_window (Gtk.Application gtk_app)
    {
        return_val_if_fail (gtk_app is GlatexApp, null);
        GlatexApp app = gtk_app as GlatexApp;

        MainWindow? active_main_window = app.get_active_main_window ();
        if (active_main_window != null)
            active_main_window.save_state ();

        bool first_window = active_main_window == null;

        MainWindow new_window = new MainWindow (app);
        if (first_window)
            reopen_files (app);

        return new_window.ref () as MainWindow;
    }

    private void reopen_files (GlatexApp app)
    {
        GLib.Settings editor_settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.editor");

        if (editor_settings.get_boolean ("reopen-files"))
        {
            GLib.Settings window_settings =
                new GLib.Settings ("org.gnome.gnome-latex.state.window");

            string[] uris = window_settings.get_strv ("documents");
            File[] files = {};
            foreach (string uri in uris)
            {
                if (0 < uri.length)
                    files += File.new_for_uri (uri);
            }

            app.open_documents (files);
        }
    }
}
