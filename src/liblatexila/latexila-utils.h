/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014-2015, 2018 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef LATEXILA_UTILS_H
#define LATEXILA_UTILS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* File utilities */

void		latexila_utils_show_uri				(GtkWidget    *widget,
								 const gchar  *uri,
								 guint32       timestamp,
								 GError      **error);

/* UI utilities */

GtkWidget *	latexila_utils_get_dialog_component		(const gchar *title,
								 GtkWidget   *widget);

GtkWidget *	latexila_utils_join_widgets			(GtkWidget *widget_top,
								 GtkWidget *widget_bottom);

GdkPixbuf *	latexila_utils_get_pixbuf_from_icon_name	(const gchar *icon_name,
								 GtkIconSize  icon_size);

/* Others */

void		latexila_utils_register_icons			(void);

void		latexila_utils_migrate_latexila_to_gnome_latex	(void);

G_END_DECLS

#endif /* LATEXILA_UTILS_H */
