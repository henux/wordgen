/*
Copyright (C) 2008 Henri Hakkinen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config.h>
#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gettext.h"
#define _(String) gettext(String)

/* Minium and maxium lengths of generated words. (-l, --length) */
int min_length;
int max_length;

/* Output filename. (-o, --output) */
char *output_file;

/* Source character string. (-s, --source) */
char *source;

/* Prefix to be appended to each generated word. (--prefix) */
char *prefix;

/* Suffix to be prepended to each generated word. (--suffix) */
char *suffix;

/* Print message to stderr and exit with error code. */
static void
error (const char *format, ...)
{
  va_list args;

  va_start (args, format);
  fprintf (stderr, "%s: ", PACKAGE);
  vfprintf (stderr, format, args);
  if (errno)
    fprintf (stderr, ": %s", strerror (errno));
  fputc ('\n', stderr);
  va_end (args);
  exit (1);
}

/* Print help to stdout and exit. */
static void
usage ()
{
  printf (_("Usage: %s [OPTIONS]\n"), PACKAGE);
  fputs (_("Generate word lists.\n"
           "\nSource characters:\n"
	   "  -s, --source=SOURCE    use SOURCE as the source string\n"
	   "      --lowercase        use ASCII lowercase characters as SOURCE\n"
	   "      --uppercase        use ASCII uppercase characters as SOURCE\n"
	   "      --digits           use digits as SOURCE\n"
	   "      --xdigits          use lowercase hexadecimal digits as SOURCE\n"
	   "      --Xdigits          use uppercase hexadecimal digits as SOURCE\n"
	   "      --punct            use ASCII punctuation marks as SOURCE\n"
	   "      --alpha            use all ASCII characters as SOURCE\n"
	   "      --alnum            use all ASCII characters and digits as SOURCE\n"
	   "      --ascii            use all printable 7-bit ASCII characters as SOURCE\n"
           "\nBy default, SOURCE contains all ASCII characters and digits.\n"
           "\nLength control:\n"
	   "  -l, --length=LEN       length of generated words\n"
           "  -l, --length=MIN-MAX   minium and maxium length of generated words\n"
           "\nThe default length for words is 6.\n"
           "\nOutput control:\n"
           "  -o, --output=FILE      output filename\n"
	   "  -P, --prefix=STR       prepend words with STR\n"
	   "  -S, --suffix=STR       append words with STR\n"
           "\nBy default, words are outputted to stdout.\n"
           "\nMiscellaneous:\n"
	   "      --help             display this help and exit\n"
	   "      --version          output version information and exit\n"),
         stdout);

  /* TRANSLATORS: The placeholder indicates the bug-reporting address
     for this package. Please add _another line_ saying
     "Report translation bugs to <...>\n" with the address for
     translation bugs (typically your translation team's web or email
     address). */
  printf (_("\nReport bugs to <%s>.\n"), PACKAGE_BUGREPORT);

  exit (0);
}

/* Print version and license information to stdout and exit. */
static void
version ()
{
  printf ("%s\n", PACKAGE_STRING);
  printf (_("Copyright (C) 2008 %s.\n"),
	  /* TRANSLATORS: This is a proper name. See the gettext
	     manual, section Names. Note this is actually a non-ASCII
	     name: The last name is (with Unicode escapes)
	     "H\u00e4kkinen" or (with HTML entities) "H&auml;kkinen". */
	  _("Henri Hakkinen"));
  fputs (_("License GPLv3+: GNU GPL version 3 or later <http://gnu."
	   "org/licenses/gpl.html>\nThis is free software: you are "
	   "free to change and redistribute it.\nThere is NO WARRAN"
	   "TY, to the extent permitted by law.\n"), stdout);
  printf (_("\nWritten by %s.\n"), _("Henri Hakkinen"));
  exit (0);
}

/* Duplicate string with error checking. */
static char *
xstrdup (const char *s)
{
  char *x;
  x = strdup (s);
  if (x == NULL)
    error (_("out of memory"));
}

/* Parse lengths from a string. The string can be of two different
   formats: "NUM" and "NUM-NUM", where NUM is positive integer. */
static void
parselen (const char *s)
{
  int n;
  n = sscanf (s, "%d-%d", &min_length, &max_length);
  if (n == 1)
    max_length = min_length;
  else if (n == 0)
      error (_("misformatted length"));
  if (min_length <= 0 || max_length <= 0)
    error (_("length must be greater than 0"));
  else if (min_length > max_length)
    error (_("min length must be less than max length"));
}

/* Process command line options. */
static void
options (int argc, char **argv)
{
  int opt;
  char *endptr;
  struct option longopts[] = {
    {"source", 1, NULL, 's'},
    {"lowercase", 0, NULL, '0'},
    {"uppercase", 0, NULL, '1'},
    {"digits", 0, NULL, '2'},
    {"xdigits", 0, NULL, '3'},
    {"Xdigits", 0, NULL, '4'},
    {"punct", 0, NULL, '5'},
    {"alpha", 0, NULL, '6'},
    {"alnum", 0, NULL, '7'},
    {"ascii", 0, NULL, '8'},
    {"length", 1, NULL, 'l'},
    {"output", 1, NULL, 'o'},
    {"prefix", 1, NULL, 'P'},
    {"suffix", 1, NULL, 'S'},
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'v'},
    {0}
  };

  /* Default options. */
  min_length = 6;
  max_length = 6;
  output_file = NULL;
  source = NULL;
  prefix = NULL;
  suffix = NULL;

  while ((opt = getopt_long (argc, argv, "s:l:o:P:S:", longopts, NULL)) != -1)
    {
      switch (opt)
        {
        case 's':
          /* -s, --source */
          free (source);
          source = xstrdup (optarg);
          break;
        case '0':
          /* --lowercase */
          free (source);
          source = xstrdup ("abcdefghijklmnopqrstuvwxyz");
          break;
        case '1':
          /* --uppercase */
          free (source);
          source = xstrdup ("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
          break;
        case '2':
          /* --digits */
          free (source);
          source = xstrdup ("0123456789");
          break;
        case '3':
          /* --xdigits */
          free (source);
          source = xstrdup ("0123456789abcdef");
          break;
        case '4':
          /* --Xdigits */
          free (source);
          source = xstrdup ("0123456789ABCDEF");
          break;
        case '5':
          /* --punct */
          free (source);
          source = xstrdup ("!\"#$%&'()*+,-./:" ";<=>?@[\\]^_`{|}~");
          break;
        case '6':
          /* --alpha */
          free (source);
          source = xstrdup ("abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXZY");
          break;
        case '7':
          /* --alnum */
          free (source);
          source= xstrdup ("abcdefghijklmnopqrstuvwxyzABCDE"
                           "FGHIJKLMNOPQRSTUVWXZY0123456789");
          break;
        case '8':
          /* --ascii */
          free (source);
          source = xstrdup ("!\"#$%&'()*+,-./0123456789:;<=>?@"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                            "abcdefghijklmnopqrstuvwxyz{|}~");
          break;
        case 'l':
          /* -l, --length */
          parselen (optarg);
          break;
        case 'o':
          /* -o, --output */
          free (output_file);
          output_file = xstrdup (optarg);
          break;
        case 'P':
          /* -P, --prefix */
          free (prefix);
          prefix = xstrdup (optarg);
          break;
        case 'S':
          /* -S, --suffix */
          free (suffix);
          suffix = xstrdup (optarg);
          break;
        case 'h':
          /* --help */
          usage ();
          break;
        case 'v':
          /* --version */
          version ();
          break;
        default:
          fprintf (stderr,
                   _("Try `%s --help' for more information.\n"), PACKAGE);
          exit (1);
        }
    }

  /* The default source string, if none was given. */
  if (source == NULL)
    source = xstrdup ("abcdefghijklmnopqrstuvwxyzABCDE"
                      "FGHIJKLMNOPQRSTUVWXYZ0123456789");
}

int
main (int argc, char **argv)
{ 
  FILE *fp;
  size_t setlen;
  size_t *tab;
  int t, i;

  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  /* Parse the command line options. */
  options (argc, argv);

  setlen = strlen (source);
  if (setlen < 1)
    return 0;

  /* Allocate buffer for the word generator. */
  tab = (size_t *) calloc (max_length, sizeof (size_t));
  if (tab == NULL)
    error (_("out of memory"));
  
  /* Open the output file, if given. */
  fp = (output_file == NULL ? stdout : fopen (output_file, "w"));
  if (fp == NULL)
    error (_("can't open %s"), output_file);

  /* Generate the words. */
  for (t = min_length; t <= max_length; ++t)
    {
      memset (tab, 0, sizeof (size_t) * t);

      do
        {
          /* Output the word. */
          if (prefix != NULL)
            fputs (prefix, fp);
          for (i = 0; i < t; ++i)
            fputc (source[tab[i]], fp);
          if (suffix != NULL)
            fputs (suffix, fp);
          fputc ('\n', fp);

          /* Increase character indices. */
          ++tab[0];
          for (i = 0; i < t - 1; ++i)
            {
              if (tab[i] >= setlen)
                {
                  tab[i] = 0;
                  ++tab[i + 1];
                }
            }
        }
      while (tab[t - 1] < setlen);
    }

  /* Cleanup. */
  free (tab);
  free (suffix);
  free (prefix);
  free (source);
  free (output_file);

  if (fp != stdout)
    fclose (fp);

  return 0;
}
