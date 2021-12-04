/*
 * Copyright (c) 2014 2015 2016 ... 2019 2020
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

#ifndef JCSV_H

#define JCSV_H "$Id: jcsv.h,v 2.2 2020/06/14 17:40:58 jmccue Exp $"

#define PROG_NAME  "jcsv"
#define FIELD_DELIM     ','
#define CHAR_DQUOTE     '"'
#define CHAR_SQUOTE     '\''
#define CHAR_COMMA      ','
#define CHAR_PERIOD     '.'
#define CHAR_SPACE      ' '
#define CHAR_DASH       '-'
#define OVERRIDE_COMMA  0x01
#define OVERRIDE_DQUOTE 0x02

/*** structures ***/
struct s_cols
{
  long int num_cols;
  int  *coltype;     /* one value per col, one of COL_TYPE* */
  char **cols;
};

struct s_file_info
{
  FILE *fp;
  char *fname;
} ;

struct s_work
{
  struct s_file_info out;         /* default stdout        */
  struct s_file_info err;         /* default stderr        */
  char *prog_name;                /* real program name     */
  char decimal;                   /* Decimal Point         */
  int  *txt_cols;                 /* formatted text list   */
  int num_files;                  /* # of files to process */
  int verbose;                    /* 0 -- ??               */
  int only_ascii;                 /* TRUE or FALSE         */
  int max_col_size;
  int is_csv;                     /* TRUE or FALSE         */
  int to_csv;                     /* TRUE or FALSE         */
  int force;                      /* TRUE or FALSE         */
  int empty_space;                /* TRUE or FALSE         */
  char delm;                      /* field delimiter       */
} ;

/*** more defines ***/
#define COL_TYPE_UNKNOWN         0
#define COL_TYPE_CHAR            1
#define COL_TYPE_NUMBER          2
#define COL_TYPE_TIME            3 
#define COL_TYPE_EMPTY           4
#define COL_TYPE_DATE_YYYYMMDD   5
#define COL_TYPE_DATE_MMDDYYYY   6
#define COL_TYPE_DATE_DDMMYYYY   7
#define COL_TYPE_DATE_UNKNOWN    8
#define COL_TYPE_BAD            99

#define LIT_COL_UNKNOWN         "Column type not set"
#define LIT_COL_CHAR            "Character String"
#define LIT_COL_NUMBER          "Number"
#define LIT_COL_TIME            "Time"
#define LIT_COL_EMPTY           "Empty, no value"
#define LIT_COL_BAD             "Unknown Type"
#define LIT_COL_DATE_DDMMYYYY   "Date, DD/MM/YYYY"
#define LIT_COL_DATE_MMDDYYYY   "Date, MM/DD/YYYY"
#define LIT_COL_DATE_YYYYMMDD   "Date, YYYY/MM/DD"
#define LIT_COL_DATE_UNK        "Date, but format unknown"

/*** prototypes ***/
void init(int, char **, struct s_work *);
void init_finfo(struct s_file_info *);
void show_brief_help(FILE *, char *);
void show_rev(FILE *, char *);
char *process_arg(int, char **, struct s_work *);
int  open_out(FILE *, struct s_file_info *, char *, int);
void close_out(struct s_file_info *);
int  open_in(FILE **, char *, FILE *);
void close_in(FILE **, char *);

void cols_split(struct s_cols *, char *, char, struct s_work *w);
void cols_free(struct s_cols *);
void cols_show(FILE *, struct s_cols *, long int);

#endif /*  JCSV_H  */

/* END: jcsv.h */
