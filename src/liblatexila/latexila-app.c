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

#include "latexila-app.h"

/**
 * SECTION:app
 * @title: LatexilaApp
 * @short_description: Subclass of #GtkApplication
 */

G_DEFINE_TYPE (LatexilaApp, latexila_app, GTK_TYPE_APPLICATION)

static void
latexila_app_class_init (LatexilaAppClass *klass)
{
}

static void
latexila_app_init (LatexilaApp *app)
{
}

/**
 * latexila_app_new:
 *
 * Returns: (transfer full): a new #LatexilaApp object.
 */
LatexilaApp *
latexila_app_new (void)
{
	return g_object_new (LATEXILA_TYPE_APP, NULL);
}
