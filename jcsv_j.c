/*
 * Copyright (c) 1994 1995 1996 ... 2023 2024
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
 * jcsv_j.c -- routines from lib j_lib2
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "jcsv.h"

/*
 * j2_bye_char() -- removes all occurances of a specific a char from a string
 */
int j2_bye_char(char *buffer, char c)

{
  char *temp_str;
  int i, j, num_c = 0;
  int found = (int) FALSE;
#ifdef OpenBSD
  size_t len;
#endif
  
  if (c == JLIB2_CHAR_NULL)
    return(0);
  
  i = strlen(buffer) + 2;
  temp_str = calloc(i, sizeof(char));
  if (temp_str == (char *) NULL)
    return(-1);
  
  for ( i=0, j=0; buffer[i] != JLIB2_CHAR_NULL; i++)
    {
      if (buffer[i] == c)
	{
	  ++num_c;
	  found = (int) TRUE;
	}
      else
	{
	  temp_str[j] = buffer[i];
	  ++j;
	}
    }

  if (found == (int) FALSE)
    {
      free(temp_str);
      return(num_c);
    }

  /* note: size of buffer will never increase and   */
  /*       we removed at least 1 embedded character */
#ifdef OpenBSD
  len = strlen(temp_str) + 1;
  strlcpy(buffer, temp_str, len);
#else
  strcpy(buffer, temp_str);
#endif

  free(temp_str);
  return(num_c);

} /* j2_bye_char() */


/*
 * j2_chg_char() -- replaces all occurrences of one character 
 *                  with another char
 */
long int j2_chg_char(char old, char new, char *s, SSIZE_T force_size)

{
  size_t found = (size_t) 0;

  if (s == (char *) NULL)
    return(found);

  if (force_size < (SSIZE_T) 1)
    {
      for ( ; (*s) != JLIB2_CHAR_NULL; s++)
        {
          if ((*s) == old)
	    {
	      found++;
	      (*s) = new;
	    }
        }
    }
  else
    {
      for ( ; found < force_size; found++)
        {
          if (s[found] == old)
	    (*s) = new;
        }
    }

  return((long int) found);

} /* j2_chg_char() */

/*
 * j2_f_exist() -- determines if a file exists
 */
int j2_f_exist(char *file_name)

{
  if (file_name == (char *) NULL)
    return((int) FALSE);

#ifdef _MSDOS
  if (access(file_name, 00) == -1)
    return (FALSE);
  else
    return (TRUE);
#else
  struct stat file_info;
  if (stat(file_name, &file_info) == 0)
    return (TRUE);
  else
    return (FALSE);
#endif

} /* j2_f_exist() */

/*
 * j2_fix_delm() -- generates a new string to allow strtok(3) to work
 *                  correctly. strtok(3) has problems when delimiters
 *                  are next to each other.
 */
long int j2_fix_delm(char delm, char **fixed, char *buf)
{
  size_t sz, i, j;

  (*fixed) = (char *) NULL;

  if (buf == (char *) NULL)
    return(0);
  if (strlen(buf) < 1)
    return(0);

  sz = (strlen(buf) * 2) + 5;

  (*fixed) = (char *) calloc(sz, sizeof(char));

  if ((*fixed) == (char *) NULL)
    return(0);

  for (i = 0, j=0; buf[i] != JLIB2_CHAR_NULL; i++)
    {
      if ((buf[i] == delm) && (buf[(i + 1)] == delm))
        {
          (*fixed)[j] = buf[i];
          j++;
          (*fixed)[j] = ' ';
        }
      else
        (*fixed)[j] = buf[i];
      j++;
    }

  return((long int)strlen((*fixed)));

} /* j2_fix_delm() */

/*
 * j2_get_prgname() -- return filename or default name.
 *
 *                     basename() is not used.  Some OS Systems do not
 *                     have basename() so this can be a basename()
 *                     replacement
 */
char *j2_get_prgname(char *argv_0, char *default_name)

{
  int i = 0, j = 0;

  if (argv_0 == (char *) NULL)
    {
      if (default_name == (char *) NULL)
	return((char *) NULL);
      else
	return(strdup(default_name));
    }
  
  /* basename(3) not in Coherent or MS-DOS */
 
  for (i = 0, j = 0; argv_0[i] != JLIB2_CHAR_NULL; i++)
    {
      if (argv_0[i] == '/')
	j = i + 1;
      else
	{
	  if (argv_0[i] == '\\')
	    j = i + 1;
	}
    } 

  if (argv_0[j] == JLIB2_CHAR_NULL)
    if (default_name == (char *) NULL)
      return((char *) NULL);
    else
      return(strdup(default_name));
  else
    return(strdup(&(argv_0[j])));

} /* j2_get_prgname() */

/*
 * j2_is_numr() -- determines if all characters are numeric
 */
int j2_is_numr(char *s)

{
  if (s == (char *) NULL)
    return((int) FALSE); /* NULL pointer */
  
  for ( ; (*s) != JLIB2_CHAR_NULL; s++)
    {
      if ( ! isdigit((int)(*s)) )
	return(FALSE);
    }
  
  return(TRUE);

} /* j2_is_numr() */

/*
 * j2_justleft() -- left justifies a string (removes beginning white
 *                  space) returns -1 on failure, strlen on success
 */
long int j2_justleft(char *s)

{
  char *temp_s;
  int i;
#ifdef OpenBSD
  size_t len;
#endif
  
  if (s == (char *) NULL)
    return(0L); /* NULL pointer, pretend 0 */

  if (strlen(s) < 2)
    {
      if (isspace((*s)))
	{
	  (*s) = JLIB2_CHAR_NULL;
	  return(0L);
	}
      else
	return((long int) strlen(s));
    }
  
#ifdef OpenBSD
  len = strlen(s) + 1;
  temp_s = strndup(s, len);
#else
  temp_s = strdup(s);
#endif

  if ( temp_s == (char *) NULL )
    return(-1L); /* not enough memory */
  
  for (i=0; temp_s[i] != JLIB2_CHAR_NULL; i++)
    {
      if (! isspace((int) temp_s[i]) )
	break;
    }
 
#ifdef OpenBSD
  strlcpy(s,&(temp_s[i]),len);
#else
  strcpy(s,&(temp_s[i]));
#endif

  free(temp_s);
  
  return((long int) strlen(s));

} /* j2_justleft() */

/*
 * j2_rtw() -- removes trailing white space
 */
long int j2_rtw(char *buffer)

{
  char *last_non_space;
  char *b = buffer;

  if (buffer == (char *) NULL)
    return(0L); /* NULL pointer */

  last_non_space = buffer;

  for ( ; (*buffer) != JLIB2_CHAR_NULL; buffer++)
    {
      if ( ! isspace((int)(*buffer)) )
	last_non_space = buffer;
    }

  if ( ! isspace ((int) *last_non_space) )
    last_non_space++;

  (*last_non_space) = JLIB2_CHAR_NULL;

  return((long int) strlen(b));

} /* j2_rtw() */

