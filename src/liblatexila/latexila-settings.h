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

#ifndef LATEXILA_SETTINGS_H
#define LATEXILA_SETTINGS_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define LATEXILA_TYPE_SETTINGS             (latexila_settings_get_type ())
#define LATEXILA_SETTINGS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), LATEXILA_TYPE_SETTINGS, LatexilaSettings))
#define LATEXILA_SETTINGS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), LATEXILA_TYPE_SETTINGS, LatexilaSettingsClass))
#define LATEXILA_IS_SETTINGS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LATEXILA_TYPE_SETTINGS))
#define LATEXILA_IS_SETTINGS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), LATEXILA_TYPE_SETTINGS))
#define LATEXILA_SETTINGS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), LATEXILA_TYPE_SETTINGS, LatexilaSettingsClass))

typedef struct _LatexilaSettings         LatexilaSettings;
typedef struct _LatexilaSettingsClass    LatexilaSettingsClass;
typedef struct _LatexilaSettingsPrivate  LatexilaSettingsPrivate;

struct _LatexilaSettings
{
	GObject parent;

	LatexilaSettingsPrivate *priv;
};

struct _LatexilaSettingsClass
{
	GObjectClass parent_class;
};

GType		latexila_settings_get_type			(void);

LatexilaSettings *
		latexila_settings_get_singleton			(void);

void		_latexila_settings_unref_singleton		(void);

GSettings *	latexila_settings_peek_editor_settings		(LatexilaSettings *self);

G_END_DECLS

#endif /* LATEXILA_SETTINGS_H */
