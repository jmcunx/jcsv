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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jcsv.h"

/*
 * cols_init() -- split record into columns
 */
void cols_init(struct s_cols *c)
{

  c->num_cols = 0L;
  c->coltype  = (int *) NULL;
  c->cols     = (char **) NULL;

} /* cols_init() */

/*
 * col_date_sep() -- data element separator makes sense ?
 */
int col_date_sep(char *b)
{
  int count_slash  = 0;
  int count_dash   = 0;
  char *c;
  
  for (c = b; (*c) != JLIB2_CHAR_NULL; c++)
    {
      if ((*c) == '/')
	{
	  count_slash++;
	  continue;
	}
      if ((*c) == '-')
	{
	  count_dash++;
	  continue;
	}
      if (! isdigit((*c)))
	return(0);
	
    }

  if ((count_dash == 2) && (count_slash != 0))
    return((int) 0);
  if ((count_dash != 0) && (count_slash == 2))
    return((int) 0);
  if (count_dash != 2)
    {
      if (count_slash != 2)
	return((int) 0);
    }
    
  if ((b[4] == '/') && (b[7] == '/'))
    return((int) 1);
  if ((b[4] == '-') && (b[7] == '-'))
    return((int) 1);

  if ((b[2] == '/') && (b[5] == '/'))
    return((int) 2);
  if ((b[2] == '-') && (b[5] == '-'))
    return((int) 2);

  return((int) 0);
  
} /* col_date_sep() */

/*
 * col_date() -- is this a date ? try and find format
 */
int col_date(char *b)
{
  char s[3];
  int date_type = 0;
  int p1, p2;
    
  memset(s, JLIB2_CHAR_NULL, 3);

  if (b == (char *) NULL)
    return(COL_TYPE_UNKNOWN);
  if (strlen(b) != 10)
    return(COL_TYPE_UNKNOWN);

  date_type = col_date_sep(b);
  if (date_type == 0)
    return(COL_TYPE_UNKNOWN); 

  /*** real good chance we have a date ***/

  /* YYYY/MM/DD ? */
  if (date_type == 1)
    {
      s[0] = b[5];
      s[1] = b[6];
      p1 = atoi(s);
      s[0] = b[8];
      s[1] = b[9];
      p2 = atoi(s);
      if ((p1 < 1) || (p2 < 1))
	return(COL_TYPE_UNKNOWN);
      if (p1 > 12)
	return(COL_TYPE_UNKNOWN);
      if (p1 > 31)
	return(COL_TYPE_UNKNOWN);
      return(COL_TYPE_DATE_YYYYMMDD);
    }

  /* seem to be somelind of ??/??/YYYY, guess time */
  s[0] = b[0];
  s[1] = b[1];
  p1 = atoi(s);
  s[0] = b[3];
  s[1] = b[4];
  p2 = atoi(s);

  if ((p1 < 1) || (p2 < 1))
    return(COL_TYPE_UNKNOWN);
  if ((p1 > 12) && (p2 > 12))
    return(COL_TYPE_UNKNOWN);
  if ((p1 > 12) && (p2 < 13))
    return(COL_TYPE_DATE_DDMMYYYY);
  if ((p1 < 13) && (p2 > 12))
    return(COL_TYPE_DATE_MMDDYYYY);
  if ((p1 < 13) && (p2 < 13))
    return(COL_TYPE_DATE_UNKNOWN);
  
  return(COL_TYPE_UNKNOWN);
  
} /* col_date() */

/*
 * col_type() -- determine column type
 */
int col_type(char *b, int colnum, struct s_work *w)
{

  char *c = b;
  size_t slen      = (size_t) 0;
  int count_period = 0;
  int count_comma  = 0;
  int count_dash   = 0;
  int count_slash  = 0;
  int count_colon  = 0;
  int count_digit  = 0;
  int i;
  int date_type;
  char real_comma;
  char real_period;
  int  comma_period_ok = (int) TRUE;
  
  if (b == (char *) NULL)
    return(COL_TYPE_EMPTY);

  if (w->decimal == CHAR_PERIOD)
    {
      real_comma  = CHAR_COMMA;
      real_period = CHAR_PERIOD;
    }
  else
    {
      real_comma  = CHAR_PERIOD;
      real_period = CHAR_COMMA;
    }

  /*** first see if we are forcing text ***/
  if (w->txt_cols != (int *) NULL)
    {
      for ( i = 0; w->txt_cols[i] > -1; i++)
	{
	  if (w->txt_cols[i] == 0)
	    return(COL_TYPE_CHAR);
	  if (w->txt_cols[i] == (colnum + 1))
	    return(COL_TYPE_CHAR);
	}
    }

  /*** figure it out ***/
  slen = strlen(b);
  if (slen < (size_t) 1)
    return(COL_TYPE_EMPTY);

  if (((*b) != CHAR_DASH) && ((*b) != real_period)) /* a simple quick check for char strings */
    {
      if (! isdigit((*b)) )
	return(COL_TYPE_CHAR);
    }

  /*** this a date ?  Complex logic, do first ***/
  date_type = col_date(b);
  if (date_type != COL_TYPE_UNKNOWN)
      return(date_type);

  /*** character ?  probably most common ***/
  for ( c = b; (*c) != JLIB2_CHAR_NULL; c++)
    {
      if ((*c) == real_period)
	{
	  count_period++;
	  continue;
	}
      if ((*c) == real_comma)
	{
	  if (count_period != 0)
	    comma_period_ok = (int) FALSE;
	  count_comma++;
	  continue;
	}
      if ((*c) == '-')
	{
	  count_dash++;
	  continue;
	}
      if ((*c) == '/')
	{
	  count_slash++;
	  continue;
	}
      if ((*c) == ':')
	{
	  count_colon++;
	  continue;
	}
      if ( ((*c) > 47) && ((*c) < 58) )
	{
	  count_digit++;
	  continue;
	}
      return(COL_TYPE_CHAR);
    }

  /*** do we have a number ? ***/
  if ((count_period < 2) && (count_dash < 2) && comma_period_ok)
    {
      if ( (count_colon == 0) && (count_slash == 0) )
	return(COL_TYPE_NUMBER);
    }
  
  /*** a time field ? ***/
  if (count_colon == 2)
    {
      if (slen != 8)
	return(COL_TYPE_CHAR);
      if ( (b[2] == ':') && (b[5] == ':') )
	return(COL_TYPE_TIME);
    }
  else
    {
      if (count_colon != 0)
	return(COL_TYPE_CHAR);
    }
  
  /*** do not know what this is ***/
  return(COL_TYPE_BAD);
  
} /* col_type() */

/*
 * cols_split() -- split record into columns
 */
void cols_split(struct s_cols *c, char *buf, char delim, struct s_work *w)
{
  char *b, *s;
  char  d[2];
  long int count_d;

  cols_init(c);

  if (buf == (char *) NULL)
    return;
  if (strlen(buf) < 1)
    return;
  d[0] = delim;
  d[1] = JLIB2_CHAR_NULL;

  /*** count delimiters ***/
  for (count_d = 0L, b = buf; (*b) != JLIB2_CHAR_NULL; b++)
    {
      if ((*b) == delim)
	count_d++;
    }
  count_d += 3L;

  /*** allocate memory for columns ***/
  c->cols     = (char **) calloc(count_d, sizeof(char *));
  c->coltype  = (int *) calloc(count_d, sizeof(int));

  /*** fix delimeters for strtok() ***/
  b = (char *) NULL;
  j2_fix_delm(delim, &b, buf);

  /*** load cols ***/
  s = strtok(b, d);
  while (s != (char *) NULL)
    {
      j2_justleft(s);
      j2_rtw(s);
      c->cols[c->num_cols]    = strdup(s);
      c->coltype[c->num_cols] = col_type(s, c->num_cols, w);
      (c->num_cols)++;
      s = strtok((char *) NULL, d);
    }
  
  /*** DONE ***/
  if (b != (char *) NULL)
    free(b);
 return;

} /* cols_split() */

/*
 * cols_free() -- Free memory for column data
 */
void cols_free(struct s_cols *c)
{
  long int i;

  if (c->cols != (char **) NULL)
    {
      for (i = 0L; i < c->num_cols; i++)
	{
	  if (c->cols[i] != (char *) NULL)
	    free(c->cols[i]);
	}
      if (c->coltype != (int *) NULL)
	free(c->coltype);
      free(c->cols);
    }

  cols_init(c);

} /* cols_free() */

/*
 * cols_show() -- Show Columns
 */
void cols_show(FILE *fp, struct s_cols *c, long int rec_num)
{
  long int i;

  if (c->cols == (char **) NULL)
    {
      fprintf(fp, MSG_INFO_I053, rec_num);
      return;
    }

  fprintf(fp, "\n");
  fprintf(fp, MSG_INFO_I054, rec_num, c->num_cols);

  for (i = 0L; c->cols[i] != (char *) NULL; i++)
    {
      fprintf(fp, MSG_INFO_I055, i, c->cols[i]);
      switch (c->coltype[i])
	{
	case COL_TYPE_UNKNOWN:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_UNKNOWN);
	  break;
	case COL_TYPE_CHAR:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_CHAR);
	  break;
	case COL_TYPE_NUMBER:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_NUMBER);
	  break;
	case COL_TYPE_TIME:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_TIME);
	  break;
	case COL_TYPE_EMPTY:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_EMPTY);
	  break;
	case COL_TYPE_DATE_YYYYMMDD:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_DATE_YYYYMMDD);
	  break;
	case COL_TYPE_DATE_MMDDYYYY:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_DATE_MMDDYYYY);
	  break;
	case COL_TYPE_DATE_DDMMYYYY:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_DATE_DDMMYYYY);
	  break;
	case COL_TYPE_DATE_UNKNOWN:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_DATE_UNK);
	  break;
	default:
	  fprintf(fp, MSG_INFO_I056, LIT_COL_BAD);
	  break;
	}
	      
      if (i >= c->num_cols)
	break;
    }

} /* cols_show() */
