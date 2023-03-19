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

#ifndef JCSV_H

#define JCSV_H "INCLUDED"

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

#ifndef JLIB2_CHAR_NULL
#define NO_JLIB 1 /* fuure use */
#define JLIB2_CHAR_NULL  ((char) '\0')
#endif
#ifndef NULL
#define NULL '\0'
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifdef _MSDOS
#define SNPRINTF _snprintf
#define SSIZE_T unsigned long int
#endif
#ifndef SSIZE_T
#define SSIZE_T ssize_t
#endif

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

#ifdef NO_JLIB
#define ARG_ADD_SPACE     'w'  /* Add Spaces                         */
#define ARG_DECIMAL       'D'  /* Decimal Mode                       */
#define ARG_DELM          'd'  /* Field Delimiter                    */
#define ARG_DELM_OUT      'D'  /* Output Field Delimiter             */
#define ARG_ERR           'e'  /* Output Error File                  */
#define ARG_FMT_CSV       'c'  /* Format output as CSV               */
#define ARG_FORCE         'f'  /* force create files                 */
#define ARG_HELP          'h'  /* Show Help                          */
#define ARG_MAX_SIZE      'M'  /* Maximum Size                       */
#define ARG_OUT           'o'  /* Output File                        */
#define ARG_TEXT          't'  /* Process text mode                  */
#define ARG_UTF_8_ASCII   'a'  /* Convert UTF-8 to ASCII             */
#define ARG_VERBOSE       'v'  /* Verbose                            */
#define ARG_VERSION       'V'  /* Show Version Information           */
#define FILE_NAME_STDIN                  "-"
#define FILE_NAME_STDOUT                 "-"
#define LIT_C80                          "::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
#define LIT_DECIMAL_P                    "decimal point"
#define LIT_INFO_01                      "\tBuild: %s %s\n"
#define LIT_NO                           "No"
#define LIT_REV                          "Revision"
#define LIT_STDERR                       "(standard error)"
#define LIT_STDIN                        "(standard input)"
#define LIT_STDOUT                       "(standard output)"
#define LIT_UNLIMITED                    "unlimited"
#define LIT_YES                          "Yes"
#define MSG_ERR_E000                     "Try '%s %c%c' for more information\n"
#define MSG_ERR_E002                     "ERROR E002: Cannot open '%s' for write, processing aborted\n"
#define MSG_ERR_E006                     "ERROR E006: '%s' is an invalid value for %c%c\n"
#define MSG_ERR_E025                     "ERROR E025: File %s cannot be created, already exists\n"
#define MSG_ERR_E072                     "ERROR E072: %c%c invalid, can only specify a period or a comma\n"
#define MSG_ERR_E074                     "ERROR E074: 'Too many Arguments specified for %c%c\n"
#define MSG_ERR_E078                     "ERROR E078: Argument %c%c cannot only be specified once\n"
#define MSG_ERR_E080                     "ERROR E080: cannot allocate initial memory : %s\n"
#define MSG_INFO_I053                    "I053: Column Data not found for rec %ld\n"
#define MSG_INFO_I054                    "I054: Column Data for record  %ld, total columns: %ld\n"
#define MSG_INFO_I055                    "I055:     Column   %3ld: %s\n"
#define MSG_INFO_I056                    "I056: Field Type         %s\n"
#define MSG_INFO_I072                    "I072:    Lines Reads:  %9ld - File %s\n"
#define MSG_INFO_I080                    "I080:   Lines Writes:  %9ld - File %s\n"
#define MSG_INFO_I081                    "I081: Number of Input Files          %d\n"
#define MSG_INFO_I082                    "I082: Remove non-ASCII characters  ? %s\n"
#define MSG_INFO_I083                    "I083: Input  Files in csv format   ? %s\n"
#define MSG_INFO_I084                    "I084: Output File  in csv format   ? %s\n"
#define MSG_INFO_I085                    "I085: Non-csv Field Delimiter        '%c'\n"
#define MSG_INFO_I086                    "I086: Errors written to file       ? %s\n"
#define MSG_INFO_I087                    "I087: Force Create Output          ? %s\n"
#define MSG_INFO_I088                    "I088: Maximum Field Size             %s\n"
#define MSG_INFO_I089                    "I089: Maximum Field Size             %d\n"
#define MSG_INFO_I090                    "I090: Output written to file       ? %s\n"
#define MSG_INFO_I091                    "I091: Force these columns as Text    %s\n"
#define MSG_INFO_I092                    "I092: Verbose Level                  %d\n"
#define MSG_INFO_I093                    "I093: Print Space on Empty Columns ? %s\n"
#define MSG_INFO_I094                    "I094: Decimal Point is a             '%c'\n"
#define MSG_INFO_I095                    "I095:             column  as Text    %d\n"
#define MSG_WARN_W002                    "W002: Open Error Bypass File '%s' : %s\n"
#define SWITCH_CHAR                      '-'
#define USG_MSG_ARG_ADD_SPACE            "\t%c%c\t\t: write a space instead of an empty value\n"
#define USG_MSG_ARG_DECIMAL_P            "\t%c%c c\t\t: Decimal Point for Numbers, actual character OR\n"
#define USG_MSG_ARG_DECIMAL_P1           "\t\t\t  Ex: 46 = '.' or 44 = ',' or just '.' or ','\n"
#define USG_MSG_ARG_DELM                 "\t%c%c c\t\t: Field delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_A               "\t\t\t  the decimal # representing the character.\n"
#define USG_MSG_ARG_ERR                  "\t%c%c file\t\t: Write errors to file 'file', default stderr\n"
#define USG_MSG_ARG_FMT_CSV              "\t%c%c\t\t: Format output as a csv file\n"
#define USG_MSG_ARG_FORCE                "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_HELP                 "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_MAX_SIZE_1           "\t%c%c n\t\t: Maximum Field Size, default no maximum\n"
#define USG_MSG_ARG_OUT                  "\t%c%c file\t\t: Write output to file 'file', default stdout\n"
#define USG_MSG_ARG_TEXT_COL             "\t%c%c n,n,n,n,...\t: Treat columns 'n' as text (0 = all Text)\n"
#define USG_MSG_ARG_UTF_8_ASCII_1        "\t%c%c\t\t: Remove all non-ASCII Characters\n"
#define USG_MSG_ARG_VERBOSE_8            "\t%c%c\t\t: verbose level, each time specified level increases\n"
#define USG_MSG_ARG_VERSION              "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_OPTIONS                  "Options\n"
#define USG_MSG_USAGE                    "usage:\t%s [OPTIONS] [FILES ...]\n"
#endif /* NO_JLIB */

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

#ifdef NO_JLIB
int j2_bye_char(char *buffer, char c);
long int j2_chg_char(char old, char new, char *s, SSIZE_T force_size);
int j2_f_exist(char *file_name);
long int j2_fix_delm(char delm, char **fixed, char *buf);
char *j2_get_prgname(char *argv_0, char *default_name);
int j2_is_numr(char *s);
long int j2_justleft(char *s);
long int j2_rtw(char *buffer);
#endif

#endif /*  JCSV_H  */
