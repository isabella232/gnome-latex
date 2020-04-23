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

#include "latexila-settings.h"

/**
 * SECTION:settings
 * @title: LatexilaSettings
 * @short_description: Central access to #GSettings objects
 *
 * #LatexilaSettings is a singleton class to have a central access to #GSettings
 * objects. The central access permits to share the same #GSettings objects
 * between different parts of the application, which normally increases the
 * performances when synchronizing the values (but this hasn't been verified).
 */

struct _LatexilaSettingsPrivate
{
	GSettings *settings_editor;
};

/* LatexilaSettings is a singleton. */
static LatexilaSettings *singleton = NULL;

G_DEFINE_TYPE_WITH_PRIVATE (LatexilaSettings, latexila_settings, G_TYPE_OBJECT)

static void
latexila_settings_dispose (GObject *object)
{
	LatexilaSettings *self = LATEXILA_SETTINGS (object);

	g_clear_object (&self->priv->settings_editor);

	G_OBJECT_CLASS (latexila_settings_parent_class)->dispose (object);
}

static void
latexila_settings_finalize (GObject *object)
{
	if (singleton == LATEXILA_SETTINGS (object))
	{
		singleton = NULL;
	}

	G_OBJECT_CLASS (latexila_settings_parent_class)->finalize (object);
}

static void
latexila_settings_class_init (LatexilaSettingsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = latexila_settings_dispose;
	object_class->finalize = latexila_settings_finalize;
}

static void
latexila_settings_init (LatexilaSettings *self)
{
	self->priv = latexila_settings_get_instance_private (self);

	self->priv->settings_editor = g_settings_new ("org.gnome.gnome-latex.preferences.editor");
}

/**
 * latexila_settings_get_singleton:
 *
 * Returns: (transfer none): the #LatexilaSettings singleton instance.
 */
LatexilaSettings *
latexila_settings_get_singleton (void)
{
	if (singleton == NULL)
	{
		singleton = g_object_new (LATEXILA_TYPE_SETTINGS, NULL);
	}

	return singleton;
}

/**
 * latexila_settings_unref_singleton:
 *
 * Unrefs the #LatexilaSettings singleton instance.
 */
void
latexila_settings_unref_singleton (void)
{
	if (singleton != NULL)
	{
		g_object_unref (singleton);
	}

	/* singleton is not set to NULL here, it is set to NULL in
	 * latexila_settings_finalize() (i.e. when we are sure that the ref
	 * count reaches 0).
	 */
}

/**
 * latexila_settings_peek_editor_settings:
 * @self: the #LatexilaSettings instance.
 *
 * Returns: (transfer none): the #GSettings for
 * `"org.gnome.gnome-latex.preferences.editor"`.
 */
GSettings *
latexila_settings_peek_editor_settings (LatexilaSettings *self)
{
	g_return_val_if_fail (LATEXILA_IS_SETTINGS (self), NULL);
	return self->priv->settings_editor;
}
