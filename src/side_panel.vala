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

public class SidePanel : Grid
{
    private Gtk.Stack _stack;

    public SidePanel ()
    {
        _stack = new Gtk.Stack ();

        margin_start = 6;

        Gtk.StackSwitcher stack_switcher = new Gtk.StackSwitcher ();
        stack_switcher.set_stack (_stack);

        Gtk.ActionBar action_bar = new Gtk.ActionBar ();
        action_bar.set_center_widget (stack_switcher);
        action_bar.pack_end (get_close_button ());

        attach (action_bar, 0, 0, 1, 1);
        attach (_stack, 0, 1, 1, 1);
        show_all ();
    }

    private Button get_close_button ()
    {
        Button close_button = Tepl.utils_create_close_button () as Button;
        close_button.tooltip_text = _("Hide panel");

        close_button.clicked.connect (() => this.hide ());

        return close_button;
    }

    public void add_component (Grid component, string name, string title, string icon_name)
    {
        component.show ();
        Tepl.stack_add_component (_stack, component, name, title, icon_name);
    }

    public void restore_state ()
    {
        GLib.Settings settings = new GLib.Settings ("org.gnome.gnome-latex.preferences.ui");
        Tepl.stack_bind_setting (_stack, settings, "side-panel-component");
    }
}
