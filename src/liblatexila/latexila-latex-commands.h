/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2017-2018 - Sébastien Wilmet <swilmet@gnome.org>
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

#ifndef LATEXILA_LATEX_COMMANDS_H
#define LATEXILA_LATEX_COMMANDS_H

#include <tepl/tepl.h>

G_BEGIN_DECLS

void		latexila_latex_commands_add_action_infos	(GtkApplication *gtk_app);

void		latexila_latex_commands_add_actions		(GtkApplicationWindow *gtk_window);

GtkToolbar *	latexila_latex_commands_get_edit_toolbar	(void);

G_END_DECLS

#endif /* LATEXILA_LATEX_COMMANDS_H */
