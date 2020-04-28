/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2010-2011 Sébastien Wilmet
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

public class AppSettings : GLib.Settings
{
    private static AppSettings instance = null;

    private Settings editor;

    public string system_font { get; private set; }

    /* AppSettings is a singleton */
    private AppSettings ()
    {
        Object (schema_id: "org.gnome.gnome-latex");
        initialize ();
    }

    public static AppSettings get_default ()
    {
        if (instance == null)
            instance = new AppSettings ();
        return instance;
    }

    private void initialize ()
    {
        Settings prefs = get_child ("preferences");
        editor = prefs.get_child ("editor");

        Settings desktop_interface = new Settings ("org.gnome.desktop.interface");
        system_font = desktop_interface.get_string ("monospace-font-name");

        desktop_interface.changed["monospace-font-name"].connect ((setting, key) =>
        {
            system_font = setting.get_string (key);
            if (editor.get_boolean ("use-default-font"))
                set_font (system_font);
        });

        editor.changed["use-default-font"].connect ((setting, key) =>
        {
            bool val = setting.get_boolean (key);
            string font = val ? system_font : editor.get_string ("editor-font");
            set_font (font);
        });

        editor.changed["editor-font"].connect ((setting, key) =>
        {
            if (editor.get_boolean ("use-default-font"))
                return;
            set_font (setting.get_string (key));
        });

        editor.changed["bracket-matching"].connect ((setting, key) =>
        {
            bool val = setting.get_boolean (key);

            foreach (Document doc in GlatexApp.get_instance ().get_documents ())
                doc.highlight_matching_brackets = val;
        });

        editor.changed["auto-save"].connect ((setting, key) =>
        {
            bool val = setting.get_boolean (key);

            foreach (Document doc in GlatexApp.get_instance ().get_documents ())
                doc.tab.auto_save = val;
        });

        editor.changed["auto-save-interval"].connect ((setting, key) =>
        {
            uint val;
            setting.get (key, "u", out val);

            foreach (Document doc in GlatexApp.get_instance ().get_documents ())
                doc.tab.auto_save_interval = val;
        });
    }

    private void set_font (string font)
    {
        foreach (DocumentView view in GlatexApp.get_instance ().get_views ())
            view.set_font_from_string (font);
    }
}
