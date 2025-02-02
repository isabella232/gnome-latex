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
 */

using Gtk;

public class MainWindow : ApplicationWindow
{
    // for the menu and the toolbar
    // name, icon-name, label, accelerator, tooltip, callback
    private const Gtk.ActionEntry[] _action_entries =
    {
        { "FileQuit", "application-exit", N_("_Quit"), "<Control>Q",
            N_("Quit the program") },

        // View
        { "View", null, N_("_View") },
        { "ViewZoomIn", "zoom-in", N_("Zoom _In"), "<Control>plus",
            N_("Enlarge the font"), on_view_zoom_in },
        { "ViewZoomOut", "zoom-out", N_("Zoom _Out"), "<Control>minus",
            N_("Shrink the font"), on_view_zoom_out },
        { "ViewZoomReset", "zoom-original", N_("_Reset Zoom"), "<Control>0",
            N_("Reset the size of the font"), on_view_zoom_reset },

        // Search
        { "Search", null, N_("_Search") },
        { "SearchFind", "edit-find", N_("_Find"), "<Control>F",
            N_("Search for text"), on_search_find },
        { "SearchReplace", "edit-find-replace", N_("Find and _Replace"), "<Control>H",
            N_("Search for and replace text"), on_search_replace },
        { "SearchForward", null, N_("_Jump to PDF"), "<Control><Alt>F",
            N_("Jump to the associated position in the PDF file. Another shortcut: Ctrl+click, which works in both directions."),
            on_search_forward },

        // LaTeX and Math
        { "Latex", null, "_LaTeX" },
        { "Math", null, N_("_Math") },

        // Projects
        { "Projects", null, N_("_Projects") },
        { "ProjectsNew", "document-new", N_("_New Project"), "",
            N_("Create a new project"), on_projects_new },
        { "ProjectsConfigCurrent", "document-properties", N_("_Configure Current Project"),
            null, N_("Change the main file of the current project"),
            on_projects_config_current },
        { "ProjectsManage", "preferences-system", N_("_Manage Projects"), null,
            N_("Manage Projects"), on_projects_manage },

        // Help
        { "Help", null, N_("_Help") },
        { "HelpContents", "help-browser", N_("_Contents"), "<Release>F1",
            N_("Open the GNOME LaTeX documentation") },
        { "HelpLatexReference", null, N_("_LaTeX Reference"), null,
            N_("The Kile LaTeX Reference"), on_help_latex_reference },
        { "HelpAbout", "help-about", N_("_About"), null,
            N_("About GNOME LaTeX") }
    };

    private const ToggleActionEntry[] _toggle_action_entries =
    {
        { "ViewMainToolbar", null, N_("_Main Toolbar"), null,
            N_("Show or hide the main toolbar"), null },
        // Translators: "Edit" here is an adjective.
        { "ViewEditToolbar", null, N_("_Edit Toolbar"), null,
            N_("Show or hide the edit toolbar"), null },
        { "ViewSidePanel", null, N_("_Side panel"), "<Release>F12",
            N_("Show or hide the side panel"), null },
        { "ViewBottomPanel", null, N_("_Bottom panel"), null,
            N_("Show or hide the bottom panel"), null }
    };

    public string default_location = Environment.get_home_dir ();
    private DocumentsPanel _documents_panel;
    private Tepl.Statusbar _statusbar;
    private SearchAndReplace _search_and_replace;
    private Tepl.Panel _side_panel;
    private Paned _main_hpaned;
    private Paned _vpaned;

    private UIManager _ui_manager;
    private Gtk.ActionGroup _action_group;

    private MainWindowFile _main_window_file;
    private MainWindowEdit _main_window_edit;
    private MainWindowBuildTools _main_window_build_tools;
    private MainWindowStructure _main_window_structure;
    private MainWindowDocuments _main_window_documents;
    private MainWindowTools _main_window_tools;

    // context id for the statusbar
    private uint _tip_message_cid;

    /*************************************************************************/
    // Properties

    public DocumentTab? active_tab
    {
        get
        {
            Tepl.ApplicationWindow tepl_window =
                Tepl.ApplicationWindow.get_from_gtk_application_window (this);
            return tepl_window.get_active_tab () as DocumentTab?;
        }

        set
        {
            Tepl.ApplicationWindow tepl_window =
                Tepl.ApplicationWindow.get_from_gtk_application_window (this);
            tepl_window.set_active_tab (value);
        }
    }

    public DocumentView? active_view
    {
        get
        {
            Tepl.ApplicationWindow tepl_window =
                Tepl.ApplicationWindow.get_from_gtk_application_window (this);
            return tepl_window.get_active_view () as DocumentView?;
        }
    }

    public Document? active_document
    {
        get
        {
            Tepl.ApplicationWindow tepl_window =
                Tepl.ApplicationWindow.get_from_gtk_application_window (this);
            return tepl_window.get_active_buffer () as Document?;
        }
    }

    /*************************************************************************/
    // Construction

    public MainWindow (GlatexApp app)
    {
        Object (application: app);

        // Init TeplApplicationWindow GActions.
        Tepl.ApplicationWindow tepl_window =
            Tepl.ApplicationWindow.get_from_gtk_application_window (this);

        Latexila.latex_commands_add_actions (this);

        /* GtkUIManager */

        initialize_ui_manager ();

        _main_window_file = new MainWindowFile (this, _ui_manager);
        _main_window_edit = new MainWindowEdit (this, _ui_manager);
        _main_window_build_tools = new MainWindowBuildTools (this, _ui_manager);
        _main_window_documents = new MainWindowDocuments (this, _ui_manager);
        _main_window_structure = new MainWindowStructure (_ui_manager);
        _main_window_tools = new MainWindowTools (this, _ui_manager);

        show_images_in_menu ();
        update_file_actions_sensitivity ();

        /* Main vertical grid */

        Grid main_vgrid = new Grid ();
        main_vgrid.orientation = Orientation.VERTICAL;
        main_vgrid.show ();
        add (main_vgrid);

        /* Menu */

        Widget menu = _ui_manager.get_widget ("/MainMenu");
        Gtk.MenuItem latex_menu_item =
            _ui_manager.get_widget ("/MainMenu/Latex") as Gtk.MenuItem;
        latex_menu_item.set_submenu (Latexila.latex_commands_create_latex_menu (this));
        Gtk.MenuItem math_menu_item =
            _ui_manager.get_widget ("/MainMenu/Math") as Gtk.MenuItem;
        math_menu_item.set_submenu (Latexila.latex_commands_create_math_menu (this));

        // Allow the menu to shrink below its minimum width
        Paned menu_paned = new Paned (Orientation.HORIZONTAL);
        Viewport viewport = new Viewport (null, null);
        viewport.shadow_type = ShadowType.NONE;
        Gdk.RGBA transparent = Gdk.RGBA ();
        transparent.alpha = 0.0;
        viewport.override_background_color (StateFlags.NORMAL, transparent);
        viewport.add (menu);
        menu_paned.add1 (viewport);
        menu_paned.show_all ();
        main_vgrid.add (menu_paned);

        /* Main and edit toolbars */

        Toolbar main_toolbar = get_main_toolbar ();
        Toolbar edit_toolbar = get_edit_toolbar ();
        main_vgrid.add (main_toolbar);
        main_vgrid.add (edit_toolbar);

        /* Main horizontal paned.
         * Left: side panel (symbols, file browser, ...)
         * Right: documents, bottom panel, ...
         */

        _main_hpaned = new Paned (Orientation.HORIZONTAL);
        _main_hpaned.show ();
        main_vgrid.add (_main_hpaned);

        /* Side panel */

        _side_panel = get_side_panel ();
        _main_hpaned.pack1 (_side_panel, false, false);

        /* Vertical paned.
         * Top: documents, search and replace, ...
         * Bottom: bottom panel
         */

        _vpaned = new Paned (Orientation.VERTICAL);
        _vpaned.show ();
        _main_hpaned.add2 (_vpaned);

        /* Vertical grid: documents, search and replace */

        Grid docs_vgrid = new Grid ();
        docs_vgrid.orientation = Orientation.VERTICAL;
        docs_vgrid.set_row_spacing (2);
        docs_vgrid.show ();

        // Documents panel
        init_documents_panel ();
        docs_vgrid.add (_documents_panel);

        // Search and Replace
        _search_and_replace = new SearchAndReplace (this);
        docs_vgrid.add (_search_and_replace.get_widget ());

        /* Bottom panel */

        BottomPanel bottom_panel = get_bottom_panel ();
        _main_window_build_tools.set_bottom_panel (bottom_panel);

        // When we resize the window, the bottom panel keeps the same height.
        _vpaned.pack1 (docs_vgrid, true, true);
        _vpaned.pack2 (bottom_panel, false, false);

        /* Statusbar */

        _statusbar = new Tepl.Statusbar ();
        _statusbar.set_tab_group (_documents_panel);
        _statusbar.show_all ();
        main_vgrid.add (_statusbar);

        Amtk.ApplicationWindow amtk_window =
            Amtk.ApplicationWindow.get_from_gtk_application_window (this);
        amtk_window.set_statusbar (_statusbar);

        _tip_message_cid = _statusbar.get_context_id ("tip_message");

        /* TeplApplicationWindow */

        tepl_window.notify["active-tab"].connect (() =>
        {
            update_file_actions_sensitivity ();
            update_config_project_sensitivity ();

            if (this.active_tab == null)
            {
                _search_and_replace.hide ();
            }

            this.notify_property ("active-tab");
        });

        tepl_window.notify["active-view"].connect (() =>
        {
            this.notify_property ("active-view");
        });

        tepl_window.notify["active-buffer"].connect (() =>
        {
            this.notify_property ("active-document");
        });

        tepl_window.set_tab_group (_documents_panel);
        tepl_window.set_handle_title (true);

        /* Other misc stuff */

        support_drag_and_drop ();

        delete_event.connect (() =>
        {
            quit ();

            // the destroy signal is not emitted
            return true;
        });

        restore_state ();
        show_or_hide_widgets ();
        show ();
    }

    // Force to show icons in the menu.
    // In the LaTeX and Math menu, icons are needed.
    private void show_images_in_menu ()
    {
        foreach (Gtk.ActionGroup action_group in _ui_manager.get_action_groups ())
        {
            foreach (Gtk.Action action in action_group.list_actions ())
            {
                action.set_always_show_image (true);
            }
        }
    }

    private void initialize_ui_manager ()
    {
        _action_group = new Gtk.ActionGroup ("ActionGroup");
        _action_group.set_translation_domain (Config.GETTEXT_PACKAGE);
        _action_group.add_actions (_action_entries, this);
        _action_group.add_toggle_actions (_toggle_action_entries, this);

        Amtk.utils_create_gtk_action (this, "win.tepl-goto-line",
            _action_group, "SearchGoToLine");

        _ui_manager = new UIManager ();
        _ui_manager.insert_action_group (_action_group, 0);

        try
        {
            string path = Path.build_filename (Config.DATA_DIR, "ui", "ui.xml");
            _ui_manager.add_ui_from_file (path);
        }
        catch (GLib.Error err)
        {
            error ("%s", err.message);
        }

        add_accel_group (_ui_manager.get_accel_group ());

        GlatexApp app = GlatexApp.get_instance ();
        Amtk.utils_bind_g_action_to_gtk_action (app, "quit",
            _action_group, "FileQuit");
        Amtk.utils_bind_g_action_to_gtk_action (app, "help",
            _action_group, "HelpContents");
        Amtk.utils_bind_g_action_to_gtk_action (app, "about",
            _action_group, "HelpAbout");

        /* Show tooltips in the statusbar */

        _ui_manager.connect_proxy.connect ((action, p) =>
        {
            if (p is Gtk.MenuItem)
            {
                Gtk.MenuItem proxy = p as Gtk.MenuItem;
                proxy.select.connect (on_menu_item_select);
                proxy.deselect.connect (on_menu_item_deselect);
            }
        });

        _ui_manager.disconnect_proxy.connect ((action, p) =>
        {
            if (p is Gtk.MenuItem)
            {
                Gtk.MenuItem proxy = p as Gtk.MenuItem;
                proxy.select.disconnect (on_menu_item_select);
                proxy.deselect.disconnect (on_menu_item_deselect);
            }
        });
    }

    private void on_menu_item_select (Gtk.MenuItem proxy)
    {
        Gtk.Action action = proxy.get_related_action ();
        return_if_fail (action != null);
        if (action.tooltip != null)
            _statusbar.push (_tip_message_cid, action.tooltip);
    }

    private void on_menu_item_deselect (Gtk.MenuItem proxy)
    {
        _statusbar.pop (_tip_message_cid);
    }

    private Toolbar get_main_toolbar ()
    {
        Toolbar main_toolbar = _ui_manager.get_widget ("/MainToolbar") as Toolbar;
        ToolItem open_button = _main_window_file.get_toolbar_open_button ();
        main_toolbar.insert (open_button, 1);

        main_toolbar.set_style (ToolbarStyle.ICONS);
        StyleContext context = main_toolbar.get_style_context ();
        context.add_class (Gtk.STYLE_CLASS_PRIMARY_TOOLBAR);

        main_toolbar.show_all ();

        ToggleAction action =
            _action_group.get_action ("ViewMainToolbar") as ToggleAction;
        action.active = true;

        main_toolbar.bind_property ("visible", action, "active",
            BindingFlags.BIDIRECTIONAL);

        return main_toolbar;
    }

    private Toolbar get_edit_toolbar ()
    {
        Toolbar edit_toolbar = Latexila.latex_commands_create_edit_toolbar (this);

        ToggleAction action =
            _action_group.get_action ("ViewEditToolbar") as ToggleAction;
        action.active = true;

        edit_toolbar.bind_property ("visible", action, "active",
            BindingFlags.BIDIRECTIONAL);

        return edit_toolbar;
    }

    private Tepl.Panel get_side_panel ()
    {
        Tepl.Panel side_panel = new Tepl.Panel.for_left_side_panel ();

        // Symbols
        SymbolsView symbols = new SymbolsView (this);
        side_panel.add_component (symbols, "symbols", _("Symbols"), "symbol_greek");

        // File browser
        FileBrowser file_browser = new FileBrowser (this);
        side_panel.add_component (file_browser, "file-browser", _("File Browser"),
            "document-open");

        // Structure
        Structure structure = new Structure (this);
        _main_window_structure.set_structure (structure);
        side_panel.add_component (structure, "structure", _("Structure"), Stock.INDEX); // FIXME don't use GtkStock

        // Restore state
        GLib.Settings settings = new GLib.Settings ("org.gnome.gnome-latex.preferences.ui");
        side_panel.provide_active_component_gsetting (settings, "side-panel-component");
        side_panel.restore_state_from_gsettings ();

        // Bind the toggle action to show/hide the side panel
        ToggleAction action = _action_group.get_action ("ViewSidePanel") as ToggleAction;
        action.active = true;
        side_panel.bind_property ("visible", action, "active",
            BindingFlags.BIDIRECTIONAL);

        return side_panel;
    }

    private void init_documents_panel ()
    {
        _documents_panel = new DocumentsPanel (this);
        _documents_panel.show_all ();

        _main_window_documents.set_documents_panel (_documents_panel);

        _documents_panel.right_click.connect ((event) =>
        {
            Gtk.Menu popup_menu = _ui_manager.get_widget ("/NotebookPopup") as Gtk.Menu;
            popup_menu.popup_at_pointer (event);
        });
    }

    private BottomPanel get_bottom_panel ()
    {
        Latexila.BuildView build_view = new Latexila.BuildView ();
        _main_window_build_tools.set_build_view (build_view);

        Toolbar build_toolbar = _ui_manager.get_widget ("/BuildToolbar") as Toolbar;
        build_toolbar.set_style (ToolbarStyle.ICONS);
        build_toolbar.set_icon_size (IconSize.MENU);
        build_toolbar.set_orientation (Orientation.VERTICAL);

        BottomPanel bottom_panel = new BottomPanel (build_view, build_toolbar);

        // Bind the toggle action to show/hide the bottom panel
        ToggleAction action =
            _action_group.get_action ("ViewBottomPanel") as ToggleAction;
        action.active = false;

        bottom_panel.bind_property ("visible", action, "active",
            BindingFlags.BIDIRECTIONAL);

        return bottom_panel;
    }

    private void restore_state ()
    {
        GLib.Settings settings = new GLib.Settings ("org.gnome.gnome-latex.state.window");

        /* The window itself */

        int width;
        int height;
        settings.get ("size", "(ii)", out width, out height);
        set_default_size (width, height);

        Gdk.WindowState state = (Gdk.WindowState) settings.get_int ("state");
        if (Gdk.WindowState.MAXIMIZED in state)
            maximize ();
        else
            unmaximize ();

        if (Gdk.WindowState.STICKY in state)
            stick ();
        else
            unstick ();

        /* Widgets */

        _main_hpaned.set_position (settings.get_int ("side-panel-size"));
        _vpaned.set_position (settings.get_int ("vertical-paned-position"));
    }

    // Drag and drop of a list of files.
    private void support_drag_and_drop ()
    {
        Gtk.drag_dest_set (this, DestDefaults.ALL, {}, Gdk.DragAction.COPY);
        Gtk.drag_dest_add_uri_targets (this);
        drag_data_received.connect ((dc, x, y, selection_data, info, time) =>
        {
            GlatexApp app = GlatexApp.get_instance ();

            File[] files = {};
            foreach (string uri in selection_data.get_uris ())
            {
                if (0 < uri.length)
                    files += File.new_for_uri (uri);
            }

            app.open_documents (files);
            Gtk.drag_finish (dc, true, true, time);
        });
    }

    private void show_or_hide_widgets ()
    {
        GLib.Settings settings = new GLib.Settings ("org.gnome.gnome-latex.preferences.ui");

        ToggleAction action =
            _action_group.get_action ("ViewMainToolbar") as ToggleAction;
        action.active = settings.get_boolean ("main-toolbar-visible");

        action = _action_group.get_action ("ViewEditToolbar") as ToggleAction;
        action.active = settings.get_boolean ("edit-toolbar-visible");

        action = _action_group.get_action ("ViewSidePanel") as ToggleAction;
        action.active = settings.get_boolean ("side-panel-visible");

        action = _action_group.get_action ("ViewBottomPanel") as ToggleAction;
        action.active = settings.get_boolean ("bottom-panel-visible");
    }

    /*************************************************************************/
    // Misc public functions

    public MainWindowStructure get_main_window_structure ()
    {
        return _main_window_structure;
    }

    /*************************************************************************/
    // Manage documents: get list of documents, open or save a document, etc.

    public Gee.List<Document> get_documents ()
    {
        Gee.List<Document> all_documents = new Gee.LinkedList<Document> ();
        int nb_documents = _documents_panel.get_n_pages ();
        for (int i = 0; i < nb_documents; i++)
        {
            DocumentTab tab = _documents_panel.get_nth_page (i) as DocumentTab;
            all_documents.add (tab.document);
        }

        return all_documents;
    }

    public Gee.List<Document> get_unsaved_documents ()
    {
        Gee.List<Document> unsaved_documents = new Gee.LinkedList<Document> ();
        foreach (Document doc in get_documents ())
        {
            if (doc.get_modified ())
                unsaved_documents.add (doc);
        }

        return unsaved_documents;
    }

    public Gee.List<DocumentView> get_views ()
    {
        Gee.List<DocumentView> all_views = new Gee.LinkedList<DocumentView> ();

        if (_documents_panel == null)
            return all_views;

        int nb_documents = _documents_panel.get_n_pages ();

        for (int i = 0; i < nb_documents; i++)
        {
            DocumentTab tab = _documents_panel.get_nth_page (i) as DocumentTab;
            all_views.add (tab.document_view);
        }

        return all_views;
    }

    public DocumentTab? open_document (File location, bool jump_to = true)
    {
        /* check if the document is already opened */
        foreach (Window window in GlatexApp.get_instance ().get_windows ())
        {
            if (!(window is MainWindow))
                continue;

            MainWindow w = window as MainWindow;

            foreach (Document doc in w.get_documents ())
            {
                if (doc.location == null || !location.equal (doc.location))
                    continue;

                /* the document is already opened in this window */
                if (this == w)
                {
                    if (jump_to)
                        active_tab = doc.tab;
                    return doc.tab;
                }

                /* the document is already opened in another window */
                DocumentTab tab = create_tab_from_location (location, jump_to);
                tab.document.readonly = true;

                Tepl.InfoBar infobar = Tepl.io_error_info_bar_file_already_open (location);
                tab.add_info_bar (infobar);
                infobar.show ();

                infobar.response.connect ((response_id) =>
                {
                    if (response_id == ResponseType.YES)
                        tab.document.readonly = false;
                    infobar.destroy ();
                    tab.view.grab_focus ();
                });

                return tab;
            }
        }

        return create_tab_from_location (location, jump_to);
    }

    public DocumentTab? create_tab (bool jump_to)
    {
        DocumentTab tab = new DocumentTab ();
        return process_create_tab (tab, jump_to);
    }

    private DocumentTab? create_tab_from_location (File location, bool jump_to)
    {
        DocumentTab tab = new DocumentTab.from_location (location);
        return process_create_tab (tab, jump_to);
    }

    public void create_tab_with_view (DocumentView view)
    {
        DocumentTab tab = new DocumentTab.with_view (view);
        process_create_tab (tab, true);
    }

    private DocumentTab? process_create_tab (DocumentTab? tab, bool jump_to)
    {
        if (tab == null)
            return null;

        tab.close_request.connect (() => {
            close_tab (tab);
            Signal.stop_emission_by_name (tab, "close-request");
        });

        Document doc = tab.document;

        doc.notify["location"].connect (() =>
        {
            sync_name (tab);
            _main_window_build_tools.update_sensitivity ();
        });

        doc.notify["tepl-short-title"].connect (() =>
        {
            sync_name (tab);
        });

        doc.notify["project-id"].connect (() =>
        {
            _main_window_build_tools.update_sensitivity ();
        });

        doc.modified_changed.connect (() => sync_name (tab));
        doc.notify["readonly"].connect (() => sync_name (tab));

        tab.show ();

        // add the tab at the end of the notebook
        _documents_panel.add_tab (tab, -1, jump_to);

        _main_window_edit.update_sensitivity ();
        _main_window_tools.update_sensitivity ();

        if (!this.get_visible ())
            this.present ();

        return tab;
    }

    // return true if the tab was closed
    public bool close_tab (DocumentTab tab, bool force_close = false)
    {
        /* If document not saved
         * Ask the user if he wants to save the file, or close without saving, or cancel
         */
        if (!force_close && tab.get_buffer ().get_modified ())
        {
            Dialog dialog = new MessageDialog (this,
                DialogFlags.DESTROY_WITH_PARENT,
                MessageType.QUESTION,
                ButtonsType.NONE,
                _("Save changes to document “%s” before closing?"),
                tab.get_buffer ().get_file ().get_short_name ());

            dialog.add_buttons (_("Close _without Saving"), ResponseType.CLOSE,
                _("_Cancel"), ResponseType.CANCEL);

            if (tab.document.location == null)
                dialog.add_button (_("Save _As"), ResponseType.ACCEPT);
            else
                dialog.add_button (_("_Save"), ResponseType.ACCEPT);

            while (true)
            {
                int res = dialog.run ();
                // Close without Saving
                if (res == ResponseType.CLOSE)
                    break;

                // Save or Save As
                else if (res == ResponseType.ACCEPT)
                {
                    if (save_document (tab.document, false))
                        break;
                    continue;
                }

                // Cancel
                else
                {
                    dialog.destroy ();
                    return false;
                }
            }

            dialog.destroy ();
        }

        _documents_panel.remove_tab (tab);
        return true;
    }

    private void sync_name (DocumentTab tab)
    {
        _main_window_documents.update_document_name (tab);
    }

    // return true if the document has been saved
    public bool save_document (Document doc, bool force_save_as)
    {
        if (!force_save_as && doc.location != null)
        {
            doc.save ();

            if (doc == active_document)
                _main_window_structure.refresh ();

            return true;
        }

        FileChooserDialog file_chooser = new FileChooserDialog (_("Save File"), this,
            FileChooserAction.SAVE,
            _("_Cancel"), ResponseType.CANCEL,
            _("_Save"), ResponseType.ACCEPT
        );

        file_chooser.set_do_overwrite_confirmation (true);
        file_chooser.set_local_only (false);

        string short_name = doc.get_file ().get_short_name ();
        if (doc.location == null)
            file_chooser.set_current_name (short_name + ".tex");
        else
            file_chooser.set_current_name (short_name);

        if (this.default_location != null)
            file_chooser.set_current_folder (this.default_location);

        if (doc.location != null)
        {
            try
            {
                // override the current name and current folder
                file_chooser.set_file (doc.location);
            }
            catch (Error e) {}
        }

        int response = file_chooser.run ();
        if (response == ResponseType.ACCEPT)
            doc.location = file_chooser.get_file ();

        this.default_location = file_chooser.get_current_folder ();
        file_chooser.destroy ();

        if (doc.location != null)
        {
            // force saving
            doc.save (false, true);

            if (doc == active_document)
                _main_window_structure.refresh ();

            return true;
        }
        return false;
    }

    // return true if all the documents are closed
    public bool close_all_documents ()
    {
        Gee.List<Document> unsaved_documents = get_unsaved_documents ();

        /* no unsaved document */
        if (unsaved_documents.size == 0)
        {
            _documents_panel.remove_all_tabs ();
            return true;
        }

        /* only one unsaved document */
        else if (unsaved_documents.size == 1)
        {
            Document doc = unsaved_documents.first ();
            active_tab = doc.tab;
            if (close_tab (doc.tab))
            {
                _documents_panel.remove_all_tabs ();
                return true;
            }
        }

        /* more than one unsaved document */
        else
        {
            Dialogs.close_several_unsaved_documents (this, unsaved_documents);
            if (_documents_panel.get_n_pages () == 0)
                return true;
        }

        return false;
    }

    public void remove_all_tabs ()
    {
        _documents_panel.remove_all_tabs ();
    }

    public void save_state ()
    {
        /* state of the window */
        GLib.Settings settings_window =
            new GLib.Settings ("org.gnome.gnome-latex.state.window");
        Gdk.WindowState state = get_window ().get_state ();
        settings_window.set_int ("state", state);

        // get width and height of the window
        int width;
        int height;
        get_size (out width, out height);
        settings_window.set ("size", "(ii)", width, height);

        settings_window.set_int ("side-panel-size", _main_hpaned.get_position ());
        settings_window.set_int ("vertical-paned-position", _vpaned.get_position ());

        _main_window_structure.save_state ();

        /* ui preferences */
        GLib.Settings settings_ui =
            new GLib.Settings ("org.gnome.gnome-latex.preferences.ui");

        // We don't bind this settings to the toggle action because when we change the
        // setting it must be applied only on the current window and not all windows.

        ToggleAction action = (ToggleAction) _action_group.get_action ("ViewMainToolbar");
        settings_ui.set_boolean ("main-toolbar-visible", action.active);

        action = (ToggleAction) _action_group.get_action ("ViewEditToolbar");
        settings_ui.set_boolean ("edit-toolbar-visible", action.active);

        action = (ToggleAction) _action_group.get_action ("ViewSidePanel");
        settings_ui.set_boolean ("side-panel-visible", action.active);

        action = (ToggleAction) _action_group.get_action ("ViewBottomPanel");
        settings_ui.set_boolean ("bottom-panel-visible", action.active);

        _main_window_build_tools.save_state ();

        _side_panel.save_state_to_gsettings ();
    }

    // start_line and end_line begins at 0.
    // The selection stops at the end of end_line (not at the start of end_line).
    public void jump_to_file_position (File file, int start_line, int end_line)
    {
        return_if_fail (start_line >= 0 && end_line >= 0);

        DocumentTab tab = open_document (file);

        // Ensure that the file is fully loaded before selecting the lines.
        Utils.flush_queue ();

        tab.view.select_lines (start_line, end_line);
    }

    /*************************************************************************/
    // Sensitivity

    private void update_file_actions_sensitivity ()
    {
        Tepl.ApplicationWindow tepl_window =
            Tepl.ApplicationWindow.get_from_gtk_application_window (this);
        bool sensitive = tepl_window.active_tab != null;

        // Actions that must be insensitive if the notebook is empty.
        string[] file_actions =
        {
            "ViewZoomIn",
            "ViewZoomOut",
            "ViewZoomReset",
            "SearchFind",
            "SearchReplace",
            "SearchForward",
            "ProjectsConfigCurrent",
            "Latex",
            "Math"
        };

        foreach (string file_action in file_actions)
        {
            Gtk.Action action = _action_group.get_action (file_action);
            action.set_sensitive (sensitive);
        }

        _main_window_file.update_sensitivity ();
        _main_window_edit.update_sensitivity ();
        _main_window_build_tools.update_sensitivity ();
        _main_window_tools.update_sensitivity ();
    }

    public void update_config_project_sensitivity ()
    {
        Gtk.Action action = _action_group.get_action ("ProjectsConfigCurrent");
        action.set_sensitive (active_tab != null && active_document.project_id != -1);
    }

    // Returns true if all the documents are closed.
    // Quits only this window, not the whole app.
    public bool quit ()
    {
        // save documents list
        string[] list_uris = {};
        foreach (Document doc in get_documents ())
        {
            if (doc.location != null)
                list_uris += doc.location.get_uri ();
        }

        GLib.Settings settings = new GLib.Settings ("org.gnome.gnome-latex.state.window");
        settings.set_strv ("documents", list_uris);

        if (close_all_documents ())
        {
            save_state ();
            destroy ();
            return true;
        }

        return false;
    }

    /*************************************************************************/
    // Gtk.Action callbacks

    /* View */

    public void on_view_zoom_in ()
    {
        return_if_fail (active_tab != null);
        active_view.enlarge_font ();
    }

    public void on_view_zoom_out ()
    {
        return_if_fail (active_tab != null);
        active_view.shrink_font ();
    }

    public void on_view_zoom_reset ()
    {
        return_if_fail (active_tab != null);
        active_view.set_font_from_settings ();
    }

    /* Search */

    public void on_search_find ()
    {
        return_if_fail (active_tab != null);
        _search_and_replace.show_search ();
    }

    public void on_search_replace ()
    {
        return_if_fail (active_tab != null);
        _search_and_replace.show_search_and_replace ();
    }

    public void on_search_forward ()
    {
        return_if_fail (active_tab != null);

        Latexila.Synctex synctex = Latexila.Synctex.get_instance ();
        synctex.forward_search (active_document, active_document.location,
            active_document.get_main_file (), Gdk.CURRENT_TIME);
    }

    /* Projects */

    public void on_projects_new ()
    {
        ProjectDialogs.new_project (this);
    }

    public void on_projects_config_current ()
    {
        return_if_fail (active_tab != null);
        return_if_fail (active_document.project_id != -1);
        ProjectDialogs.configure_project (this, active_document.project_id);
    }

    public void on_projects_manage ()
    {
        ProjectDialogs.manage_projects (this);
    }

    /* Help */

    public void on_help_latex_reference ()
    {
        try
        {
            string uri = Filename.to_uri (Path.build_filename (Config.DATA_DIR,
                "latexhelp.html", null));
            Latexila.utils_show_uri (this, uri, Gdk.CURRENT_TIME);
        }
        catch (Error e)
        {
            warning ("Impossible to open the LaTeX reference: %s", e.message);
        }
    }
}
