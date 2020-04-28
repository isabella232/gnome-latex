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

#ifndef LATEXILA_APP_H
#define LATEXILA_APP_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define LATEXILA_TYPE_APP             (latexila_app_get_type ())
#define LATEXILA_APP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), LATEXILA_TYPE_APP, LatexilaApp))
#define LATEXILA_APP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), LATEXILA_TYPE_APP, LatexilaAppClass))
#define LATEXILA_IS_APP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LATEXILA_TYPE_APP))
#define LATEXILA_IS_APP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), LATEXILA_TYPE_APP))
#define LATEXILA_APP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), LATEXILA_TYPE_APP, LatexilaAppClass))

typedef struct _LatexilaApp         LatexilaApp;
typedef struct _LatexilaAppClass    LatexilaAppClass;

struct _LatexilaApp
{
	GtkApplication parent;
};

struct _LatexilaAppClass
{
	GtkApplicationClass parent_class;
};

GType		latexila_app_get_type	(void);

LatexilaApp *	latexila_app_new	(void);

G_END_DECLS

#endif /* LATEXILA_APP_H */
