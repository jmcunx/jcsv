/*
 * Copyright (c) 2014 2015 2016 ... 2023 2024
 *     John McCue <jmccue@jmcunx.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * jcsv_u.c -- common routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jcsv.h"

/*
 * open_in() -- open in file
 */
int open_in(FILE **in, char *fname, FILE *fp_err)

{

  if (fname == (char *) NULL)
    {
      (*in) = stdin;
      return((int) TRUE);
    }
  if (strcmp(fname, FILE_NAME_STDIN) == 0)
    {
      (*in) = stdin;
      return((int) TRUE);
    }

  (*in) = fopen(fname, "r");

  if ((*in) == (FILE *) NULL)
    {
      fprintf(fp_err, MSG_WARN_W002, fname, strerror(errno));
      return((int) FALSE);
    }
  
  return((int) TRUE);

} /* open_in() */

/*
 * open_out() -- save the file anem and check status
 */
int open_out(FILE *wfp, struct s_file_info *f, char *prog_name, int force)

{

  if (f->fname == (char *) NULL)
    return((int) FALSE);
  if (strcmp(f->fname, FILE_NAME_STDOUT) == 0)
    return((int) FALSE);

  if (force == (int) FALSE)
    {
      if ( j2_f_exist(f->fname) )
	{
	  fprintf(wfp, MSG_ERR_E025, f->fname);
	  fprintf(wfp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  f->fp = fopen(f->fname, "w");
  if (f->fp == (FILE *) NULL)
    {
      fprintf(wfp, MSG_ERR_E002, f->fname);
      fprintf(wfp, "\t%s\n", strerror(errno));
      fprintf(wfp, MSG_ERR_E000, prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** success ***/
  return((int) TRUE);

} /* open_out() */

/*
 * close_in() -- Close an input file
 */
void close_in(FILE **in, char *fname)

{
  if (fname == (char *) NULL)
    return;
  if (strcmp(fname, FILE_NAME_STDIN) == 0)
    return;
  
  if ((*in) != (FILE *) NULL)
    {
      fclose((*in));
      (*in) = stdin;
    }

} /* close_in() */

/*
 * close_out() -- close output
 */
void close_out(struct s_file_info *f)

{
  if (f->fname == (char *) NULL)
    return;
  if (strcmp(f->fname, FILE_NAME_STDOUT) == 0)
    return;

  fclose(f->fp);
  free(f->fname);
  f->fname = (char *) NULL;

} /* close_out() */

/*
 * init_finfo() -- initialize out file structure
 */
void init_finfo(struct s_file_info *f)

{

  f->fp    = (FILE *) NULL;
  f->fname = (char *) NULL;

} /* init_finfo() */
