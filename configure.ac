#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Init Autoconf
AC_PREREQ([2.64])

AC_INIT([GNOME LaTeX],
	[3.37.0],
	[https://gitlab.gnome.org/GNOME/gnome-latex/issues],
	[gnome-latex],
	[https://wiki.gnome.org/Apps/GNOME-LaTeX])

AX_REQUIRE_DEFINED([AX_IS_RELEASE])
AX_IS_RELEASE([git-directory])

AC_CONFIG_SRCDIR([src/liblatexila/latexila.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Init Automake
# gnu strictness to generate the INSTALL file.
AM_INIT_AUTOMAKE([1.14 gnu tar-ustar no-dist-gzip dist-xz -Wno-portability])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

# Init Libtool
LT_PREREQ([2.2])
LT_INIT

# Required dependencies versions
GLIB_REQUIRED_VERSION="2.56"
GTK_REQUIRED_VERSION="3.22"
GTKSOURCEVIEW_REQUIRED_VERSION="4.0"
TEPL_REQUIRED_VERSION="4.99"
GSPELL_REQUIRED_VERSION="1.8"
VALA_REQUIRED_VERSION="0.46"
GEE_REQUIRED_VERSION="0.10"

AC_SUBST([GLIB_REQUIRED_VERSION])

# Some directories

# adl_RECURSIVE_EVAL(VALUE, RESULT)
# =================================
# Interpolate the VALUE in loop until it doesn't change,
# and set the result to $RESULT.
# WARNING: It's easy to get an infinite loop with some unsane input.
# For example ${datadir} becomes ${datarootdir}, and then ${prefix}/share, and
# finally ${prefix} is replaced by the prefix.
#
# Why is it used in GNOME LaTeX?
# TL;DR: Vala.
#
# If the variable is needed only in C code, it is possible to pass the value
# with -D in the CPPFLAGS in the Makefile.am, and the value will correctly be
# interpolated.
# This macro is used in GNOME LaTeX because the variables are needed in Vala,
# the variables are accessed through vapi/config.vapi, so they need to be
# present in the config.h (the variables are added to the config.h with
# AC_DEFINE_UNQUOTED).
AC_DEFUN([adl_RECURSIVE_EVAL],
[_lcl_receval="$1"
$2=`(test "x$prefix" = xNONE && prefix="$ac_default_prefix"
     test "x$exec_prefix" = xNONE && exec_prefix="${prefix}"
     _lcl_receval_old=''
     while test "[$]_lcl_receval_old" != "[$]_lcl_receval"; do
       _lcl_receval_old="[$]_lcl_receval"
       eval _lcl_receval="\"[$]_lcl_receval\""
     done
     echo "[$]_lcl_receval")`])

adl_RECURSIVE_EVAL(["${datadir}/gnome-latex"], [DATA_DIR])
AC_DEFINE_UNQUOTED([DATA_DIR],
                   ["${DATA_DIR}"],
		   [gnome-latex data directory])

adl_RECURSIVE_EVAL(["${datadir}/icons/hicolor"], [ICONS_DIR])
AC_DEFINE_UNQUOTED([ICONS_DIR],
                   ["${ICONS_DIR}"],
		   [icons directory])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_PROG([GLIB_COMPILE_RESOURCES], [glib-compile-resources])
AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_PATH_PROG([GDBUS_CODEGEN], [gdbus-codegen])
AM_PROG_VALAC([${VALA_REQUIRED_VERSION}],
	      [found_vala=true]
	      [found_vala=false])

AX_REQUIRE_DEFINED([PKG_PROG_PKG_CONFIG])
PKG_PROG_PKG_CONFIG

# Checks for dependencies
AX_REQUIRE_DEFINED([PKG_CHECK_MODULES])
PKG_CHECK_MODULES([DEP], [
	glib-2.0 >= ${GLIB_REQUIRED_VERSION}
	gio-2.0 >= ${GLIB_REQUIRED_VERSION}
	gtk+-3.0 >= ${GTK_REQUIRED_VERSION}
	gtksourceview-4 >= ${GTKSOURCEVIEW_REQUIRED_VERSION}
	tepl-5 >= ${TEPL_REQUIRED_VERSION}
	gspell-1 >= ${GSPELL_REQUIRED_VERSION}
	gee-0.8 >= ${GEE_REQUIRED_VERSION}
	gsettings-desktop-schemas
])

# dconf migration option
AC_ARG_ENABLE([dconf_migration],
	      [AS_HELP_STRING([--disable-dconf-migration],
			      [do not add support for latexila to gnome-latex dconf migration])],
	      [enable_dconf_migration=$enableval],
	      [enable_dconf_migration=yes])

if test x$enable_dconf_migration = xyes; then
	PKG_CHECK_MODULES([DCONF_DEP], [dconf])
	AC_DEFINE_UNQUOTED([HAVE_DCONF_MIGRATION], [1], [Whether dconf migration is supported])
else
	AC_DEFINE_UNQUOTED([HAVE_DCONF_MIGRATION], [0], [Whether dconf migration is supported])
fi

AM_CONDITIONAL([DCONF_MIGRATION_BUILD], [test x$enable_dconf_migration = xyes])

# Native Language Support
AX_REQUIRE_DEFINED([IT_PROG_INTLTOOL])
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=${PACKAGE_TARNAME}
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
		   ["${GETTEXT_PACKAGE}"],
		   [The domain to use with gettext])

adl_RECURSIVE_EVAL(["${localedir}"], [LOCALE_DIR])
AC_DEFINE_UNQUOTED([LOCALE_DIR],
		   ["${LOCALE_DIR}"],
		   [locale directory])

# Compile warnings. There can be deprecation warnings at any time, so disable
# -Werror by default with the [yes] argument.
AX_REQUIRE_DEFINED([AX_COMPILER_FLAGS])
AX_COMPILER_FLAGS([WARN_CFLAGS], [WARN_LDFLAGS], [yes])

# Misc
AX_REQUIRE_DEFINED([YELP_HELP_INIT])
YELP_HELP_INIT

AX_REQUIRE_DEFINED([GLIB_GSETTINGS])
GLIB_GSETTINGS

AX_REQUIRE_DEFINED([GOBJECT_INTROSPECTION_CHECK])
GOBJECT_INTROSPECTION_CHECK([1.30.0])

AX_REQUIRE_DEFINED([VAPIGEN_CHECK])
VAPIGEN_CHECK

AX_REQUIRE_DEFINED([GTK_DOC_CHECK])
GTK_DOC_CHECK([1.14], [--flavour no-tmpl])

AX_REQUIRE_DEFINED([AX_CODE_COVERAGE])
AX_CODE_COVERAGE

AX_REQUIRE_DEFINED([APPSTREAM_XML])
APPSTREAM_XML

# Generate files
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	data/org.gnome.gnome-latex.gschema.xml
	data/build_tools/Makefile
	data/icons/Makefile
	data/icons/16x16/Makefile
	data/icons/16x16/apps/Makefile
	data/icons/22x22/Makefile
	data/icons/22x22/apps/Makefile
	data/icons/24x24/Makefile
	data/icons/24x24/apps/Makefile
	data/icons/32x32/Makefile
	data/icons/32x32/apps/Makefile
	data/icons/48x48/Makefile
	data/icons/48x48/apps/Makefile
	data/icons/256x256/Makefile
	data/icons/256x256/apps/Makefile
	data/icons/symbolic/Makefile
	data/icons/symbolic/apps/Makefile
	data/images/Makefile
	data/templates/Makefile
	docs/Makefile
	docs/reference/Makefile
	help/Makefile
	man/Makefile
	po/Makefile.in
	src/Makefile
	src/evince/Makefile
	src/liblatexila/Makefile
	src/ui/Makefile
	tests/Makefile
])

AC_OUTPUT

if ${found_vala}
then
	vala_compiler="valac >= ${VALA_REQUIRED_VERSION} not found."
else
	vala_compiler=${VALAC}
fi

# Print a summary of the configuration
echo "
Configuration:

	${PACKAGE_NAME} version ${PACKAGE_VERSION}

	Source code location:	${srcdir}
	Prefix:			${prefix}
	C compiler:		${CC}
	Vala compiler:		${vala_compiler}
	Dev documentation:	${enable_gtk_doc}
	GObject introspection:	${found_introspection}
	Generate latexila.vapi:	${enable_vala}
	Code coverage:		${enable_code_coverage}
	dconf migration:	${enable_dconf_migration}
"
