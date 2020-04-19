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

#include "latexila-factory.h"

/**
 * SECTION:factory
 * @title: LatexilaFactory
 * @short_description: #TeplAbstractFactory subclass
 *
 * #LatexilaFactory is a #TeplAbstractFactory subclass to implement some virtual
 * functions for the GNOME LaTeX application.
 */

G_DEFINE_TYPE (LatexilaFactory, latexila_factory, TEPL_TYPE_ABSTRACT_FACTORY)

static GFile *
latexila_factory_create_metadata_manager_file (TeplAbstractFactory *factory)
{
	return g_file_new_build_filename (g_get_user_data_dir (),
					  "gnome-latex",
					  "gnome-latex-metadata.xml",
					  NULL);
}

static void
latexila_factory_class_init (LatexilaFactoryClass *klass)
{
	TeplAbstractFactoryClass *factory_class = TEPL_ABSTRACT_FACTORY_CLASS (klass);

	factory_class->create_metadata_manager_file = latexila_factory_create_metadata_manager_file;
}

static void
latexila_factory_init (LatexilaFactory *factory)
{
}

/**
 * latexila_factory_new:
 *
 * Returns: a new #LatexilaFactory object.
 */
LatexilaFactory *
latexila_factory_new (void)
{
	return g_object_new (LATEXILA_TYPE_FACTORY, NULL);
}
