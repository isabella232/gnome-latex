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

/**
 * SECTION:buffer
 * @title: LatexilaBuffer
 * @short_description: Additional #TeplBuffer functions
 *
 * Additional #TeplBuffer functions.
 */

#include "latexila-buffer.h"
#include "latexila-settings.h"

/**
 * latexila_buffer_setup:
 * @buffer: a #TeplBuffer.
 *
 * Setups a #TeplBuffer for GNOME LaTeX.
 */
void
latexila_buffer_setup (TeplBuffer *buffer)
{
	LatexilaSettings *settings;
	GSettings *editor_settings;

	g_return_if_fail (GTK_SOURCE_IS_BUFFER (buffer));

	settings = latexila_settings_get_singleton ();
	editor_settings = latexila_settings_peek_editor_settings (settings);

	g_settings_bind (editor_settings, "scheme",
			 buffer, "tepl-style-scheme-id",
			 G_SETTINGS_BIND_GET);

	g_settings_bind (editor_settings, "bracket-matching",
			 buffer, "highlight-matching-brackets",
			 G_SETTINGS_BIND_GET);
}
