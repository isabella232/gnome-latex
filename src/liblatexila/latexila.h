/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014-2020 - Sébastien Wilmet <swilmet@gnome.org>
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

/* Main header for the GNOME LaTeX "library". Code using the library (e.g. Vala
 * code) should #include only this header.
 */

#ifndef LATEXILA_H
#define LATEXILA_H

#include "latexila-types.h"
#include "latexila-enum-types.h"

#include "latexila-app.h"
#include "latexila-buffer.h"
#include "latexila-build-job.h"
#include "latexila-build-tool.h"
#include "latexila-build-tools.h"
#include "latexila-build-tools-default.h"
#include "latexila-build-tools-personal.h"
#include "latexila-build-view.h"
#include "latexila-factory.h"
#include "latexila-init.h"
#include "latexila-latex-commands.h"
#include "latexila-post-processor.h"
#include "latexila-post-processor-all-output.h"
#include "latexila-post-processor-latex.h"
#include "latexila-settings.h"
#include "latexila-synctex.h"
#include "latexila-templates-default.h"
#include "latexila-templates-personal.h"
#include "latexila-templates-dialogs.h"
#include "latexila-templates-manage-dialog.h"
#include "latexila-utils.h"
#include "latexila-view.h"

#endif /* LATEXILA_H */
