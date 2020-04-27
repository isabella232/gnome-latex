/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2017-2018 - Sébastien Wilmet <swilmet@gnome.org>
 * Copyright (C) 2018 - Robert Griesel <r.griesel@gmail.com>
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
 * SECTION:latex-commands
 * @title: LaTeX commands
 * @short_description: LaTeX menu, Math menu and Edit toolbar
 */

#include "config.h"
#include "latexila-latex-commands.h"
#include <glib/gi18n.h>
#include "latexila-utils.h"
#include "latexila-view.h"

/**
 * latexila_latex_commands_add_action_infos:
 * @gtk_app: the #GtkApplication instance.
 *
 * Creates the #AmtkActionInfo's related to the LaTeX and Math menus, and add
 * them to the #AmtkActionInfoStore as returned by
 * tepl_application_get_app_action_info_store().
 */
void
latexila_latex_commands_add_action_infos (GtkApplication *gtk_app)
{
	TeplApplication *tepl_app;
	AmtkActionInfoStore *store;

	const AmtkActionInfoEntry entries[] =
	{
		/* action, icon, label, accel, tooltip */

		/* LaTeX: Sectioning */

		{ "win.latex-command-with-braces::part", NULL, "\\_part", NULL,
		  N_("Part") },
		{ "win.latex-command-with-braces::chapter", NULL, "\\_chapter", NULL,
		  N_("Chapter") },
		{ "win.latex-command-with-braces::section", NULL, "\\_section", NULL,
		  N_("Section") },
		{ "win.latex-command-with-braces::subsection", NULL, "\\s_ubsection", NULL,
		  N_("Sub-section") },
		{ "win.latex-command-with-braces::subsubsection", NULL, "\\su_bsubsection", NULL,
		  N_("Sub-sub-section") },
		{ "win.latex-command-with-braces::paragraph", NULL, "\\p_aragraph", NULL,
		  N_("Paragraph") },
		{ "win.latex-command-with-braces::subparagraph", NULL, "\\subpa_ragraph", NULL,
		  N_("Sub-paragraph") },

		/* LaTeX: References */

		{ "win.latex-command-with-braces::label", NULL, "\\_label", NULL,
		  N_("Label") },
		{ "win.latex-command-with-braces::ref", NULL, "\\_ref", NULL,
		  N_("Reference to a label") },
		{ "win.latex-command-with-braces::pageref", NULL, "\\_pageref", NULL,
		  N_("Page reference to a label") },
		{ "win.latex-command-with-braces::index", NULL, "\\_index", NULL,
		  N_("Add a word to the index") },
		{ "win.latex-command-with-braces::footnote", NULL, "\\_footnote", NULL,
		  N_("Footnote") },
		{ "win.latex-command-with-braces::cite", NULL, "\\_cite", NULL,
		  N_("Reference to a bibliography item") },

		/* LaTeX: Environments */

		{ "win.latex-command-env-simple::center", "format-justify-center", "\\begin{_center}", NULL,
		  N_("Center — \\begin{center}") },
		{ "win.latex-command-env-simple::flushleft", "format-justify-left", "\\begin{flush_left}", NULL,
		  N_("Align Left — \\begin{flushleft}") },
		{ "win.latex-command-env-simple::flushright", "format-justify-right", "\\begin{flush_right}", NULL,
		  N_("Align Right — \\begin{flushright}") },
		{ "win.latex-command-env-figure", "image-x-generic", "\\begin{_figure}", NULL,
		  N_ ("Figure — \\begin{figure}") },
		{ "win.latex-command-env-table", "table", "\\begin{_table}", NULL,
		  N_("Table — \\begin{table}") },
		{ "win.latex-command-env-simple::quote", NULL, "\\begin{_quote}", NULL,
		  N_("Quote — \\begin{quote}") },
		{ "win.latex-command-env-simple::quotation", NULL, "\\begin{qu_otation}", NULL,
		  N_("Quotation — \\begin{quotation}") },
		{ "win.latex-command-env-simple::verse", NULL, "\\begin{_verse}", NULL,
		  N_("Verse — \\begin{verse}") },
		{ "win.latex-command-env-simple::verbatim", NULL, "\\begin{ver_batim}", NULL,
		  N_("Verbatim — \\begin{verbatim}") },
		{ "win.latex-command-env-simple::minipage", NULL, "\\begin{_minipage}", NULL,
		  N_("Minipage — \\begin{minipage}") },
		{ "win.latex-command-env-simple::titlepage", NULL, "\\begin{titlepage}", NULL,
		  N_("Title page — \\begin{titlepage}") },

		/* LaTeX: list environments */

		{ "win.latex-command-list-env-simple::itemize", "list-itemize", "\\begin{_itemize}", NULL,
		  N_("Bulleted List — \\begin{itemize}") },
		{ "win.latex-command-list-env-simple::enumerate", "list-enumerate", "\\begin{_enumerate}", NULL,
		  N_("Enumeration — \\begin{enumerate}") },
		{ "win.latex-command-list-env-description", "list-description", "\\begin{_description}", NULL,
		  N_("Description — \\begin{description}") },
		{ "win.latex-command-list-env-list", NULL, "\\begin{_list}", NULL,
		  N_("Custom list — \\begin{list}") },
		{ "win.latex-command-with-space::item", "list-item", "\\i_tem", "<Alt><Shift>H",
		  N_("List item — \\item") },

		/* LaTeX: character sizes */

		{ "win.latex-command-char-style::tiny", NULL, "_tiny", NULL, "tiny" },
		{ "win.latex-command-char-style::scriptsize", NULL, "_scriptsize", NULL, "scriptsize" },
		{ "win.latex-command-char-style::footnotesize", NULL, "_footnotesize", NULL, "footnotesize" },
		{ "win.latex-command-char-style::small", NULL, "s_mall", NULL, "small" },
		{ "win.latex-command-char-style::normalsize", NULL, "_normalsize", NULL, "normalsize" },
		{ "win.latex-command-char-style::large", NULL, "_large", NULL, "large" },
		{ "win.latex-command-char-style::Large", NULL, "L_arge", NULL, "Large" },
		{ "win.latex-command-char-style::LARGE", NULL, "LA_RGE", NULL, "LARGE" },
		{ "win.latex-command-char-style::huge", NULL, "_huge", NULL, "huge" },
		{ "win.latex-command-char-style::Huge", NULL, "H_uge", NULL, "Huge" },

		/* LaTeX: font styles */

		{ "win.latex-command-with-braces::textbf", "bold", "\\text_bf", "<Control>B",
		  N_("Bold — \\textbf") },
		{ "win.latex-command-with-braces::textit", "italic", "\\text_it", "<Control>I",
		  N_("Italic — \\textit") },
		{ "win.latex-command-with-braces::texttt", "typewriter", "\\text_tt", "<Alt><Shift>T",
		  N_("Typewriter — \\texttt") },
		{ "win.latex-command-with-braces::textsl", "slanted", "\\text_sl", "<Alt><Shift>S",
		  N_("Slanted — \\textsl") },
		{ "win.latex-command-with-braces::textsc", "small_caps", "\\texts_c", "<Alt><Shift>C",
		  N_("Small Capitals — \\textsc") },
		{ "win.latex-command-with-braces::textsf", "sans_serif", "\\texts_f", NULL,
		  N_("Sans Serif — \\textsf") },
		{ "win.latex-command-with-braces::emph", NULL, "\\_emph", "<Control>E",
		  N_("Emphasized — \\emph") },
		{ "win.latex-command-with-braces::underline", "underline", "\\_underline", "<Control>U",
		  N_("Underline — \\underline") },

		{ "win.latex-command-char-style::rmfamily", "roman", "\\_rmfamily", NULL,
		  N_("Roman — \\rmfamily") },
		{ "win.latex-command-char-style::sffamily", "sans_serif", "\\_sffamily", NULL,
		  N_("Sans Serif — \\sffamily") },
		{ "win.latex-command-char-style::ttfamily", "typewriter", "\\_ttfamily", NULL,
		  N_("Monospace — \\ttfamily") },

		{ "win.latex-command-char-style::mdseries", "roman", "\\_mdseries", NULL,
		  N_("Medium — \\mdseries") },
		{ "win.latex-command-char-style::bfseries", "bold", "\\_bfseries", NULL,
		  N_("Bold — \\bfseries") },

		{ "win.latex-command-char-style::upshape", "roman", "\\_upshape", NULL,
		  N_("Upright — \\upshape") },
		{ "win.latex-command-char-style::itshape", "italic", "\\_itshape", NULL,
		  N_("Italic — \\itshape") },
		{ "win.latex-command-char-style::slshape", "slanted", "\\_slshape", NULL,
		  N_("Slanted — \\slshape") },
		{ "win.latex-command-char-style::scshape", "small_caps", "\\s_cshape", NULL,
		  N_("Small Capitals — \\scshape") },

		/* LaTeX: Tabular */

		{ "win.latex-command-env-simple::tabbing", NULL, "\\begin{ta_bbing}", NULL,
		  N_("Tabbing — \\begin{tabbing}") },
		{ "win.latex-command-tabular-tabular", NULL, "\\begin{_tabular}", NULL,
		  N_("Tabular — \\begin{tabular}") },
		{ "win.latex-command-tabular-multicolumn", NULL, "\\_multicolumn", NULL,
		  N_("Multicolumn — \\multicolumn") },
		{ "win.latex-command-with-space::hline", NULL, "\\_hline", NULL,
		  N_("Horizontal line — \\hline") },
		{ "win.latex-command-with-space::vline", NULL, "\\_vline", NULL,
		  N_("Vertical line — \\vline") },
		{ "win.latex-command-tabular-cline", NULL, "\\_cline", NULL,
		  N_("Horizontal line (columns specified) — \\cline") },

		/* LaTeX: Presentation */

		{ "win.latex-command-presentation-frame", NULL, "\\begin{frame}", NULL,
		  N_("Frame — \\begin{frame}") },
		{ "win.latex-command-presentation-block", NULL, "\\begin{block}", NULL,
		  N_("Block — \\begin{block}") },
		{ "win.latex-command-presentation-columns", NULL, "\\begin{columns}", NULL,
		  N_("Two columns — \\begin{columns}") },

		/* LaTeX: Spacing */

		{ "win.latex-command-spacing-new-line", NULL, N_("New _Line"), NULL,
		  N_("New Line — \\\\") },
		{ "win.latex-command-with-newline::newpage", NULL, "\\new_page", NULL,
		  N_("New page — \\newpage") },
		{ "win.latex-command-with-newline::linebreak", NULL, "\\l_inebreak", NULL,
		  N_("Line break — \\linebreak") },
		{ "win.latex-command-with-newline::pagebreak", NULL, "\\p_agebreak", NULL,
		  N_("Page break — \\pagebreak") },
		{ "win.latex-command-with-space::bigskip", NULL, "\\_bigskip", NULL,
		  N_("Big skip — \\bigskip") },
		{ "win.latex-command-with-space::medskip", NULL, "\\_medskip", NULL,
		  N_("Medium skip — \\medskip") },
		{ "win.latex-command-with-braces::hspace", NULL, "\\_hspace", NULL,
		  N_("Horizontal space — \\hspace") },
		{ "win.latex-command-with-braces::vspace", NULL, "\\_vspace", NULL,
		  N_("Vertical space — \\vspace") },
		{ "win.latex-command-with-space::noindent", NULL, "\\_noindent", NULL,
		  N_("No paragraph indentation — \\noindent") },

		/* LaTeX: International accents */

		{ "win.latex-command-with-braces('\\'')", "accent0", "\\'", NULL, N_("Acute accent — \\'") },
		{ "win.latex-command-with-braces('`')", "accent1", "\\`", NULL, N_("Grave accent — \\`") },
		{ "win.latex-command-with-braces('^')", "accent2", "\\^", NULL, N_("Circumflex accent — \\^") },
		{ "win.latex-command-with-braces('\"')", "accent3", "\\\"", NULL, N_("Trema — \\\"") },
		{ "win.latex-command-with-braces('~')", "accent4", "\\~", NULL, N_("Tilde — \\~") },
		{ "win.latex-command-with-braces('=')", "accent5", "\\=", NULL, N_("Macron — \\=") },
		{ "win.latex-command-with-braces('.')", "accent6", "\\.", NULL, N_("Dot above — \\.") },
		{ "win.latex-command-with-braces('v')", "accent7", "\\v", NULL, N_("Caron — \\v") },
		{ "win.latex-command-with-braces('u')", "accent8", "\\u", NULL, N_("Breve — \\u") },
		{ "win.latex-command-with-braces('H')", "accent9", "\\H", NULL,
		  N_("Double acute accent — \\H") },
		{ "win.latex-command-with-braces('c')", "accent10", "\\c", NULL, N_("Cedilla — \\c") },
		{ "win.latex-command-with-braces('k')", "accent11", "\\k", NULL, N_("Ogonek — \\k") },
		{ "win.latex-command-with-braces('d')", "accent12", "\\d", NULL, N_("Dot below — \\d") },
		{ "win.latex-command-with-braces('b')", "accent13", "\\b", NULL, N_("Macron below — \\b") },
		{ "win.latex-command-with-braces('r')", "accent14", "\\r", NULL, N_("Ring — \\r") },
		{ "win.latex-command-with-braces('t')", "accent15", "\\t", NULL, N_("Tie — \\t") },

		/* LaTeX: Misc */

		{ "win.latex-command-with-braces::documentclass", NULL, "\\_documentclass", NULL,
		  N_("Document class — \\documentclass") },
		{ "win.latex-command-with-braces::usepackage", NULL, "\\_usepackage", NULL,
		  N_("Use package — \\usepackage") },
		{ "win.latex-command-ams-packages", NULL, N_("_AMS packages"), NULL,
		  N_("AMS packages") },
		{ "win.latex-command-with-braces::author", NULL, "\\au_thor", NULL, N_("Author — \\author") },
		{ "win.latex-command-with-braces::title", NULL, "\\t_itle", NULL, N_("Title — \\title") },
		{ "win.latex-command-env-simple::document", NULL, "\\begin{d_ocument}", NULL,
		  N_("Content of the document — \\begin{document}") },
		{ "win.latex-command-simple::maketitle", NULL, "\\_maketitle", NULL,
		  N_("Make title — \\maketitle") },
		{ "win.latex-command-simple::tableofcontents", NULL, "\\tableof_contents", NULL,
		  N_("Table of contents — \\tableofcontents") },
		{ "win.latex-command-env-simple::abstract", NULL, "\\begin{abst_ract}", NULL,
		  N_("Abstract — \\begin{abstract}") },
		{ "win.latex-command-with-braces::includegraphics", NULL, "\\include_graphics", NULL,
		  N_("Include an image (graphicx package) — \\includegraphics") },
		{ "win.latex-command-with-braces::input", NULL, "\\_input", NULL,
		  N_("Include a file — \\input") },

		/* Math Environments */

		{ "win.math-command-env-normal", NULL, N_("_Mathematical Environment — $…$"),
		  "<Alt><Shift>M", N_("Mathematical Environment — $…$") },
		{ "win.math-command-env-centered", NULL, N_("_Centered Formula — \\[…\\]"),
		  "<Alt><Shift>E", N_("Centered Formula — \\[…\\]") },
		{ "win.latex-command-env-simple::equation", NULL,
		  N_("_Numbered Equation — \\begin{equation}"), NULL,
		  N_("Numbered Equation — \\begin{equation}") },
		{ "win.math-command-env-array", NULL, N_("_Array of Equations — \\begin{align*}"), NULL,
		  N_("Array of Equations — \\begin{align*}") },
		{ "win.latex-command-env-simple::align", NULL,
		  N_("Numbered Array of _Equations — \\begin{align}"), NULL,
		  N_("Numbered Array of Equations — \\begin{align}") },

		/* Math misc */

		{ "win.math-command-misc-superscript", "math-superscript", N_("_Superscript — ^{}"), NULL,
		  N_("Superscript — ^{}") },
		{ "win.math-command-misc-subscript", "math-subscript", N_("Su_bscript — __{}"), NULL,
		  N_("Subscript — _{}") },
		{ "win.math-command-misc-frac", "math-frac", N_("_Fraction — \\frac{}{}"), "<Alt><Shift>F",
		  N_("Fraction — \\frac{}{}") },
		{ "win.latex-command-with-braces::sqrt", "math-square-root", N_("Square _Root — \\sqrt{}"), NULL,
		  N_("Square Root — \\sqrt{}") },
		{ "win.math-command-misc-nth-root", "math-nth-root", N_("_N-th Root — \\sqrt[]{}"), NULL,
		  N_("N-th Root — \\sqrt[]{}") },

		/* Math functions */

		{ "win.latex-command-with-space::arccos", NULL, "\\arccos", NULL, NULL },
		{ "win.latex-command-with-space::arcsin", NULL, "\\arcsin", NULL, NULL },
		{ "win.latex-command-with-space::arctan", NULL, "\\arctan", NULL, NULL },
		{ "win.latex-command-with-space::cos", NULL, "\\cos", NULL, NULL },
		{ "win.latex-command-with-space::cosh", NULL, "\\cosh", NULL, NULL },
		{ "win.latex-command-with-space::cot", NULL, "\\cot", NULL, NULL },
		{ "win.latex-command-with-space::coth", NULL, "\\coth", NULL, NULL },
		{ "win.latex-command-with-space::csc", NULL, "\\csc", NULL, NULL },
		{ "win.latex-command-with-space::deg", NULL, "\\deg", NULL, NULL },
		{ "win.latex-command-with-space::det", NULL, "\\det", NULL, NULL },
		{ "win.latex-command-with-space::dim", NULL, "\\dim", NULL, NULL },
		{ "win.latex-command-with-space::exp", NULL, "\\exp", NULL, NULL },
		{ "win.latex-command-with-space::gcd", NULL, "\\gcd", NULL, NULL },
		{ "win.latex-command-with-space::hom", NULL, "\\hom", NULL, NULL },
		{ "win.latex-command-with-space::inf", NULL, "\\inf", NULL, NULL },
		{ "win.latex-command-with-space::ker", NULL, "\\ker", NULL, NULL },
		{ "win.latex-command-with-space::lg", NULL, "\\lg", NULL, NULL },
		{ "win.latex-command-with-space::lim", NULL, "\\lim", NULL, NULL },
		{ "win.latex-command-with-space::liminf", NULL, "\\liminf", NULL, NULL },
		{ "win.latex-command-with-space::limsup", NULL, "\\limsup", NULL, NULL },
		{ "win.latex-command-with-space::ln", NULL, "\\ln", NULL, NULL },
		{ "win.latex-command-with-space::log", NULL, "\\log", NULL, NULL },
		{ "win.latex-command-with-space::max", NULL, "\\max", NULL, NULL },
		{ "win.latex-command-with-space::min", NULL, "\\min", NULL, NULL },
		{ "win.latex-command-with-space::sec", NULL, "\\sec", NULL, NULL },
		{ "win.latex-command-with-space::sin", NULL, "\\sin", NULL, NULL },
		{ "win.latex-command-with-space::sinh", NULL, "\\sinh", NULL, NULL },
		{ "win.latex-command-with-space::sup", NULL, "\\sup", NULL, NULL },
		{ "win.latex-command-with-space::tan", NULL, "\\tan", NULL, NULL },
		{ "win.latex-command-with-space::tanh", NULL, "\\tanh", NULL, NULL },

		/* Math Font Styles */

		{ "win.latex-command-with-braces::mathrm", "roman", "\\math_rm", NULL,
		  N_("Roman — \\mathrm") },
		{ "win.latex-command-with-braces::mathit", "italic", "\\math_it", NULL,
		  N_("Italic — \\mathit") },
		{ "win.latex-command-with-braces::mathbf", "bold", "\\math_bf", NULL,
		  N_("Bold — \\mathbf") },
		{ "win.latex-command-with-braces::mathsf", "sans_serif", "\\math_sf", NULL,
		  N_("Sans Serif — \\mathsf") },
		{ "win.latex-command-with-braces::mathtt", "typewriter", "\\math_tt", NULL,
		  N_("Typewriter — \\mathtt") },
		{ "win.latex-command-with-braces::mathcal", "mathcal", "\\math_cal", NULL,
		  N_("Calligraphic — \\mathcal") },
		{ "win.latex-command-with-braces::mathbb", "blackboard", "\\_mathbb", NULL,
		  N_("Blackboard (uppercase only) — \\mathbb (amsfonts package)") },
		{ "win.latex-command-with-braces::mathfrak", "mathfrak", "\\math_frak", NULL,
		  N_("Euler Fraktur — \\mathfrak (amsfonts package)") },

		/* Math Accents */

		{ "win.latex-command-with-braces::acute", "mathaccent0", "\\_acute", NULL, NULL },
		{ "win.latex-command-with-braces::grave", "mathaccent1", "\\_grave", NULL, NULL },
		{ "win.latex-command-with-braces::tilde", "mathaccent2", "\\_tilde", NULL, NULL },
		{ "win.latex-command-with-braces::bar", "mathaccent3", "\\_bar", NULL, NULL },
		{ "win.latex-command-with-braces::vec", "mathaccent4", "\\_vec", NULL, NULL },
		{ "win.latex-command-with-braces::hat", "mathaccent5", "\\_hat", NULL, NULL },
		{ "win.latex-command-with-braces::check", "mathaccent6", "\\_check", NULL, NULL },
		{ "win.latex-command-with-braces::breve", "mathaccent7", "\\b_reve", NULL, NULL },
		{ "win.latex-command-with-braces::dot", "mathaccent8", "\\_dot", NULL, NULL },
		{ "win.latex-command-with-braces::ddot", "mathaccent9", "\\dd_ot", NULL, NULL },
		{ "win.latex-command-with-braces::mathring", "mathaccent10", "\\_mathring", NULL, NULL },

		/* Math Spaces */

		{ "win.math-command-spaces-small", NULL, N_("_Small"), NULL, N_("Small — \\,") },
		{ "win.math-command-spaces-medium", NULL, N_("_Medium"), NULL, N_("Medium — \\:") },
		{ "win.math-command-spaces-large", NULL, N_("_Large"), NULL, N_("Large — \\;") },
		{ "win.latex-command-with-space::quad", NULL, "\\_quad", NULL, NULL },
		{ "win.latex-command-with-space::qquad", NULL, "\\qqu_ad", NULL, NULL },

		/* Math: Left Delimiters */

		{ "win.math-command-delimiter-left1", NULL, N_("left ("), NULL, NULL },
		{ "win.math-command-delimiter-left2", NULL, N_("left ["), NULL, NULL },
		{ "win.math-command-delimiter-left3", NULL, N_("left { "), NULL, NULL },
		{ "win.math-command-delimiter-left4", NULL, N_("left <"), NULL, NULL },
		{ "win.math-command-delimiter-left5", NULL, N_("left )"), NULL, NULL },
		{ "win.math-command-delimiter-left6", NULL, N_("left ]"), NULL, NULL },
		{ "win.math-command-delimiter-left7", NULL, N_("left  }"), NULL, NULL },
		{ "win.math-command-delimiter-left8", NULL, N_("left >"), NULL, NULL },
		{ "win.math-command-delimiter-left9", NULL, N_("left ."), NULL, NULL },

		/* Math: Right Delimiters */

		{ "win.math-command-delimiter-right1", NULL, N_("right )"), NULL, NULL },
		{ "win.math-command-delimiter-right2", NULL, N_("right ]"), NULL, NULL },
		{ "win.math-command-delimiter-right3", NULL, N_("right  }"), NULL, NULL },
		{ "win.math-command-delimiter-right4", NULL, N_("right >"), NULL, NULL },
		{ "win.math-command-delimiter-right5", NULL, N_("right ("), NULL, NULL },
		{ "win.math-command-delimiter-right6", NULL, N_("right ["), NULL, NULL },
		{ "win.math-command-delimiter-right7", NULL, N_("right { "), NULL, NULL },
		{ "win.math-command-delimiter-right8", NULL, N_("right <"), NULL, NULL },
		{ "win.math-command-delimiter-right9", NULL, N_("right ."), NULL, NULL },
	};

	g_return_if_fail (GTK_IS_APPLICATION (gtk_app));

	tepl_app = tepl_application_get_from_gtk_application (gtk_app);
	store = tepl_application_get_app_action_info_store (tepl_app);

	amtk_action_info_store_add_entries (store,
					    entries,
					    G_N_ELEMENTS (entries),
					    GETTEXT_PACKAGE);
}

/* Util functions */

static void
insert_text (TeplApplicationWindow *tepl_window,
	     const gchar           *text_before,
	     const gchar           *text_after,
	     const gchar           *text_if_no_selection)
{
	TeplView *view;
	GtkTextBuffer *buffer;
	GtkTextIter selection_start;
	GtkTextIter selection_end;
	gboolean has_selection;
	gchar *text_before_with_indent;
	gchar *text_after_with_indent;

	g_return_if_fail (TEPL_IS_APPLICATION_WINDOW (tepl_window));
	g_return_if_fail (text_before != NULL);
	g_return_if_fail (text_after != NULL);

	view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (view != NULL);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

	has_selection = gtk_text_buffer_get_selection_bounds (buffer,
							      &selection_start,
							      &selection_end);

	/* Take into account the current indentation. */
	{
		gchar *current_indent;
		gchar *newline_replacement;

		current_indent = tepl_iter_get_line_indentation (&selection_start);
		newline_replacement = g_strdup_printf ("\n%s", current_indent);

		text_before_with_indent = tepl_utils_str_replace (text_before, "\n", newline_replacement);
		text_after_with_indent = tepl_utils_str_replace (text_after, "\n", newline_replacement);

		g_free (current_indent);
		g_free (newline_replacement);
	}

	gtk_text_buffer_begin_user_action (buffer);

	/* Insert around the selected text.
	 * Move the cursor to the end.
	 */
	if (has_selection)
	{
		GtkTextMark *end_mark;
		GtkTextIter end_iter;

		end_mark = gtk_text_buffer_create_mark (buffer, NULL, &selection_end, FALSE);

		gtk_text_buffer_insert (buffer, &selection_start, text_before_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &end_iter, end_mark);
		gtk_text_buffer_delete_mark (buffer, end_mark);

		gtk_text_buffer_insert (buffer, &end_iter, text_after_with_indent, -1);
		gtk_text_buffer_place_cursor (buffer, &end_iter);
	}
	/* No selection */
	else if (text_if_no_selection != NULL)
	{
		gtk_text_buffer_insert_at_cursor (buffer, text_if_no_selection, -1);
	}
	/* No selection, move the cursor between the two inserted chunks. */
	else
	{
		GtkTextIter between_iter;
		GtkTextMark *between_mark;

		gtk_text_buffer_insert_at_cursor (buffer, text_before_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer,
						  &between_iter,
						  gtk_text_buffer_get_insert (buffer));
		between_mark = gtk_text_buffer_create_mark (buffer, NULL, &between_iter, TRUE);

		gtk_text_buffer_insert_at_cursor (buffer, text_after_with_indent, -1);

		gtk_text_buffer_get_iter_at_mark (buffer, &between_iter, between_mark);
		gtk_text_buffer_delete_mark (buffer, between_mark);
		gtk_text_buffer_place_cursor (buffer, &between_iter);
	}

	gtk_text_buffer_end_user_action (buffer);

	gtk_widget_grab_focus (GTK_WIDGET (view));

	g_free (text_before_with_indent);
	g_free (text_after_with_indent);
}

static gchar *
get_indentation (TeplApplicationWindow *tepl_window)
{
	TeplView *view;

	view = tepl_tab_group_get_active_view (TEPL_TAB_GROUP (tepl_window));
	g_return_val_if_fail (view != NULL, NULL);

	return latexila_view_get_indentation_style (GTK_SOURCE_VIEW (view));
}

/* When it doesn't make sense to have the selected text between @text_before and
 * @text_after when calling insert_text(), call this function first.
 */
static void
deselect_text (TeplApplicationWindow *tepl_window)
{
	TeplBuffer *tepl_buffer;
	GtkTextBuffer *gtk_buffer;
	GtkTextIter selection_start;
	GtkTextIter selection_end;

	tepl_buffer = tepl_tab_group_get_active_buffer (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (tepl_buffer != NULL);

	gtk_buffer = GTK_TEXT_BUFFER (tepl_buffer);

	gtk_text_buffer_get_selection_bounds (gtk_buffer,
					      &selection_start,
					      &selection_end);

	/* Always place cursor at beginning of selection, to have more predictable
	 * results, instead of placing the cursor at the insert mark.
	 */
	gtk_text_buffer_place_cursor (gtk_buffer, &selection_start);
}

/* GActions implementation */

static void
latex_command_simple_cb (GSimpleAction *action,
			 GVariant      *parameter,
			 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s", command);

	insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_with_braces_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s{", command);

	insert_text (tepl_window, text_before, "}", NULL);

	g_free (text_before);
}

static void
latex_command_with_space_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s ", command);

	insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_with_newline_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *command;
	gchar *text_before;

	command = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\%s\n", command);

	insert_text (tepl_window, text_before, "", NULL);

	g_free (text_before);
}

static void
latex_command_env_simple_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *environment;
	gchar *text_before;
	gchar *text_after;

	environment = g_variant_get_string (parameter, NULL);
	text_before = g_strdup_printf ("\\begin{%s}\n", environment);
	text_after = g_strdup_printf ("\n\\end{%s}", environment);

	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_env_figure_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{figure}\n"
				       "%s\\begin{center}\n"
				       "%s%s\\includegraphics{",
				       indent,
				       indent, indent);

	text_after = g_strdup_printf ("}\n"
				      "%s%s\\caption{}\n"
				      "%s%s\\label{fig:}\n"
				      "%s\\end{center}\n"
				      "\\end{figure}",
				      indent, indent,
				      indent, indent,
				      indent);

	deselect_text (tepl_window);
	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_env_table_cb (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{table}\n"
				       "%s\\caption{",
				       indent);

	text_after = g_strdup_printf ("}\n"
				      "%s\\label{tab:}\n"
				      "\n"
				      "%s\\begin{center}\n"
				      "%s%s\\begin{tabular}{cc}\n"
				      "%s%s%s a & b \\\\\n"
				      "%s%s%s c & d \\\\\n"
				      "%s%s\\end{tabular}\n"
				      "%s\\end{center}\n"
				      "\\end{table}",
				      indent,
				      indent,
				      indent, indent,
				      indent, indent, indent,
				      indent, indent, indent,
				      indent, indent,
				      indent);

	deselect_text (tepl_window);
	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_simple_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *list_env;
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	list_env = g_variant_get_string (parameter, NULL);

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{%s}\n"
				       "%s\\item ",
				       list_env,
				       indent);

	text_after = g_strdup_printf ("\n\\end{%s}", list_env);

	deselect_text (tepl_window);
	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_description_cb (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{description}\n"
				       "%s\\item[",
				       indent);

	text_after = g_strdup_printf ("] \n\\end{description}");

	deselect_text (tepl_window);
	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_list_env_list_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_after;

	indent = get_indentation (tepl_window);
	text_after = g_strdup_printf ("}{}\n%s\\item \n\\end{list}", indent);

	deselect_text (tepl_window);
	insert_text (tepl_window, "\\begin{list}{", text_after, NULL);

	g_free (indent);
	g_free (text_after);
}

static void
latex_command_char_style_cb (GSimpleAction *action,
			     GVariant      *parameter,
			     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	const gchar *style;
	TeplBuffer *buffer;
	TeplSelectionType selection_type;
	gchar *text_before;
	gchar *text_after;
	gchar *text_if_no_selection;

	style = g_variant_get_string (parameter, NULL);

	buffer = tepl_tab_group_get_active_buffer (TEPL_TAB_GROUP (tepl_window));
	g_return_if_fail (buffer != NULL);

	selection_type = tepl_buffer_get_selection_type (buffer);

	if (selection_type == TEPL_SELECTION_TYPE_MULTIPLE_LINES)
	{
		text_before = g_strdup_printf ("\\begin{%s}\n", style);
		text_after = g_strdup_printf ("\n\\end{%s}", style);
		text_if_no_selection = NULL;
	}
	else
	{
		text_before = g_strdup_printf ("{\\%s ", style);
		text_after = g_strdup_printf ("}");
		text_if_no_selection = g_strdup_printf ("\\%s ", style);
	}

	insert_text (tepl_window,
		     text_before,
		     text_after,
		     text_if_no_selection);

	g_free (text_before);
	g_free (text_after);
	g_free (text_if_no_selection);
}

static void
latex_command_tabular_tabular_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_after;

	indent = get_indentation (tepl_window);
	text_after = g_strdup_printf ("}\n"
				      "%s a & b \\\\\n"
				      "%s c & d \\\\\n"
				      "\\end{tabular}",
				      indent,
				      indent);

	deselect_text (tepl_window);
	insert_text (tepl_window,
		     "\\begin{tabular}{cc",
		     text_after,
		     NULL);

	g_free (indent);
	g_free (text_after);
}

static void
latex_command_tabular_multicolumn_cb (GSimpleAction *action,
				      GVariant      *parameter,
				      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\multicolumn{}{}{", "}", NULL);
}

static void
latex_command_tabular_cline_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\cline{", "-}", NULL);
}

static void
latex_command_presentation_frame_cb (GSimpleAction *action,
				     GVariant      *parameter,
				     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;

	indent = get_indentation (tepl_window);
	text_before = g_strdup_printf ("\\begin{frame}\n"
				       "%s\\frametitle{}\n"
				       "%s\\framesubtitle{}\n",
				       indent,
				       indent);

	insert_text (tepl_window,
		     text_before,
		     "\n\\end{frame}",
		     NULL);

	g_free (indent);
	g_free (text_before);
}

static void
latex_command_presentation_block_cb (GSimpleAction *action,
				     GVariant      *parameter,
				     gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window,
		     "\\begin{block}{}\n",
		     "\n\\end{block}",
		     NULL);
}

static void
latex_command_presentation_columns_cb (GSimpleAction *action,
				       GVariant      *parameter,
				       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);
	gchar *indent;
	gchar *text_before;
	gchar *text_after;

	indent = get_indentation (tepl_window);

	text_before = g_strdup_printf ("\\begin{columns}\n"
				       "%s\\begin{column}{.5\\textwidth}\n",
				       indent);

	text_after = g_strdup_printf ("\n"
				      "%s\\end{column}\n"
				      "%s\\begin{column}{.5\\textwidth}\n\n"
				      "%s\\end{column}\n"
				      "\\end{columns}",
				      indent,
				      indent,
				      indent);

	insert_text (tepl_window, text_before, text_after, NULL);

	g_free (indent);
	g_free (text_before);
	g_free (text_after);
}

static void
latex_command_spacing_new_line_cb (GSimpleAction *action,
				   GVariant      *parameter,
				   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\\\\n", "", NULL);
}

static void
latex_command_ams_packages_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window,
		     "\\usepackage{amsmath}\n"
		     "\\usepackage{amsfonts}\n"
		     "\\usepackage{amssymb}",
		     "",
		     NULL);
}

static void
math_command_env_normal_cb (GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "$ ", " $", NULL);
}

static void
math_command_env_centered_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\[ ", " \\]", NULL);
}

static void
math_command_env_array_cb (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window,
		     "\\begin{align*}\n",
		     "\n\\end{align*}",
		     NULL);
}

static void
math_command_misc_superscript_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "^{", "}", NULL);
}

static void
math_command_misc_subscript_cb (GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "_{", "}", NULL);
}

static void
math_command_misc_frac_cb (GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\frac{", "}{}", NULL);
}

static void
math_command_misc_nth_root_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\sqrt[]{", "}", NULL);
}

static void
math_command_spaces_small_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\, ", "", NULL);
}

static void
math_command_spaces_medium_cb (GSimpleAction *action,
			       GVariant      *parameter,
			       gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\: ", "", NULL);
}

static void
math_command_spaces_large_cb (GSimpleAction *action,
			      GVariant      *parameter,
			      gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\; ", "", NULL);
}

static void
math_command_delimiter_left1_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left( ", "", NULL);
}

static void
math_command_delimiter_left2_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left[ ", "", NULL);
}

static void
math_command_delimiter_left3_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left\\lbrace ", "", NULL);
}

static void
math_command_delimiter_left4_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left\\langle ", "", NULL);
}

static void
math_command_delimiter_left5_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left) ", "", NULL);
}

static void
math_command_delimiter_left6_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left] ", "", NULL);
}

static void
math_command_delimiter_left7_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left\\rbrace ", "", NULL);
}

static void
math_command_delimiter_left8_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left\\rangle ", "", NULL);
}

static void
math_command_delimiter_left9_cb (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\left. ", "", NULL);
}

static void
math_command_delimiter_right1_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right) ", "", NULL);
}

static void
math_command_delimiter_right2_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right] ", "", NULL);
}

static void
math_command_delimiter_right3_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right\\rbrace ", "", NULL);
}

static void
math_command_delimiter_right4_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right\\rangle ", "", NULL);
}

static void
math_command_delimiter_right5_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right( ", "", NULL);
}

static void
math_command_delimiter_right6_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right[ ", "", NULL);
}

static void
math_command_delimiter_right7_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right\\lbrace ", "", NULL);
}

static void
math_command_delimiter_right8_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right\\langle ", "", NULL);
}

static void
math_command_delimiter_right9_cb (GSimpleAction *action,
				  GVariant      *parameter,
				  gpointer       user_data)
{
	TeplApplicationWindow *tepl_window = TEPL_APPLICATION_WINDOW (user_data);

	insert_text (tepl_window, "\\right. ", "", NULL);
}

/**
 * latexila_latex_commands_add_actions:
 * @gtk_window: a #GtkApplicationWindow.
 *
 * Creates the #GAction's related to the LaTeX and Math menus, and add them to
 * @gtk_window.
 */
void
latexila_latex_commands_add_actions (GtkApplicationWindow *gtk_window)
{
	TeplApplicationWindow *tepl_window;

	const GActionEntry entries[] = {
		{ "latex-command-simple", latex_command_simple_cb, "s" },
		{ "latex-command-with-braces", latex_command_with_braces_cb, "s" },
		{ "latex-command-with-space", latex_command_with_space_cb, "s" },
		{ "latex-command-with-newline", latex_command_with_newline_cb, "s" },
		{ "latex-command-env-simple", latex_command_env_simple_cb, "s" },
		{ "latex-command-env-figure", latex_command_env_figure_cb },
		{ "latex-command-env-table", latex_command_env_table_cb },
		{ "latex-command-list-env-simple", latex_command_list_env_simple_cb, "s" },
		{ "latex-command-list-env-description", latex_command_list_env_description_cb },
		{ "latex-command-list-env-list", latex_command_list_env_list_cb },
		{ "latex-command-char-style", latex_command_char_style_cb, "s" },
		{ "latex-command-tabular-tabular", latex_command_tabular_tabular_cb },
		{ "latex-command-tabular-multicolumn", latex_command_tabular_multicolumn_cb },
		{ "latex-command-tabular-cline", latex_command_tabular_cline_cb },
		{ "latex-command-presentation-frame", latex_command_presentation_frame_cb },
		{ "latex-command-presentation-block", latex_command_presentation_block_cb },
		{ "latex-command-presentation-columns", latex_command_presentation_columns_cb },
		{ "latex-command-spacing-new-line", latex_command_spacing_new_line_cb },
		{ "latex-command-ams-packages", latex_command_ams_packages_cb },
		{ "math-command-env-normal", math_command_env_normal_cb },
		{ "math-command-env-centered", math_command_env_centered_cb },
		{ "math-command-env-array", math_command_env_array_cb },
		{ "math-command-misc-superscript", math_command_misc_superscript_cb },
		{ "math-command-misc-subscript", math_command_misc_subscript_cb },
		{ "math-command-misc-frac", math_command_misc_frac_cb },
		{ "math-command-misc-nth-root", math_command_misc_nth_root_cb },
		{ "math-command-spaces-small", math_command_spaces_small_cb },
		{ "math-command-spaces-medium", math_command_spaces_medium_cb },
		{ "math-command-spaces-large", math_command_spaces_large_cb },
		{ "math-command-delimiter-left1", math_command_delimiter_left1_cb },
		{ "math-command-delimiter-left2", math_command_delimiter_left2_cb },
		{ "math-command-delimiter-left3", math_command_delimiter_left3_cb },
		{ "math-command-delimiter-left4", math_command_delimiter_left4_cb },
		{ "math-command-delimiter-left5", math_command_delimiter_left5_cb },
		{ "math-command-delimiter-left6", math_command_delimiter_left6_cb },
		{ "math-command-delimiter-left7", math_command_delimiter_left7_cb },
		{ "math-command-delimiter-left8", math_command_delimiter_left8_cb },
		{ "math-command-delimiter-left9", math_command_delimiter_left9_cb },
		{ "math-command-delimiter-right1", math_command_delimiter_right1_cb },
		{ "math-command-delimiter-right2", math_command_delimiter_right2_cb },
		{ "math-command-delimiter-right3", math_command_delimiter_right3_cb },
		{ "math-command-delimiter-right4", math_command_delimiter_right4_cb },
		{ "math-command-delimiter-right5", math_command_delimiter_right5_cb },
		{ "math-command-delimiter-right6", math_command_delimiter_right6_cb },
		{ "math-command-delimiter-right7", math_command_delimiter_right7_cb },
		{ "math-command-delimiter-right8", math_command_delimiter_right8_cb },
		{ "math-command-delimiter-right9", math_command_delimiter_right9_cb },
	};

	g_return_if_fail (GTK_IS_APPLICATION_WINDOW (gtk_window));

	tepl_window = tepl_application_window_get_from_gtk_application_window (gtk_window);

	amtk_action_map_add_action_entries_check_dups (G_ACTION_MAP (gtk_window),
						       entries,
						       G_N_ELEMENTS (entries),
						       tepl_window);
}

/**
 * latexila_latex_commands_get_edit_toolbar:
 *
 * Returns: (transfer floating): a new #GtkToolbar with the most common LaTeX
 * actions.
 */
GtkToolbar *
latexila_latex_commands_get_edit_toolbar (void)
{
	GtkToolbar *toolbar;

	toolbar = GTK_TOOLBAR (gtk_toolbar_new ());
	gtk_widget_show_all (GTK_WIDGET (toolbar));

	return toolbar;
}
