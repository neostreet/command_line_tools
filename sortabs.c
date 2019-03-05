#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#ifndef CYGWIN
#define O_BINARY 0
#endif
#endif

#define LINEFEED 0x0a

static char save_dir[_MAX_PATH];

static char usage[] =
"usage: sortabs (-descending) (-line_numbers) (-pos_streak) (-neg_streak)\n"
"  (-ends_with_a_bang) (-ends_with_a_big_bang) (-only_zero) (-only_nonzero)\n"
"  (-get_date_from_path) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";

static bool bDescending;

static int *vals;

int compare(const void *elem1,const void *elem2);
static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bLineNumbers;
  bool bStreak;
  bool bPosStreak;
  bool bEndsWithABang;
  bool bEndsWithABigBang;
  bool bOnlyZero;
  bool bOnlyNonzero;
  bool bGetDateFromPath;
  bool bVerbose;
  int last_winning_hand_ix;
  bool bBang;
  int ix;
  struct stat statbuf;
  off_t mem_amount;
  char *in_buf;
  int in_buf_ix;
  int fhndl;
  int bytes_to_io;
  int lines;
  char **cppt;
  int curr_line;
  int retval;
  char *date_string;
  int work;
  int *ixs;
  int streak_count;

  if ((argc < 2) || (argc > 12)) {
    printf(usage);
    return 1;
  }

  bDescending = false;
  bLineNumbers = false;
  bStreak = false;
  bEndsWithABang = false;
  bEndsWithABigBang = false;
  bOnlyZero = false;
  bOnlyNonzero = false;
  bGetDateFromPath = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = true;
    else if (!strcmp(argv[curr_arg],"-line_numbers"))
      bLineNumbers = true;
    else if (!strcmp(argv[curr_arg],"-pos_streak")) {
      bStreak = true;
      bPosStreak = true;
    }
    else if (!strcmp(argv[curr_arg],"-neg_streak")) {
      if (bStreak) {
        printf("can't specify both -pos_streak and -neg_streak\n");
        return 2;
      }

      bStreak = true;
      bPosStreak = false;
    }
    else if (!strcmp(argv[curr_arg],"-ends_with_a_bang")) {
      bEndsWithABang = true;
      bStreak = true;
      bPosStreak = true;
      bDescending = true;
    }
    else if (!strcmp(argv[curr_arg],"-ends_with_a_big_bang")) {
      bEndsWithABigBang = true;
      bStreak = true;
      bPosStreak = true;
      bDescending = true;
    }
    else if (!strcmp(argv[curr_arg],"-only_zero"))
      bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-only_nonzero"))
      bOnlyNonzero = true;
    else if (!strcmp(argv[curr_arg],"-get_date_from_path"))
      bGetDateFromPath = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (bEndsWithABang && bEndsWithABigBang) {
    printf("can't specify both -ends_with_a_bang and -ends_with_a_big_bang\n");
    return 3;
  }

  if (bOnlyZero && bOnlyNonzero) {
    printf("can't specify both -only_zero and -only_nonzero\n");
    return 4;
  }

  if (bStreak || bGetDateFromPath)
    getcwd(save_dir,_MAX_PATH);

  if (bGetDateFromPath) {
    retval = get_date_from_path(save_dir,'/',2,&date_string);

    if (retval) {
      printf("get_date_from_path() on %s failed: %d\n",save_dir,retval);
      return 5;
    }
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 6;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 7;
  }

  mem_amount = (size_t)statbuf.st_size;

  if ((in_buf = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    return 8;
  }

  if ((fhndl = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    free(in_buf);
    return 9;
  }

  bytes_to_io = (int)mem_amount;

  if (read(fhndl,in_buf,bytes_to_io) != bytes_to_io) {
    printf(read_failed,argv[curr_arg],bytes_to_io);
    free(in_buf);
    close(fhndl);
    return 10;
  }

  lines = 0;

  for (n = 0; n < bytes_to_io; n++) {
    if (in_buf[n] == LINEFEED)
      lines++;
  }

  if ((cppt = (char **)malloc(lines * sizeof (char *))) == NULL) {
    printf(malloc_failed,lines * sizeof (char *));
    return 11;
  }

  curr_line = 0;
  cppt[curr_line++] = in_buf;

  for (n = 0; n < bytes_to_io; n++) {
    if (in_buf[n] == LINEFEED) {
      in_buf[n] = 0;

      if (curr_line < lines)
        cppt[curr_line++] = &in_buf[n + 1];
    }
  }

  if ((vals = (int *)malloc(lines * sizeof (int))) == NULL) {
    printf("couldn't malloc %d structs\n",lines);
    return 12;
  }

  if ((ixs = (int *)malloc(lines * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",lines);
    free(vals);
    return 13;
  }

  ix = 0;

  for (n = 0; n < lines; n++) {
    sscanf(cppt[n],"%d",&vals[n]);
    ixs[n] = n;
  }

  if (bEndsWithABang || bEndsWithABigBang) {
    last_winning_hand_ix = -1;

    for (n = lines - 1; n >= 0; n--) {
      if (vals[n] > 0) {
        last_winning_hand_ix = n;
        break;
      }
    }
  }

  qsort(ixs,lines,sizeof (int),compare);

  if (!bStreak) {
    for (n = 0; n < lines; n++) {
      if (!bLineNumbers) {
        if (!bVerbose)
          printf("%d\n",vals[ixs[n]]);
        else
          printf("%s\n",cppt[ixs[n]]);
      }
      else {
        if (!bVerbose)
          printf("%d (%d)\n",vals[ixs[n]],ixs[n]+1);
        else
          printf("%s (%d)\n",cppt[ixs[n]],ixs[n]+1);
      }
    }
  }
  else {
    if (!bEndsWithABang && !bEndsWithABigBang) {
      streak_count = 0;

      for (n = 0; n < lines; n++) {
        if (!bPosStreak) {
          if (vals[ixs[n]] < 0)
            streak_count++;
          else
            break;
        }
        else {
          if (vals[ixs[n]] > 0)
            streak_count++;
          else
            break;
        }
      }
    }
    else {
      bBang = false;

      for (n = 0; n < lines; n++) {
        if (vals[ixs[n]] > 0) {
          if (bEndsWithABang) {
            if (ixs[n] == last_winning_hand_ix)
              bBang = true;
          }
          else {
            if (!n && ixs[n] == last_winning_hand_ix)
              bBang = true;
          }
        }
        else
          break;
      }
    }

    if (!bEndsWithABang && !bEndsWithABigBang) {
      if (!bOnlyZero || !streak_count)
        if (!bOnlyNonzero || streak_count)
          printf("%d %s/%s\n",streak_count,save_dir,argv[curr_arg]);
    }
    else if (bBang) {
      if (!bGetDateFromPath)
        printf("%s/%s\n",save_dir,argv[curr_arg]);
      else
        printf("%s\n",date_string);
    }
  }

  free(ixs);
  free(vals);

  free(cppt);
  free(in_buf);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;
  int int1;
  int int2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  int1 = vals[ix1];
  int2 = vals[ix2];

  if (int1 < 0)
    int1 *= -1;

  if (int2 < 0)
    int2 *= -1;

  if (!bDescending)
    return int1 - int2;

  return int2 - int1;
}

static char sql_date_string[11];

static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(path);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (path[n] == slash_char) {
      slash_count++;

      if (slash_count == num_slashes)
        break;
    }
  }

  if (slash_count != num_slashes)
    return 1;

  if (path[n+5] != slash_char)
    return 2;

  strncpy(sql_date_string,&path[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&path[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&path[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

  return 0;
}
