/*
 * This file is part of GNOME LaTeX.
 *
 * Copyright (C) 2014 - Sébastien Wilmet <swilmet@gnome.org>
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

#include "latexila.h"

static void
test_str_replace (void)
{
	gchar *result;

	result = latexila_utils_str_replace ("$filename", "$filename", "blah");
	g_assert_cmpstr (result, ==, "blah");
	g_free (result);

	result = latexila_utils_str_replace ("$shortname.pdf", "$shortname", "blah");
	g_assert_cmpstr (result, ==, "blah.pdf");
	g_free (result);

	result = latexila_utils_str_replace ("abcdabcd", "ab", "r");
	g_assert_cmpstr (result, ==, "rcdrcd");
	g_free (result);

	result = latexila_utils_str_replace ("abcd", "ef", "r");
	g_assert_cmpstr (result, ==, "abcd");
	g_free (result);
}

gint
main (gint    argc,
      gchar **argv)
{
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/utils/str-replace", test_str_replace);

	return g_test_run ();
}
