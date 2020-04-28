/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2017-2020 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef LATEXILA_VIEW_H
#define LATEXILA_VIEW_H

#include <gtksourceview/gtksource.h>

G_BEGIN_DECLS

void	latexila_view_setup			(GtkSourceView *view);

gchar *	latexila_view_get_indentation_style	(GtkSourceView *view);

G_END_DECLS

#endif /* LATEXILA_VIEW_H */
