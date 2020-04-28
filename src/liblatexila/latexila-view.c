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

/**
 * SECTION:view
 * @title: LatexilaView
 * @short_description: Additional #TeplView functions
 *
 * Additional #TeplView functions.
 */

#include "latexila-view.h"
#include "latexila-settings.h"

static void
configure_space_drawer (GtkSourceView *view)
{
	GtkSourceSpaceDrawer *space_drawer;

	space_drawer = gtk_source_view_get_space_drawer (view);

	/* Rationale for always drawing non-breaking spaces:
	 *
	 * With my Dvorak bépo keyboard layout, it is possible to type a non-breaking
	 * space. I remember that one time I inserted one by mistake in GNOME LaTeX,
	 * and when compiling the document there was an incomprehensible error, it
	 * took me some time to figure out that there was a non-breaking space... So,
	 * I think it's better to always draw non-breaking spaces, to distinguish them
	 * from normal spaces. -- swilmet
	 */
	gtk_source_space_drawer_set_types_for_locations (space_drawer,
							 GTK_SOURCE_SPACE_LOCATION_ALL,
							 GTK_SOURCE_SPACE_TYPE_NBSP);

	gtk_source_space_drawer_set_enable_matrix (space_drawer, TRUE);
}

/**
 * latexila_view_setup:
 * @view: a #GtkSourceView widget.
 *
 * Setups a #GtkSourceView widget for GNOME LaTeX.
 */
void
latexila_view_setup (GtkSourceView *view)
{
	GtkTextView *text_view;
	LatexilaSettings *settings;
	GSettings *editor_settings;

	g_return_if_fail (GTK_SOURCE_IS_VIEW (view));

	text_view = GTK_TEXT_VIEW (view);

	configure_space_drawer (view);

	gtk_text_view_set_wrap_mode (text_view, GTK_WRAP_WORD);
	gtk_source_view_set_auto_indent (view, TRUE);
	gtk_source_view_set_smart_home_end (view, GTK_SOURCE_SMART_HOME_END_AFTER);

	settings = latexila_settings_get_singleton ();
	editor_settings = latexila_settings_peek_editor_settings (settings);

	g_settings_bind (editor_settings, "forget-no-tabs",
			 view, "smart-backspace",
			 G_SETTINGS_BIND_GET | G_SETTINGS_BIND_NO_SENSITIVITY);

	g_settings_bind (editor_settings, "tabs-size",
			 view, "tab-width",
			 G_SETTINGS_BIND_GET | G_SETTINGS_BIND_NO_SENSITIVITY);

	g_settings_bind (editor_settings, "insert-spaces",
			 view, "insert-spaces-instead-of-tabs",
			 G_SETTINGS_BIND_GET | G_SETTINGS_BIND_NO_SENSITIVITY);

	g_settings_bind (editor_settings, "display-line-numbers",
			 view, "show-line-numbers",
			 G_SETTINGS_BIND_GET | G_SETTINGS_BIND_NO_SENSITIVITY);

	g_settings_bind (editor_settings, "highlight-current-line",
			 view, "highlight-current-line",
			 G_SETTINGS_BIND_GET | G_SETTINGS_BIND_NO_SENSITIVITY);
}

/**
 * latexila_view_get_indentation_style:
 * @view: a #GtkSourceView.
 *
 * Returns one indentation level, as a string.
 *
 * Attention, this function assumes that the #GtkSourceView:indent-width is -1
 * or equal to the #GtkSourceView:tab-width, which is always the case in
 * GNOME LaTeX (there is no #GSettings for the #GtkSourceView:indent-width).
 *
 * Returns: one indentation level, as a string. Free with g_free() when no
 * longer needed.
 */
gchar *
latexila_view_get_indentation_style (GtkSourceView *view)
{
	guint tab_width;
	gint indent_width;

	g_return_val_if_fail (GTK_SOURCE_IS_VIEW (view), NULL);

	tab_width = gtk_source_view_get_tab_width (view);
	indent_width = gtk_source_view_get_indent_width (view);
	g_return_val_if_fail (indent_width == -1 || indent_width == (gint)tab_width, NULL);

	if (gtk_source_view_get_insert_spaces_instead_of_tabs (view))
	{
		return g_strnfill (tab_width, ' ');
	}

	return g_strdup ("\t");
}
