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

#ifndef LATEXILA_FACTORY_H
#define LATEXILA_FACTORY_H

#include <tepl/tepl.h>

G_BEGIN_DECLS

#define LATEXILA_TYPE_FACTORY             (latexila_factory_get_type ())
#define LATEXILA_FACTORY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), LATEXILA_TYPE_FACTORY, LatexilaFactory))
#define LATEXILA_FACTORY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), LATEXILA_TYPE_FACTORY, LatexilaFactoryClass))
#define LATEXILA_IS_FACTORY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LATEXILA_TYPE_FACTORY))
#define LATEXILA_IS_FACTORY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), LATEXILA_TYPE_FACTORY))
#define LATEXILA_FACTORY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), LATEXILA_TYPE_FACTORY, LatexilaFactoryClass))

typedef struct _LatexilaFactory         LatexilaFactory;
typedef struct _LatexilaFactoryClass    LatexilaFactoryClass;

struct _LatexilaFactory
{
	TeplAbstractFactory parent;
};

struct _LatexilaFactoryClass
{
	TeplAbstractFactoryClass parent_class;
};

GType			latexila_factory_get_type	(void);

LatexilaFactory *	latexila_factory_new		(void);

G_END_DECLS

#endif /* LATEXILA_FACTORY_H */
