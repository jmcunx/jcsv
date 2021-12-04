/*
 * Copyright (c) 2014 2015 2016 ... 2021 2022
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
 * jcsv.c -- main routines
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifdef IS_BSD
#include <err.h>
#endif

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jcsv.h"

/*** prototypes ***/
void process_all(int, char **, struct s_work *);
long int process_a_file(struct s_work *, char *, char **, size_t *);
void embedded_csv(char *);
int open_csv_quote(char *, size_t *);
int close_csv_quote(char *, size_t);
void only_ascii(char *, int);
void fix_cols(struct s_cols *, char, int, int, int, char);
void fix_col_string(char *, char, int, int, int);
void fix_number(char *, char);
void show_file_heading(struct s_work *, char *);
void write_data(struct s_cols *, struct s_work *);
void write_a_ccol(char *, struct s_work *);

/*
 * main()
 */
int main(int argc, char **argv)

{
  struct s_work w;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath",NULL) == -1)
    err(1,"pledge\n");
#endif
  
  init(argc, argv, &w);
  
  process_all(argc, argv, &w);
  
  close_out(&(w.out));
  close_out(&(w.err));
  exit(EXIT_SUCCESS);

}  /* main() */

/*
 * process_all() -- Process all files
 */
void process_all(int argc, char **argv, struct s_work *w)

{
  long int lines_written = 0L;
  char *buf = (char *) NULL;
  size_t bsiz = (size_t) 0;
  int i;
  
  /* allocate initial read buffer memory (optional) */
  if (buf == (char *) NULL)
    {
      bsiz = 512;
      buf = (char *) calloc(bsiz, sizeof(char));
      if (buf == (char *) NULL)
	{
	  fprintf(w->err.fp, MSG_ERR_E080, strerror(errno));
	  return;
	}
    }

  /* process data */
  for (i = optind; i < argc; i++)
    lines_written += process_a_file(w, argv[i], &buf, &bsiz);

  if (i == optind)
    lines_written += process_a_file(w, FILE_NAME_STDIN, &buf, &bsiz);

  if (buf != (char *) NULL)
    free(buf);

  if (w->verbose >= 1)
    {
      fprintf(w->err.fp, MSG_INFO_I080, lines_written,
	     (w->err.fname == (char *) NULL ? LIT_STDOUT : w->err.fname));
    }

}  /* process_all() */

/*
 * process_a_file()
 */
long int process_a_file(struct s_work *w, char *fname, 
                        char **buf,       size_t *bsiz)

{

  long int lines_read = 0L;
  long int lines_w    = 0L;
  FILE *fp;
  struct s_cols c;

  if ((w->verbose >= 2) && (w->num_files > 1))
    show_file_heading(w, fname);
  
  if ( ! open_in(&fp, fname, w->err.fp) )
    return(0L);

  /*** process data ***/
  while (getline(buf, bsiz, fp) > (ssize_t) -1)
    {
      lines_read++;
      j2_rtw((*buf));
      if (w->only_ascii == (int) TRUE)
	{
	  if (w->is_csv == (int) TRUE)
	    only_ascii((*buf), CHAR_COMMA);
	  else
	    only_ascii((*buf), w->delm);
	}
      if (w->is_csv == (int) TRUE)
	embedded_csv((*buf));
      if (w->is_csv == (int) TRUE)
	cols_split(&c, (*buf), CHAR_COMMA, w);
      else
	cols_split(&c, (*buf), w->delm, w);
      fix_cols(&c, w->delm, w->is_csv, w->to_csv, w->max_col_size, w->decimal);
      if (w->verbose >= 3)
	cols_show(w->err.fp, &c,  lines_read);
      lines_w++;
      write_data(&c, w);
      cols_free(&c);
    }

  /*** complete ***/
  close_in(&fp, fname);
  
  if (w->verbose >= 1)
    {
      if (w->verbose >= 2)
	fprintf(w->err.fp, "\n");
      fprintf(w->err.fp, MSG_INFO_I072, lines_read, fname);
    }

  return(lines_w);

} /* process_a_file() */

/*
 * write_a_ccol() -- write a character column
 */
void write_a_ccol(char *s, struct s_work *w)
{

  char *fmt = s;

  if (w->empty_space == (int) TRUE)
    {
      if (strlen(s) < 1)
	fmt = " ";
    }

  if (w->to_csv)
    fprintf(w->out.fp, "%c%s%c",
	    CHAR_DQUOTE, fmt, CHAR_DQUOTE);
  else
    fprintf(w->out.fp, "%s", fmt);

} /* write_a_ccol() */

/*
 * write_a_ncol() -- write a non-character column
 */
void write_a_ncol(char *fmt, struct s_work *w, int lead_zero)
{

  double n;
  size_t sz = strlen(fmt);

  if (sz < 1)
    {
      if (w->empty_space == TRUE)
	fprintf(w->out.fp, " ");
      return;
    }

  if ((lead_zero == FALSE) || (sz < 2))
    {
      fprintf(w->out.fp, "%s", fmt);
      return;
    }

  n = atof(fmt);
  if (n >= (double) 0)
    {
      if (! isdigit(*fmt))
	fprintf(w->out.fp, "0%s", fmt);
      else
	fprintf(w->out.fp, "%s", fmt);
      return;
    }

  /* fix negative numbers */
  if (n <= (double) -1)
    {
      fprintf(w->out.fp, "%s", fmt);
      return;
    }

  if (isdigit(fmt[1]))
    fprintf(w->out.fp, "-");
  else
    fprintf(w->out.fp, "-0");

  fprintf(w->out.fp, "%s", &(fmt[1]));


} /* write_a_ncol() */

/*
 * write_data() -- correct columns
 */
void write_data(struct s_cols *c, struct s_work *w)
{

  long int i;
  char use_delm;

  if (c->cols == (char **) NULL)
    {
      fprintf(w->out.fp, "\n");
      return;
    }

  if (w->to_csv)
    use_delm = CHAR_COMMA;
  else
    use_delm = w->delm;

  for (i = 0L; c->cols[i] != (char *) NULL; i++)
    {	      
      if (i > 0L)
	fprintf(w->out.fp, "%c", use_delm);
      switch (c->coltype[i])
	{
	  case COL_TYPE_NUMBER:
	    write_a_ncol(c->cols[i], w, TRUE);
	    break;
	  case COL_TYPE_TIME:
	  case COL_TYPE_DATE_YYYYMMDD:
	  case COL_TYPE_DATE_MMDDYYYY:
	  case COL_TYPE_DATE_DDMMYYYY:
	  case COL_TYPE_DATE_UNKNOWN:
	    write_a_ncol(c->cols[i], w, FALSE);
	    break;
	  default:
	    /*** assume character ***/
	    write_a_ccol(c->cols[i], w);
	    break;
	}
      if (i >= c->num_cols)
	break;
    }

  fprintf(w->out.fp, "\n");
  
} /* write_data() */

/*
 * fix_cols() -- correct columns
 */
void fix_cols(struct s_cols *c, char delm, 
              int is_csv, int to_csv, int max_col_size, 
              char decimal_point)
{

  long int i;

  if (c->cols == (char **) NULL)
    return;

  for (i = 0L; c->cols[i] != (char *) NULL; i++)
    {	      
      if (c->coltype[i] == COL_TYPE_NUMBER)
	fix_number(c->cols[i], decimal_point);
      fix_col_string(c->cols[i], delm, is_csv, to_csv, max_col_size);
      if (i >= c->num_cols)
	break;
    }

  
} /* fix_cols() */

/*
 * fix_number() -- convert number, will ensure:
 *                     1.  decimal will be a period
 *                     2.  negative sign (if name) starts the string
 *                     3.  thousand separators are removed
 */
void fix_number(char *b, char decimal)
{

  char comma;
  char period;
  int  removed;
  size_t size_b;
  char *s;

  size_b = strlen(b) + 1; /* for NULL */

  if (decimal == CHAR_PERIOD)
    {
      comma  = CHAR_COMMA;
      period = CHAR_PERIOD;
    }
  else
    {
      comma  = CHAR_PERIOD;
      period = CHAR_COMMA;
    }

  j2_bye_char(b, comma);

  if (period != CHAR_PERIOD)
    j2_chg_char(period, CHAR_PERIOD, b, 0);

  removed = j2_bye_char(b, '-');

  if (removed != 1)
    return;

  s = calloc(size_b, sizeof(char));

#ifdef OpenBSD
  strlcpy(s, b, size_b);
#else
  strcpy(s,b);
#endif
  snprintf(b, size_b, "-%s", s);

  free(s);

} /* fix_number() */

/*
 * fix_col_string() -- correct override characters,
 *                     remove trailing/leading quote
 */
void fix_col_string(char *b, char delm, 
                    int is_csv, int to_csv, 
                    int max_col_size)
{
  size_t i;
  char *s = (char *) NULL;

  if (b == (char *) NULL)
    return;
  if (strlen(b) < 1)
    return;

  /* remove leading/trailing quote */
  if ((*b) == CHAR_DQUOTE)
    s = strdup(&(b[1]));
  else
    s = strdup(b);
  i = strlen(s) - 1;
  if (s[i] == CHAR_DQUOTE)
    s[i] = JLIB2_CHAR_NULL;

  /*** generate new string ***/
  memset(b, JLIB2_CHAR_NULL, strlen(b));

  if (to_csv)
    {
      j2_chg_char(OVERRIDE_COMMA,  CHAR_SPACE,  s, 0);
      j2_chg_char(OVERRIDE_DQUOTE, CHAR_SQUOTE, s, 0);
      j2_chg_char(CHAR_COMMA,      CHAR_SPACE,  s, 0);
      j2_chg_char(CHAR_DQUOTE,     CHAR_SQUOTE, s, 0);
    }
  else
    {
      j2_chg_char(OVERRIDE_COMMA,  CHAR_COMMA,  s, 0);
      j2_chg_char(OVERRIDE_DQUOTE, CHAR_DQUOTE, s, 0);
      j2_chg_char(delm,            CHAR_SPACE,  s, 0);
    }

  if (max_col_size > 0)
    {
      if (strlen(s) > max_col_size)
	s[max_col_size] = JLIB2_CHAR_NULL;
    }

#ifdef OpenBSD
  i = strlen(s) + 5;
  strlcpy(b, s, i);
#else
  strcpy(b, s);
#endif

  if (s != (char *) NULL)
    free(s);
  
} /* fix_col_string() */

/*
 * only_ascii() -- remove all non-ASCII characters
 */
void only_ascii(char *b, int delm)
{
  char *s = (char *) NULL;
  int i, j;

  if (b == (char *) NULL)
    return;
  if (strlen(b) < 1)
    return;

  s = strdup(b);
  memset(b, JLIB2_CHAR_NULL, strlen(b));

  for (i = 0, j = 0; s[i] != JLIB2_CHAR_NULL; i++)
    {
      if ( (s[i] == delm) || ((s[i] > 31) && (s[i] < 127)) )
	{
	  b[j] = s[i];
	  j++;
	}
    }

  if (s != (char *) NULL)
    free(s);
  
} /* only_ascii() */

/*
 * embedded_csv() -- For use when converting FROM a csv file
 *
 *                   temporally change embedded commas/double
 *                   quote to another character
 */
void embedded_csv(char *b)
{
  int quote_open = (int) FALSE;
  size_t i;
    
  if (b == (char *) NULL)
    return;
  if (strlen(b) < 5)
    return;
  
  if ((*b) == CHAR_DQUOTE)
    quote_open = (int) TRUE;

  for (i = 1; b[i] != JLIB2_CHAR_NULL; i++)
    {
      if (quote_open)
	quote_open = open_csv_quote(b, &i);
      else
	quote_open = close_csv_quote(b, i);
    }
  
} /* embedded_csv() */

/*
 * open_csv_quote() -- examine data when we are INSIDE " strings
 */
int open_csv_quote(char *b, size_t *i)
{
  size_t ni = (*i) + 1;
  
  if (b[(*i)] == CHAR_COMMA)
    {
      b[(*i)] = OVERRIDE_COMMA;
      return((int) TRUE);
    }

  if (b[(*i)] == CHAR_DQUOTE)
    {
      if (b[ni] == CHAR_COMMA)
	{
	  (*i)++;
	  return((int) FALSE);
	}
      if (b[ni] == JLIB2_CHAR_NULL)
	return((int) FALSE);
      b[(*i)] = OVERRIDE_DQUOTE;
      return((int) TRUE);
    }

  return((int) TRUE);

} /* open_csv_quote() */

/*
 * close_csv_quote() -- examine data when we are OUTSIDE " strings
 */
int close_csv_quote(char *b, size_t i)
{
  
  size_t pi = i - 1; 

  if (b[i] == CHAR_COMMA)
    return((int) FALSE);
  
  if (b[i] == CHAR_DQUOTE)
    {
      if (b[pi] == CHAR_COMMA)
	return((int) TRUE);
    }

  if (b[i] == CHAR_DQUOTE)
    b[i] = OVERRIDE_DQUOTE;

  return((int) FALSE);

} /* _csv_quote() */

/*
 * show_file_heading() -- Show run stats
 */
void show_file_heading(struct s_work *w, char *fname)

{

  fprintf(w->err.fp, "%s\n", LIT_C80);

  if (fname == (char *) NULL)
    fprintf(w->err.fp, "%s\n", LIT_STDIN);
  else
    {
      if (strcmp(fname, FILE_NAME_STDIN) == 0)
	fprintf(w->err.fp, "%s\n", LIT_STDIN);
      else
	fprintf(w->err.fp, "%s\n", fname);
    }
  
  fprintf(w->err.fp, "%s\n", LIT_C80);

} /* show_file_heading() */
