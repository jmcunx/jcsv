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
 * jcsv_i.c -- Initialization Routines
 */
#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "jcsv.h"

#define SCKARG 80

/*** prototypes ***/
void init_w(struct s_work *, char *);
char get_delm(FILE *, char *, char *);
int  get_numb(FILE *, char *, char, char *);
void parse_txt_col(struct s_work *, char *);

/*
 * init() -- initialize
 */
void init(int argc, char **argv, struct s_work *w)

{
  int i;
  char *txt_raw = (char *) NULL;

  init_w(w, argv[0]);

  txt_raw = process_arg(argc, argv, w);

  parse_txt_col(w, txt_raw);

  if (w->verbose >= 2)
    {
      fprintf(w->err.fp, MSG_INFO_I081, w->num_files);
      fprintf(w->err.fp, MSG_INFO_I082, (w->only_ascii ? LIT_YES : LIT_NO));
      fprintf(w->err.fp, MSG_INFO_I083, (w->is_csv     ? LIT_YES : LIT_NO));
      fprintf(w->err.fp, MSG_INFO_I084, (w->to_csv     ? LIT_YES : LIT_NO));
      fprintf(w->err.fp, MSG_INFO_I094, w->decimal);
      fprintf(w->err.fp, MSG_INFO_I085, w->delm);
      fprintf(w->err.fp, MSG_INFO_I086, 
             (w->err.fname == (char *) NULL ? LIT_STDERR : w->err.fname));
      fprintf(w->err.fp, MSG_INFO_I087, (w->force      ? LIT_YES : LIT_NO));
      if (w->max_col_size == 0)
	fprintf(w->err.fp, MSG_INFO_I088, LIT_UNLIMITED);
      else
	fprintf(w->err.fp, MSG_INFO_I089, w->max_col_size);
      fprintf(w->err.fp, MSG_INFO_I090, 
             (w->out.fname == (char *) NULL ? LIT_STDOUT : w->out.fname));
      if (txt_raw != (char *) NULL)
	{
	  fprintf(w->err.fp, MSG_INFO_I091, txt_raw);
	  for (i = 0; w->txt_cols[i] > -1; i++)
	    fprintf(w->err.fp, MSG_INFO_I095, w->txt_cols[i] );
	}
      fprintf(w->err.fp, MSG_INFO_I092, w->verbose);
      fprintf(w->err.fp, MSG_INFO_I093, (w->empty_space ? LIT_YES : LIT_NO));
      fprintf(w->err.fp, "\n");
    }

  if (txt_raw != (char *) NULL)
    free(txt_raw);

}  /* init() */

/*
 * init_w() -- initialize work area
 */
void init_w(struct s_work *w, char *a)

{
  init_finfo(&(w->out));
  init_finfo(&(w->err));
  w->err.fp    = stderr;
  w->out.fp    = stdout;

  w->prog_name        = j2_get_prgname(a, PROG_NAME);
  w->num_files        = 0;
  w->verbose          = 0;
  w->decimal          = '.';
  w->txt_cols         = (int *) NULL;
  w->empty_space      = (int) FALSE;
  w->force            = (int) FALSE;
  w->only_ascii       = (int) FALSE;
  w->is_csv           = (int) TRUE;
  w->to_csv           = (int) FALSE;
  w->delm             = FIELD_DELIM;
  w->max_col_size     = 0;

}  /* init_w() */

/*
 * save_fname() -- Check and Save File Name
 */
void save_fname(struct s_file_info *f, char *afname, char *pname, char arg_val)
{
  if (f->fname == (char *) NULL)
    f->fname = strdup(afname);
  else
    {
      fprintf(stderr, MSG_ERR_E074, SWITCH_CHAR, arg_val);
      fprintf(stderr, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

} /* save_fname() */

/*
 * process_arg() -- process arguments
 */
char *process_arg(int argc, char **argv, struct s_work *w)

{

  char ckarg[SCKARG];
  int opt;
  int i;
  char *txt = (char *) NULL;

  snprintf(ckarg, SCKARG, "%c%c%c%c%c%c%c%c:%c:%c:%c:%c:%c:", 
	  ARG_UTF_8_ASCII, ARG_FMT_CSV, ARG_FORCE, ARG_ADD_SPACE,
	  ARG_HELP, ARG_VERBOSE, ARG_VERSION,
	  ARG_ERR, ARG_OUT, ARG_DELM, ARG_MAX_SIZE, ARG_TEXT, ARG_DECIMAL);

  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	case ARG_ADD_SPACE:
	  w->empty_space = (int) TRUE;
	  break;
	case ARG_UTF_8_ASCII:
	  w->only_ascii = (int) TRUE;
	  break;
	case ARG_FMT_CSV:
	  w->to_csv = (int) TRUE;
	  w->is_csv = (int) FALSE;
	  break;
	case ARG_FORCE:
	  w->force = (int) TRUE;
	  break;
	case ARG_HELP:
	  show_brief_help(stderr, w->prog_name);
	  break;
	case ARG_VERBOSE:
	  w->verbose++;
	  break;
	case ARG_VERSION:
	  show_rev(stderr, w->prog_name);
	  break;
	case ARG_DECIMAL:
	  w->decimal = get_delm(stderr, optarg, LIT_DECIMAL_P);
	  if ((w->decimal != '.') && (w->decimal != '.') )
	    {
	      fprintf(stderr, MSG_ERR_E072, SWITCH_CHAR, ARG_DECIMAL);
	      fprintf(stderr, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);;
	      exit(EXIT_FAILURE);
	    }
	  break;
	case ARG_DELM:
	  w->delm = get_delm(stderr, optarg, w->prog_name);
	  break;
	case ARG_ERR:
	  save_fname(&(w->err), optarg, w->prog_name, ARG_ERR);
	  break;
	case ARG_OUT:
	  save_fname(&(w->out), optarg, w->prog_name, ARG_ERR);
	  break;
	case ARG_MAX_SIZE:
	  w->max_col_size = get_numb(stderr, optarg, ARG_MAX_SIZE, w->prog_name);
	  break;
	case ARG_TEXT:
	  if (txt == (char *) NULL)
	    txt = strdup(optarg);
	  else
	    {
	      fprintf(stderr, MSG_ERR_E078, SWITCH_CHAR, ARG_TEXT);
	      fprintf(stderr, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);;
	      exit(EXIT_FAILURE);
	    }
	  break;
	default:
	  fprintf(stderr, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	  break;
	}
    }

  /*** open 'out' files ***/
  if ( ! open_out(stderr, &(w->err), w->prog_name, w->force))
    w->err.fp = stderr;
  if ( ! open_out(w->err.fp, &(w->out), w->prog_name, w->force) )
    w->out.fp = stdout;

  /*** Count number of files to process */
  for (i = optind; i < argc; i++)
    (w->num_files)++;
  if (w->num_files == 0)
    (w->num_files)++;  /* stdin when no files */

  return(txt);

} /* process_arg() */

/*
 * parse_txt_col() -- Parse Text Columns
 */
void parse_txt_col(struct s_work *w, char *txt)
{
  size_t s, i;
  char *t = (char *) NULL, *tok = (char *) NULL;

  if (txt == (char *) NULL)
    return;

  j2_fix_delm(',', &t, txt); /* this will allow a check for ', ,' */
  j2_justleft(t);
  j2_rtw(t);
  s = strlen(t);

  if (s < 0)
    {
      fprintf(w->err.fp, MSG_ERR_E006, txt, SWITCH_CHAR, ARG_TEXT);
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }
  if ((*t) == ',')
    {
      fprintf(w->err.fp, MSG_ERR_E006, txt, SWITCH_CHAR, ARG_TEXT);
      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  /*** first pass to validate string, n ***/
  for (tok = t; *tok != JLIB2_CHAR_NULL; tok++)
    {
      if ((*tok) != ',')
	{
	  if (! isdigit((*tok)) )
	    {
	      fprintf(w->err.fp, MSG_ERR_E006, txt, SWITCH_CHAR, ARG_TEXT);
	      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
    }

  /*** save data ***/
  s = strlen(t);
  s += 5;
  w->txt_cols = calloc(s, sizeof(int));

  for (i = 0; i < s; i++)
    w->txt_cols[i] = -1;

  s = 0;
  tok = strtok(t, ",");
  while (tok != (char *) NULL)
    {
      if (j2_is_numr(tok))
	{
	  w->txt_cols[s] = atoi(tok);
	  if (w->txt_cols[s] < 0)
	    {
	      fprintf(w->err.fp, MSG_ERR_E006, txt, SWITCH_CHAR, ARG_TEXT);
	      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	  s++;
	}
      else
	{
	  fprintf(w->err.fp, MSG_ERR_E006, txt, SWITCH_CHAR, ARG_TEXT);
	  fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
      tok = strtok((char *) NULL, ",");
    }

  /*** done ***/
  if (t != (char *) NULL)
    free(t);

} /* parse_txt_col() */

/*
 * get_delm() -- translate a number or string into a delimiter
 */
char get_delm(FILE *fp, char *s, char *pname)
{
  int d;
  int c = JLIB2_CHAR_NULL;

  if (strlen(s) == 1)
    {
      if ( ((*s) > 47)  && ((*s) < 58) ) /* 0 -- 9 */
	c = (*s) - 48;
      else
	c = (*s);
    }
  else
    {
      if (j2_is_numr(s) == (int) TRUE)
	{
	  d = atoi(s);
	  if ((d < 256) && (d > 0))
	    c = (char) d;
	  else
	    {
	      fprintf(fp, MSG_ERR_E006, s, SWITCH_CHAR, ARG_DELM);
	      fprintf(fp, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  fprintf(fp, MSG_ERR_E006, s, SWITCH_CHAR, ARG_DELM);
	  fprintf(fp, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  return(c);

} /* get_delm() */

/*
 * get_numb() -- check and convert arg to numeric
 */
int get_numb(FILE *fp, char *s, char arg_char, char *pname)
{
  
  if (! j2_is_numr(s))
    {
      fprintf(fp, MSG_ERR_E006, s, SWITCH_CHAR, arg_char);
      fprintf(fp, MSG_ERR_E000, pname,  SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

  return(atoi(optarg));
  
} /* get_numb() */
