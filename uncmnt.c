#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "uncmnt.h"

#define FALSE 0
#define TRUE  1

static char open_comment[]       = "/*";
static char close_comment[]      = "*/";
static char new_style_comment[]  = "//";
#define COMMENT_LEN 2

int get_uncommented_line_info(FILE *fptr,char *line,int *line_len,int maxllen,
  int *comment_start,int *comment_stop,
  int *bInComment,int *bInDoubleQuote,int *curly_brace_depth)
{
  int chara;
  int local_line_len;
  int m;
  int n;
  int tries;
  int line_ix;
  int start_comment_ix;
  int stop_comment_ix;
  int bInSingleQuote;

  local_line_len = 0;
  start_comment_ix = -1;
  stop_comment_ix = -1;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
    else {
      line[local_line_len] = 0;
      *line_len = local_line_len;
      *comment_start = start_comment_ix;
      *comment_stop = stop_comment_ix;

      return -1;
    }
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;

  if (*bInComment)
    start_comment_ix = 0;

  bInSingleQuote = FALSE;

  line_ix = 0;

  for ( ; (line_ix < local_line_len); ) {
    if (*bInComment) {
      /* search for close comment */
      if (local_line_len - line_ix >= COMMENT_LEN) {
        tries = (local_line_len - line_ix) - COMMENT_LEN + 1;

        for (m = 0; m < tries; m++) {
          for (n = 0; n < COMMENT_LEN; n++) {
            if (line[line_ix+m+n] != close_comment[n])
              break;
          }

          if (n == COMMENT_LEN) {
            *bInComment = FALSE;
            stop_comment_ix = line_ix+m+n;

            line_ix += m + COMMENT_LEN;

            break;
          }
        }
      }

      if (*bInComment) {
        *comment_start = start_comment_ix;
        *comment_stop = local_line_len;

        return 0;
      }

      continue;
    }

    if (bInSingleQuote) {
      /* search for close quote */
      if (local_line_len - line_ix >= 1) {
        tries = local_line_len - line_ix;

        for (m = 0; m < tries; m++) {
          if (line[line_ix+m] == '\'')
            break;
        }

        if (m < tries) {
          bInSingleQuote = FALSE;
          line_ix += m + 1;
        }

        if (bInSingleQuote)
          return 0;
      }

      continue;
    }

    if (*bInDoubleQuote) {
      /* search for close quote */
      if (local_line_len - line_ix >= 1) {
        tries = local_line_len - line_ix;

        for (m = 0; m < tries; m++) {
          if (line[line_ix+m] == '"')
            break;
        }

        if (m < tries) {
          *bInDoubleQuote = FALSE;
          line_ix += m + 1;
        }

        if (*bInDoubleQuote)
          return 0;
      }

      continue;
    }

    /* search for open comment, new style comment, single quote,
       or double quote */
    if (local_line_len - line_ix >= COMMENT_LEN) {
      tries = (local_line_len - line_ix) - COMMENT_LEN + 1;

      for (m = 0; m < tries; m++) {
        if ((line[line_ix+m] == open_comment[0]) &&
          (line[line_ix+m+1] == open_comment[1])) {
          *bInComment = TRUE;
          start_comment_ix = line_ix + m;
          line_ix += m + COMMENT_LEN;

          break;
        }

        if ((line[line_ix+m] == new_style_comment[0]) &&
          (line[line_ix+m+1] == new_style_comment[1])) {
          *comment_start = line_ix + m;
          *comment_stop = local_line_len;

          return 0;
        }

        if (line[line_ix+m] == '\'') {
          bInSingleQuote = TRUE;
          line_ix += m + 1;
          break;
        }

        if (line[line_ix+m] == '"') {
          *bInDoubleQuote = TRUE;
          line_ix += m + 1;
          break;
        }
      }
    }

    if (!(*bInComment) && !bInSingleQuote && !(*bInDoubleQuote)) {
      if (line[line_ix] == '{')
        (*curly_brace_depth)++;
      else if (line[line_ix] == '}')
        (*curly_brace_depth)--;

      line_ix++;
    }
  }

  if (*bInComment)
    stop_comment_ix = local_line_len;

  *comment_start = start_comment_ix;
  *comment_stop = stop_comment_ix;

  return 0;
}

int uncommented_line_blank(char *line,int line_len,
  int comment_start,int comment_stop)
{
  int n;

  if (!line_len || ((comment_start == 0) && (comment_stop == line_len)))
    return TRUE;

  for (n = 0; n < line_len; n++) {
    if ((n < comment_start) || (n >= comment_stop)) {
      if (!isspace(line[n]))
        return  FALSE;
    }
  }

  return TRUE;
}

void make_uncommented_line(char *uncommented_line,char *line,int line_len,
  int comment_start,int comment_stop)
{
  int n;

  for (n = 0; n < line_len; n++) {
    if ((n >= comment_start) && (n < comment_stop))
      uncommented_line[n] = ' ';
    else
      uncommented_line[n] = line[n];
  }

  uncommented_line[n] = 0;
}
