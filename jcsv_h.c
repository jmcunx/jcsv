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
 * jcsv_h.c -- Show hel and prev routines
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "jcsv.h"

/*** Messages ***/
#define MSG_HELP_11  "Convert Text Files from/to csv"

/*
 * show_brief_help()
 */
void show_brief_help(FILE *fp, char *pname)

{

  fprintf(fp, USG_MSG_USAGE, pname);
  fprintf(fp, "\t%s\n", MSG_HELP_11);
  fprintf(fp, USG_MSG_OPTIONS);

  fprintf(fp, USG_MSG_ARG_UTF_8_ASCII_1,  SWITCH_CHAR, ARG_UTF_8_ASCII);
  fprintf(fp, USG_MSG_ARG_FMT_CSV,        SWITCH_CHAR, ARG_FMT_CSV);
  fprintf(fp, USG_MSG_ARG_DECIMAL_P,      SWITCH_CHAR, ARG_DECIMAL);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_DECIMAL_P1);
  fprintf(fp, USG_MSG_ARG_DELM,           SWITCH_CHAR, ARG_DELM);
  fprintf(fp, USG_MSG_ARG_DELM_A);
  fprintf(fp, USG_MSG_ARG_ERR,            SWITCH_CHAR, ARG_ERR);
  fprintf(fp, USG_MSG_ARG_FORCE,          SWITCH_CHAR, ARG_FORCE);
  fprintf(fp, USG_MSG_ARG_HELP,           SWITCH_CHAR, ARG_HELP);
  fprintf(fp, USG_MSG_ARG_MAX_SIZE_1,     SWITCH_CHAR, ARG_MAX_SIZE);
  fprintf(fp, USG_MSG_ARG_OUT,            SWITCH_CHAR, ARG_OUT);
  fprintf(fp, USG_MSG_ARG_TEXT_COL,       SWITCH_CHAR, ARG_TEXT);
  fprintf(fp, USG_MSG_ARG_VERSION,        SWITCH_CHAR, ARG_VERSION);
  fprintf(fp, USG_MSG_ARG_VERBOSE_8,      SWITCH_CHAR, ARG_VERBOSE);
  fprintf(fp, USG_MSG_ARG_ADD_SPACE,      SWITCH_CHAR, ARG_ADD_SPACE);

  exit(EXIT_FAILURE);

}  /* show_brief_help() */

/*
 * show_rev()
 */
void show_rev(FILE *fp, char *pname)

{

  fprintf(fp,"%s %s:\n", pname, LIT_REV);
#ifdef J_LIB2_H
  fprintf(fp,"\t%s %s\n", LIT_INFO_02, j2_get_build());
#endif

#ifdef OSTYPE
  fprintf(fp,"\t%s\n",OSTYPE);
#endif  /* OSTYPE  */
#ifdef PRETTY_NAME
  fprintf(fp,"\t%s\n",PRETTY_NAME);
#endif  /* OSTYPE  */
  fprintf(fp, LIT_INFO_01, __DATE__, __TIME__);

  exit(EXIT_FAILURE);

}  /* show_rev() */
