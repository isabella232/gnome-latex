/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright © 2010-2020 Sébastien Wilmet
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

using Gtk;

public class LatexMenu : Gtk.ActionGroup
{
    private const Gtk.ActionEntry[] latex_action_entries =
    {
        /* Sections and sub-sections */

        // LaTeX
        { "Latex", null, "_LaTeX" },

        // Math
        { "Math", null, N_("_Math") },
        { "MathEnvironments", null, N_("_Math Environments") },
        { "MathFunctions", null, N_("Math _Functions") },
        { "MathFontStyles", null, N_("Math Font _Styles") },
        { "MathAccents", null, N_("Math _Accents") },
        { "MathSpaces", null, N_("Math _Spaces") },
        { "MathLeftDelimiters", "delimiters-left", N_("_Left Delimiters") },
        { "MathRightDelimiters", "delimiters-right", N_("Right _Delimiters") },
    };

    private unowned MainWindow main_window;

    public LatexMenu (MainWindow main_window)
    {
        GLib.Object (name: "LatexActionGroup");
        set_translation_domain (Config.GETTEXT_PACKAGE);

        this.main_window = main_window;

        add_actions (latex_action_entries, this);

        /* GActions */

        Latexila.latex_commands_add_actions (main_window);

        // Math Environments
        Amtk.utils_create_gtk_action (main_window, "win.math-command-env-normal",
            this, "MathEnvNormal");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-env-centered",
            this, "MathEnvCentered");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-env-simple::equation",
            this, "MathEnvNumbered");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-env-array",
            this, "MathEnvArray");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-env-simple::align",
            this, "MathEnvNumberedArray");

        // Math misc
        Amtk.utils_create_gtk_action (main_window, "win.math-command-misc-superscript",
            this, "MathSuperscript");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-misc-subscript",
            this, "MathSubscript");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-misc-frac",
            this, "MathFrac");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::sqrt",
            this, "MathSquareRoot");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-misc-nth-root",
            this, "MathNthRoot");

        // Math functions
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::arccos",
            this, "MathFuncArccos");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::arcsin",
            this, "MathFuncArcsin");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::arctan",
            this, "MathFuncArctan");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::cos",
            this, "MathFuncCos");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::cosh",
            this, "MathFuncCosh");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::cot",
            this, "MathFuncCot");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::coth",
            this, "MathFuncCoth");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::csc",
            this, "MathFuncCsc");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::deg",
            this, "MathFuncDeg");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::det",
            this, "MathFuncDet");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::dim",
            this, "MathFuncDim");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::exp",
            this, "MathFuncExp");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::gcd",
            this, "MathFuncGcd");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::hom",
            this, "MathFuncHom");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::inf",
            this, "MathFuncInf");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::ker",
            this, "MathFuncKer");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::lg",
            this, "MathFuncLg");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::lim",
            this, "MathFuncLim");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::liminf",
            this, "MathFuncLiminf");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::limsup",
            this, "MathFuncLimsup");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::ln",
            this, "MathFuncLn");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::log",
            this, "MathFuncLog");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::max",
            this, "MathFuncMax");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::min",
            this, "MathFuncMin");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::sec",
            this, "MathFuncSec");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::sin",
            this, "MathFuncSin");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::sinh",
            this, "MathFuncSinh");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::sup",
            this, "MathFuncSup");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::tan",
            this, "MathFuncTan");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::tanh",
            this, "MathFuncTanh");

        // Math Font Styles
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathrm",
            this, "MathFSrm");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathit",
            this, "MathFSit");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathbf",
            this, "MathFSbf");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathsf",
            this, "MathFSsf");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathtt",
            this, "MathFStt");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathcal",
            this, "MathFScal");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathbb",
            this, "MathFSbb");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathfrak",
            this, "MathFSfrak");

        // Math Accents
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::acute",
            this, "MathAccentAcute");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::grave",
            this, "MathAccentGrave");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::tilde",
            this, "MathAccentTilde");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::bar",
            this, "MathAccentBar");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::vec",
            this, "MathAccentVec");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::hat",
            this, "MathAccentHat");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::check",
            this, "MathAccentCheck");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::breve",
            this, "MathAccentBreve");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::dot",
            this, "MathAccentDot");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::ddot",
            this, "MathAccentDdot");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-braces::mathring",
            this, "MathAccentRing");

        // Math Spaces
        Amtk.utils_create_gtk_action (main_window, "win.math-command-spaces-small",
            this, "MathSpaceSmall");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-spaces-medium",
            this, "MathSpaceMedium");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-spaces-large",
            this, "MathSpaceLarge");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::quad",
            this, "MathSpaceQuad");
        Amtk.utils_create_gtk_action (main_window, "win.latex-command-with-space::qquad",
            this, "MathSpaceQquad");

        // Math Left Delimiters
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left1",
            this, "MathLeftDelimiter1");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left2",
            this, "MathLeftDelimiter2");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left3",
            this, "MathLeftDelimiter3");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left4",
            this, "MathLeftDelimiter4");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left5",
            this, "MathLeftDelimiter5");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left6",
            this, "MathLeftDelimiter6");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left7",
            this, "MathLeftDelimiter7");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left8",
            this, "MathLeftDelimiter8");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-left9",
            this, "MathLeftDelimiter9");

        // Math Right Delimiters
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right1",
            this, "MathRightDelimiter1");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right2",
            this, "MathRightDelimiter2");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right3",
            this, "MathRightDelimiter3");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right4",
            this, "MathRightDelimiter4");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right5",
            this, "MathRightDelimiter5");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right6",
            this, "MathRightDelimiter6");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right7",
            this, "MathRightDelimiter7");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right8",
            this, "MathRightDelimiter8");
        Amtk.utils_create_gtk_action (main_window, "win.math-command-delimiter-right9",
            this, "MathRightDelimiter9");
    }
}
