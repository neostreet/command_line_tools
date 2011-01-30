#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#define strcmpi strcmp
#endif
#include <time.h>
#include <malloc.h>

struct file_info {
  char *filename;
  time_t filetime;
  off_t filesize;
};

struct file_info *fi;
int *file_ixs;

static char usage[] = "\
usage: mydir ([/o(-)n | /o(-)d | /o(-)s]) (-terse) (-name_last)\n"
"  -sincedate fnam (fnam ...)\n";

static int which_compare;

#define NAME_ASCENDING      0
#define NAME_DESCENDING     1
#define DATE_ASCENDING      2
#define DATE_DESCENDING     3
#define SIZE_ASCENDING      4
#define SIZE_DESCENDING     5

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

static time_t cvt_date(char *date_str);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bTerse;
  int bNameLast;
  int bSince;
  time_t since_date;
  int bSkip;
  int num_files;
  int num_files_found;
  struct stat statbuf;
  int max_filename_len;
  int curr_filename_len;
  char fmt_str_buf[40];
  char *cpt;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  which_compare = NAME_ASCENDING;
  bTerse = FALSE;
  bNameLast = FALSE;
  bSince = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmpi(argv[curr_arg],"/on"))
      which_compare = NAME_ASCENDING;
    else if (!strcmpi(argv[curr_arg],"/o-n"))
      which_compare = NAME_DESCENDING;
    else if (!strcmpi(argv[curr_arg],"/od"))
      which_compare = DATE_ASCENDING;
    else if (!strcmpi(argv[curr_arg],"/o-d"))
      which_compare = DATE_DESCENDING;
    else if (!strcmpi(argv[curr_arg],"/os"))
      which_compare = SIZE_ASCENDING;
    else if (!strcmpi(argv[curr_arg],"/o-s"))
      which_compare = SIZE_DESCENDING;
    else if (!strcmpi(argv[curr_arg],"-terse"))
      bTerse = TRUE;
    else if (!strcmpi(argv[curr_arg],"-name_last"))
      bNameLast = TRUE;
    else if (!strncmp(argv[curr_arg],"-since",6)) {
      bSince = TRUE;

      since_date = cvt_date(&argv[curr_arg][6]);

      if (since_date == -1L) {
        printf("%s: invalid date\n",&argv[curr_arg][6]);
        return 2;
      }
    }
    else
      break;
  }

  num_files = argc - curr_arg;

  if (!num_files) {
    printf(usage);
    return 3;
  }

  if ((fi = (struct file_info *)malloc(sizeof (struct file_info) * num_files)) == NULL) {
    printf("malloc of name structures failed\n");
    return 4;
  }

  if ((file_ixs = (int *)malloc(sizeof (int) * num_files)) == NULL) {
    printf("malloc of name indices failed\n");
    free(fi);
    return 5;
  }

  num_files_found = 0;

  for ( ; curr_arg < argc; curr_arg++) {
    if (stat(argv[curr_arg],&statbuf)) {
      printf("couldn't get status of %s\n",argv[curr_arg]);
      continue;
    }

    fi[num_files_found].filename = argv[curr_arg];
    fi[num_files_found].filetime = statbuf.st_mtime;
    fi[num_files_found++].filesize = statbuf.st_size;
  }

  if (!num_files_found) {
    printf("no files found\n");
    free(fi);
    free(file_ixs);
    return 6;
  }

  for (n = 0; n < num_files_found; n++)
    file_ixs[n] = n;

  qsort(file_ixs,num_files_found,sizeof (int),compare);

  if (!bTerse) {
    if (!bNameLast) {
      max_filename_len = -1;

      for (n = 0; n < num_files_found; n++) {
        if (!bSince)
          bSkip = FALSE;
        else
          bSkip = fi[file_ixs[n]].filetime < since_date;

        if (!bSkip) {
          curr_filename_len = strlen(fi[n].filename);

          if (curr_filename_len > max_filename_len)
            max_filename_len = curr_filename_len;
        }
      }

      sprintf(fmt_str_buf,"%%-%ds   %%10ld    %%s",max_filename_len);
    }

    for (n = 0; n < num_files_found; n++) {
      if (!bSince)
        bSkip = FALSE;
      else
        bSkip = fi[file_ixs[n]].filetime < since_date;

      if (!bSkip) {
        cpt = ctime(&fi[file_ixs[n]].filetime);

        if (!bNameLast) {
          printf(fmt_str_buf,
            fi[file_ixs[n]].filename,
            fi[file_ixs[n]].filesize,
            cpt);
        }
        else {
          cpt[strlen(cpt) - 1] = 0;

          printf("%s %10d %s\n",
            cpt,
            fi[file_ixs[n]].filesize,
            fi[file_ixs[n]].filename);
        }
      }
    }
  }
  else {
    for (n = 0; n < num_files_found; n++) {
      if (!bSince)
        bSkip = FALSE;
      else
        bSkip = fi[file_ixs[n]].filetime < since_date;

      if (!bSkip)
        printf("%s\n",fi[file_ixs[n]].filename);
    }
  }

  free(fi);
  free(file_ixs);

  return 0;
}

static time_t cvt_date(char *date_str)
{
  struct tm tim;
  char hold[11];
  int date_len;
  int bufix;
  int holdix;
  int digits[3];
  int n;
  time_t ret_tm;

  date_len = strlen(date_str);

  if (!date_len || (date_len > 10))
    return -1L;

  bufix = 0;

  for (n = 0; n < 3; n++) {
    holdix = 0;

    for ( ; bufix < date_len; ) {
      if (date_str[bufix] == '-') {
        bufix++;
        break;
      }

      if ((date_str[bufix] < '0') || (date_str[bufix] > '9'))
        return -1L;

      hold[holdix++] = date_str[bufix++];
    }

    if (!holdix || ((n != 2) && (bufix == date_len)))
      return -1L;

    hold[holdix] = 0;
    digits[n] = atoi(hold);

    if ((digits[n] > date_checks[n].upper) ||
      (digits[n] < date_checks[n].lower))
      return -1L;
  }

  if (digits[2] >= 100)
    if (digits[2] < 1970)
      return -1L;
    else
      digits[2] -= 1900;

  tim.tm_mon = digits[0] - 1;
  tim.tm_mday = digits[1];
  tim.tm_year = digits[2];

  tim.tm_hour = 0;
  tim.tm_min = 0;
  tim.tm_sec = 0;

  tim.tm_isdst = 0;

  ret_tm = mktime(&tim);

  return ret_tm;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;
  char *name1;
  char *name2;
  time_t time1;
  time_t time2;
  off_t size1;
  off_t size2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  switch(which_compare) {
    case NAME_ASCENDING:
    case NAME_DESCENDING:
      name1 = fi[ix1].filename;
      name2 = fi[ix2].filename;

      if (which_compare == NAME_ASCENDING)
        return strcmp(name1,name2);
      else
        return strcmp(name2,name1);

      break;
    case DATE_ASCENDING:
    case DATE_DESCENDING:
      time1 = fi[ix1].filetime;
      time2 = fi[ix2].filetime;

      if (which_compare == DATE_ASCENDING) {
        if (time1 < time2)
          return -1;
        else if (time1 > time2)
          return 1;
      }
      else {  /* (which_compare == DATE_ASCENDING) */
        if (time1 > time2)
          return -1;
        else if (time1 < time2)
          return 1;
      }

      break;
    case SIZE_ASCENDING:
    case SIZE_DESCENDING:
      size1 = fi[ix1].filesize;
      size2 = fi[ix2].filesize;

      if (which_compare == SIZE_ASCENDING) {
        if (size1 < size2)
          return -1;
        else if (size1 > size2)
          return 1;
      }
      else {  /* (which_compare == SIZE_ASCENDING) */
        if (size1 > size2)
          return -1;
        else if (size1 < size2)
          return 1;
      }

      break;
  }

  return 0;
}
