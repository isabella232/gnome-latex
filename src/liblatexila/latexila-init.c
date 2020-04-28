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
#include "latexila-init.h"
#include <locale.h>
#include <libintl.h>
#include <tepl/tepl.h>
#include "latexila-settings.h"

/**
 * SECTION:init
 * @title: Latexila Initialization and Finalization
 */

static void
init_i18n (void)
{
	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
}

/**
 * latexila_init:
 *
 * Initializes GNOME LaTeX (e.g. for the internationalization).
 *
 * This function can be called several times, but is meant to be called at the
 * beginning of main(), before any other Tepl function call.
 *
 * This function also calls tepl_init().
 */
void
latexila_init (void)
{
	static gboolean done = FALSE;

	if (!done)
	{
		init_i18n ();
		tepl_init ();

		done = TRUE;
	}
}

/**
 * latexila_finalize:
 *
 * Free the resources allocated by GNOME LaTeX. For example it unrefs the
 * singleton objects.
 *
 * This function also calls tepl_finalize().
 *
 * It is not mandatory to call this function, it's just to be friendlier to
 * memory debugging tools. This function is meant to be called at the end of
 * main(). It can be called several times.
 */
void
latexila_finalize (void)
{
	static gboolean done = FALSE;

	if (!done)
	{
		latexila_settings_unref_singleton ();
		tepl_finalize ();

		done = TRUE;
	}
}
