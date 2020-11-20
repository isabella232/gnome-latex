include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean

noinst_LTLIBRARIES = liblatexila.la

liblatexila_la_CPPFLAGS =		\
	$(CODE_COVERAGE_CPPFLAGS)

liblatexila_la_CFLAGS =			\
	$(DEP_CFLAGS)			\
	$(DCONF_DEP_CFLAGS)		\
	$(WARN_CFLAGS)			\
	$(CODE_COVERAGE_CFLAGS)		\
	-I$(top_builddir)/src/evince

liblatexila_la_LDFLAGS = \
	$(WARN_LDFLAGS)

liblatexila_la_LIBADD =		\
	$(DEP_LIBS)		\
	$(DCONF_DEP_LIBS)	\
	$(CODE_COVERAGE_LIBS)	\
	../evince/libevince.la

liblatexila_public_headers =			\
	latexila.h				\
	latexila-app.h				\
	latexila-buffer.h			\
	latexila-build-job.h			\
	latexila-build-tool.h			\
	latexila-build-tools.h			\
	latexila-build-tools-default.h		\
	latexila-build-tools-personal.h		\
	latexila-build-view.h			\
	latexila-factory.h			\
	latexila-init.h				\
	latexila-latex-commands.h		\
	latexila-post-processor.h		\
	latexila-post-processor-all-output.h	\
	latexila-post-processor-latex.h		\
	latexila-post-processor-latexmk.h	\
	latexila-settings.h			\
	latexila-synctex.h			\
	latexila-templates-default.h		\
	latexila-templates-dialogs.h		\
	latexila-templates-manage-dialog.h	\
	latexila-templates-personal.h		\
	latexila-types.h			\
	latexila-utils.h			\
	latexila-view.h				\
	$(NULL)

liblatexila_public_c_files =			\
	latexila-app.c				\
	latexila-buffer.c			\
	latexila-build-job.c			\
	latexila-build-tool.c			\
	latexila-build-tools.c			\
	latexila-build-tools-default.c		\
	latexila-build-tools-personal.c		\
	latexila-build-view.c			\
	latexila-factory.c			\
	latexila-init.c				\
	latexila-latex-commands.c		\
	latexila-post-processor.c		\
	latexila-post-processor-all-output.c	\
	latexila-post-processor-latex.c		\
	latexila-post-processor-latexmk.c	\
	latexila-settings.c			\
	latexila-synctex.c			\
	latexila-templates-default.c		\
	latexila-templates-dialogs.c		\
	latexila-templates-manage-dialog.c	\
	latexila-templates-personal.c		\
	latexila-utils.c			\
	latexila-view.c				\
	$(NULL)

liblatexila_private_headers =			\
	latexila-templates-common.h		\
	$(NULL)

liblatexila_private_c_files =			\
	latexila-templates-common.c		\
	$(NULL)

if DCONF_MIGRATION_BUILD
liblatexila_private_headers += dh-dconf-migration.h
liblatexila_private_c_files += dh-dconf-migration.c
endif

liblatexila_public_built_sources =	\
	latexila-enum-types.c		\
	latexila-enum-types.h		\
	$(NULL)

liblatexila_la_SOURCES =		\
	$(liblatexila_public_headers)	\
	$(liblatexila_public_c_files)	\
	$(liblatexila_private_headers)	\
	$(liblatexila_private_c_files)	\
	$(NULL)

nodist_liblatexila_la_SOURCES = $(liblatexila_public_built_sources)

BUILT_SOURCES = $(liblatexila_public_built_sources)

ENUM_TYPES = $(liblatexila_public_headers)

latexila-enum-types.h: latexila-enum-types.h.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template latexila-enum-types.h.template $(ENUM_TYPES)) > $@

latexila-enum-types.c: latexila-enum-types.c.template $(ENUM_TYPES) $(GLIB_MKENUMS)
	$(AM_V_GEN) (cd $(srcdir) && $(GLIB_MKENUMS) --template latexila-enum-types.c.template $(ENUM_TYPES)) > $@

EXTRA_DIST =				\
	latexila-enum-types.c.template	\
	latexila-enum-types.h.template

CLEANFILES = $(BUILT_SOURCES)

noinst_DATA =

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_SCANNER_ARGS =	\
	-I$(top_srcdir)		\
	-I$(top_builddir)	\
	--c-include=latexila.h	\
	--warn-all

INTROSPECTION_GIRS = Latexila.gir

Latexila.gir: liblatexila.la
Latexila_gir_NAMESPACE = Latexila
Latexila_gir_INCLUDES = Gtk-3.0 GtkSource-4 Tepl-6
Latexila_gir_LIBS = liblatexila.la
Latexila_gir_FILES =				\
	$(liblatexila_public_headers)		\
	$(liblatexila_public_c_files)		\
	$(liblatexila_public_built_sources)	\
	$(NULL)
Latexila_gir_SCANNERFLAGS = $(WARN_SCANNERFLAGS)

noinst_DATA += Latexila.gir Latexila.typelib
CLEANFILES += Latexila.gir Latexila.typelib

endif # HAVE_INTROSPECTION

if ENABLE_VAPIGEN
include $(VAPIGEN_MAKEFILE)

latexila.vapi: Latexila.gir

VAPIGEN_VAPIS = latexila.vapi

latexila_vapi_DEPS = gio-2.0 gtk+-3.0 gtksourceview-4 Tepl-6
latexila_vapi_FILES = Latexila.gir

noinst_DATA += latexila.vapi
CLEANFILES += latexila.vapi
EXTRA_DIST += latexila.vapi

endif # ENABLE_VAPIGEN

-include $(top_srcdir)/git.mk
