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
 *
 * Author: Sébastien Wilmet
 */

int main (string[] argv)
{
    Latexila.init ();
    Factory factory = new Factory ();
    factory.set_singleton ();

    GlatexApp app = new GlatexApp ();
    int status = app.run (argv);

    Latexila.finalize ();
    return status;
}
