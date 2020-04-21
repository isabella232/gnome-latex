/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2010-2012 Sébastien Wilmet
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

using Gtk;
using Gee;

public class PreferencesDialog : Dialog
{
    private static PreferencesDialog _instance = null;

    delegate unowned string Plural (ulong n);

    private PreferencesDialog ()
    {
        Object (use_header_bar : 1);

        title = _("Preferences");
        destroy_with_parent = true;
        border_width = 5;

        HeaderBar headerbar = get_header_bar () as HeaderBar;

        headerbar.set_show_close_button (true);

        // reset all button
        Button reset_button = new Button.with_mnemonic (_("_Reset All"));
        reset_button.set_tooltip_text (_("Reset all preferences"));
        reset_button.show_all ();
        reset_button.clicked.connect (() => reset_all ());
        headerbar.pack_start (reset_button);

        /* load the UI */

        Builder builder = new Builder ();

        try
        {
            string ui_path = Path.build_filename (Config.DATA_DIR, "ui",
                "preferences_dialog.ui");
            builder.add_from_file (ui_path);
        }
        catch (Error e)
        {
            string message = "Error: %s".printf (e.message);
            warning ("%s", message);

            Label label_error = new Label (message);
            label_error.set_line_wrap (true);
            Box content_area = (Box) get_content_area ();
            content_area.pack_start (label_error);
            content_area.show_all ();
            return;
        }

        init_editor_tab (builder);
        init_font_and_colors_tab (builder);
        init_interactive_completion_setting (builder);
        init_spell_checking_settings (builder);
        init_other_tab (builder);

        // pack notebook
        Notebook notebook = builder.get_object ("notebook") as Notebook;
        notebook.unparent ();
        Box content_area = (Box) get_content_area ();
        content_area.pack_start (notebook);
    }

    public static void show_me (Window parent)
    {
        if (_instance == null)
        {
            _instance = new PreferencesDialog ();

            _instance.delete_event.connect (() =>
            {
                _instance.hide ();
                return true;
            });

            _instance.destroy.connect (() =>
            {
                _instance = null;
            });
        }

        if (parent != _instance.get_transient_for ())
            _instance.set_transient_for (parent);

        _instance.present ();
    }

    private void reset_all ()
    {
        Dialog dialog = new MessageDialog (this, DialogFlags.DESTROY_WITH_PARENT,
            MessageType.QUESTION, ButtonsType.NONE,
            "%s", _("Do you really want to reset all preferences?"));

        dialog.add_button (_("_Cancel"), ResponseType.CANCEL);
        dialog.add_button (_("_Reset All"), ResponseType.YES);

        int resp = dialog.run ();
        dialog.destroy ();
        if (resp != ResponseType.YES)
            return;

        string[] settings_str =
        {
            "org.gnome.gnome-latex.preferences.editor",
            "org.gnome.gnome-latex.preferences.latex"
        };

        foreach (string setting_str in settings_str)
        {
            GLib.Settings settings = new GLib.Settings (setting_str);
            string[] keys = settings.list_keys ();
            foreach (string key in keys)
                settings.reset (key);
        }
    }

    private void init_editor_tab (Builder builder)
    {
        GLib.Settings settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.editor");

        var display_line_nb_checkbutton =
            builder.get_object ("display_line_nb_checkbutton");
        settings.bind ("display-line-numbers", display_line_nb_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var tab_width_spinbutton =
            builder.get_object ("tab_width_spinbutton") as SpinButton;
        set_spin_button_range (tab_width_spinbutton, settings, "tabs-size");
        settings.bind ("tabs-size", tab_width_spinbutton, "value",
            SettingsBindFlags.DEFAULT);

        var insert_spaces_checkbutton = builder.get_object ("insert_spaces_checkbutton");
        settings.bind ("insert-spaces", insert_spaces_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        Widget forget_no_tabs = builder.get_object ("forget_no_tabs") as Widget;
        settings.bind ("forget-no-tabs", forget_no_tabs, "active",
            SettingsBindFlags.DEFAULT);
        insert_spaces_checkbutton.bind_property ("active", forget_no_tabs, "sensitive",
            BindingFlags.DEFAULT | BindingFlags.SYNC_CREATE);

        var hl_current_line_checkbutton =
            builder.get_object ("hl_current_line_checkbutton");
        settings.bind ("highlight-current-line", hl_current_line_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var bracket_matching_checkbutton =
            builder.get_object ("bracket_matching_checkbutton");
        settings.bind ("bracket-matching", bracket_matching_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var backup_checkbutton = builder.get_object ("backup_checkbutton");
        settings.bind ("create-backup-copy", backup_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var autosave_checkbutton = builder.get_object ("autosave_checkbutton");
        settings.bind ("auto-save", autosave_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var autosave_spinbutton =
            builder.get_object ("autosave_spinbutton") as SpinButton;
        set_spin_button_range (autosave_spinbutton, settings, "auto-save-interval");
        settings.bind ("auto-save-interval", autosave_spinbutton, "value",
            SettingsBindFlags.DEFAULT);
        autosave_checkbutton.bind_property ("active", autosave_spinbutton, "sensitive",
            BindingFlags.DEFAULT | BindingFlags.SYNC_CREATE);

        Label autosave_label = builder.get_object ("autosave_label") as Label;
        set_plural (autosave_label, settings, "auto-save-interval",
            (n) => ngettext ("minute", "minutes", n));

        var reopen_checkbutton = builder.get_object ("reopen_checkbutton");
        settings.bind ("reopen-files", reopen_checkbutton, "active",
            SettingsBindFlags.DEFAULT);
    }

    private void init_font_and_colors_tab (Builder builder)
    {
        GLib.Settings settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.editor");

        var default_font_checkbutton =
            builder.get_object ("default_font_checkbutton") as Button;
        settings.bind ("use-default-font", default_font_checkbutton, "active",
            SettingsBindFlags.DEFAULT);
        set_system_font_label (default_font_checkbutton);

        AppSettings app_settings = AppSettings.get_default ();
        app_settings.notify["system-font"].connect (() =>
        {
            set_system_font_label (default_font_checkbutton);
        });

        var font_button = builder.get_object ("font_button");
        settings.bind ("editor-font", font_button, "font-name",
            SettingsBindFlags.DEFAULT);

        var font_hbox = builder.get_object ("font_hbox") as Widget;
        default_font_checkbutton.bind_property ("active", font_hbox, "sensitive",
            BindingFlags.DEFAULT | BindingFlags.SYNC_CREATE |
            BindingFlags.INVERT_BOOLEAN);

        Tepl.StyleSchemeChooserWidget style_scheme_chooser =
            builder.get_object ("style_scheme_chooser") as Tepl.StyleSchemeChooserWidget;
        settings.bind ("scheme", style_scheme_chooser, "tepl-style-scheme-id",
            SettingsBindFlags.DEFAULT);
    }

    private void init_interactive_completion_setting (Builder builder)
    {
        GLib.Settings settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.latex");

        var interactive_comp_checkbutton =
            builder.get_object ("interactive_comp_checkbutton");
        settings.bind ("interactive-completion", interactive_comp_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        var interactive_comp_spinbutton =
            builder.get_object ("interactive_comp_spinbutton") as SpinButton;
        set_spin_button_range (interactive_comp_spinbutton, settings,
            "interactive-completion-num");
        settings.bind ("interactive-completion-num", interactive_comp_spinbutton, "value",
            SettingsBindFlags.DEFAULT);
        interactive_comp_checkbutton.bind_property ("active",
            interactive_comp_spinbutton, "sensitive",
            BindingFlags.DEFAULT | BindingFlags.SYNC_CREATE);

        Label interactive_comp_label =
            builder.get_object ("interactive_comp_label") as Label;
        set_plural (interactive_comp_label, settings, "interactive-completion-num",
            (n) => ngettext ("character", "characters", n));
    }

    private void init_spell_checking_settings (Builder builder)
    {
        GLib.Settings editor_settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.editor");

        Gspell.LanguageChooserButton spell_language_button =
            builder.get_object ("spell_language_button") as Gspell.LanguageChooserButton;
        editor_settings.bind ("spell-checking-language", spell_language_button,
            "language-code", SettingsBindFlags.DEFAULT);

        var inline_spell_checkbutton = builder.get_object ("inline_spell_checkbutton");
        editor_settings.bind ("highlight-misspelled-words", inline_spell_checkbutton,
            "active", SettingsBindFlags.DEFAULT);
    }

    private void init_other_tab (Builder builder)
    {
        GLib.Settings latex_settings =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.latex");

        var confirm_clean_up_checkbutton =
            builder.get_object ("confirm_clean_up_checkbutton");
        latex_settings.bind ("no-confirm-clean", confirm_clean_up_checkbutton, "active",
            SettingsBindFlags.DEFAULT);

        Widget auto_clean_up_checkbutton =
            builder.get_object ("auto_clean_up_checkbutton") as Widget;
        latex_settings.bind ("automatic-clean", auto_clean_up_checkbutton, "active",
            SettingsBindFlags.DEFAULT);
        confirm_clean_up_checkbutton.bind_property ("active", auto_clean_up_checkbutton,
            "sensitive", BindingFlags.DEFAULT | BindingFlags.SYNC_CREATE);

        var clean_up_entry = builder.get_object ("clean_up_entry");
        latex_settings.bind ("clean-extensions", clean_up_entry, "text",
            SettingsBindFlags.DEFAULT);
    }

    private void set_system_font_label (Button button)
    {
        AppSettings app_settings = AppSettings.get_default ();
        string label = _("Use the system fixed width font (%s)")
            .printf (app_settings.system_font);
        button.set_label (label);
    }

    private void set_plural (Label label, GLib.Settings settings, string key,
        Plural plural)
    {
        uint val;
        settings.get (key, "u", out val);
        label.label = plural (val);

        settings.changed[key].connect ((setting, k) =>
        {
            uint v;
            setting.get (k, "u", out v);
            label.label = plural (v);
        });
    }

    private void set_spin_button_range (SpinButton spin_button, GLib.Settings settings,
        string key)
    {
        GLib.SettingsSchema schema = settings.settings_schema;
        Variant range = schema.get_key (key).get_range ();

        string range_type;
        Variant range_contents;
        range.get ("(sv)", out range_type, out range_contents);

        return_if_fail (range_type == "range");

        uint min;
        uint max;
        range_contents.get ("(uu)", out min, out max);

        uint cur_value;
        settings.get (key, "u", out cur_value);

        Adjustment adjustment = new Adjustment ((double) cur_value, (double) min,
            (double) max, 1.0, 0, 0);
        spin_button.set_adjustment (adjustment);
    }
}
