SUBDIRS = evince liblatexila ui

bin_PROGRAMS = gnome-latex

# --disable-since-check is used because GNOME LaTeX often depends on unstable
# versions of GNOME libraries.
AM_VALAFLAGS =					\
	--disable-since-check			\
	--pkg gtk+-3.0				\
	--pkg gtksourceview-4			\
	--pkg Tepl-5				\
	--pkg gspell-1				\
	--pkg gee-0.8				\
	--pkg posix				\
	--target-glib=@GLIB_REQUIRED_VERSION@	\
	--vapidir liblatexila			\
	--vapidir $(abs_builddir)/liblatexila	\
	--pkg latexila

# The valac command is always executed in the sources directory, not the build
# directory. If $(top_srcdir) is used, it will be relative to the build directory.
vapidir = ../vapi

vala_files =				\
	app_settings.vala		\
	bottom_panel.vala		\
	build_tool_dialog.vala		\
	build_tools_preferences.vala	\
	clean_build_files.vala		\
	completion.vala			\
	dialogs.vala			\
	documents_panel.vala		\
	document_structure.vala		\
	document_tab.vala		\
	document.vala			\
	document_view.vala		\
	encodings.vala			\
	factory.vala			\
	file_browser.vala		\
	glatex_app.vala			\
	latex_menu.vala			\
	main.vala			\
	main_window_build_tools.vala	\
	main_window_documents.vala	\
	main_window_edit.vala		\
	main_window_file.vala		\
	main_window_structure.vala	\
	main_window_tools.vala		\
	main_window.vala		\
	menu_in_toolbar.vala		\
	most_used_symbols.vala		\
	preferences_dialog.vala		\
	project_dialogs.vala		\
	projects.vala			\
	search.vala			\
	side_panel.vala			\
	stock_icons.vala		\
	structure_model.vala		\
	structure.vala			\
	symbols.vala			\
	symbols_view.vala		\
	tab_label.vala			\
	utils.vala

gnome_latex_SOURCES = 				\
	$(vala_files)				\
	$(vapidir)/config.vapi			\
	$(vapidir)/menu_tool_action.vapi

# -w: silence all warnings, because we already have the valac warnings. The
# generated C code currently triggers a lot of warnings because deprecated
# functions are used. So by silencing the GCC warnings, we can more easily
# scroll up the build output to see the valac warnings.
gnome_latex_CPPFLAGS =			\
	-I$(top_srcdir)			\
	-I$(srcdir)/liblatexila		\
	-I$(builddir)/liblatexila	\
	-include config.h		\
	-w

gnome_latex_CFLAGS =	\
	$(DEP_CFLAGS)

gnome_latex_LDADD = 			\
	$(DEP_LIBS)			\
	liblatexila/liblatexila.la

# GResource

glatex-resources.c: glatex.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/glatex.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $(srcdir)/glatex.gresource.xml

BUILT_SOURCES = glatex-resources.c
nodist_gnome_latex_SOURCES = $(BUILT_SOURCES)

EXTRA_DIST = glatex.gresource.xml

CLEANFILES =			\
	$(BUILT_SOURCES)	\
	$(vala_files:.vala=.c)	\
	gnome_latex_vala.stamp

-include $(top_srcdir)/git.mk
