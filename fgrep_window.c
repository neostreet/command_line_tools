#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define TITLELEN 1024
static char title[TITLELEN];
static char exp_title[TITLELEN];

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "\
usage: fgrep (-c) (-nl) (-nt) (-space_charc) (-doublequote_charc)\n\
  (-file_maxn) window_length string filename\n";

static char fmt_str[] = "%03d:%s\n";
static char fmt_str2[] = "    %s\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  FILE *fptr0;
  FILE *fptr;
  struct stat statbuf;
  char *cpt;
  int chara;
  int linelen;
  int searchlen;
  int line_no;
  int put_title;
  int tries;
  int m;
  int n;
  int case_sens;
  int line_numbers;
  bool bTitle;
  int string_arg;
  int put_search_string;
  int put_line;
  bool bSpaceChar;
  char space_char;
  bool bDoubleQuoteChar;
  char doublequote_char;
  int file_max;
  int file_count;
  unsigned int window_length;
  unsigned int last_match_line_no;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  case_sens = 0;
  line_numbers = 1;
  bTitle = true;
  bSpaceChar = false;
  bDoubleQuoteChar = false;
  file_max = -1;

  for (curr_arg = 1; ; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-c"))
      case_sens = 1;
    else if (!strcmp(argv[curr_arg],"-nl"))
      line_numbers = 0;
    else if (!strcmp(argv[curr_arg],"-nt"))
      bTitle = false;
    else if (!strncmp(argv[curr_arg],"-space_char",11)) {
      bSpaceChar = true;
      sscanf(&argv[curr_arg][11],"%c",&space_char);
    }
    else if (!strncmp(argv[curr_arg],"-doublequote_char",17)) {
      bDoubleQuoteChar = true;
      sscanf(&argv[curr_arg][17],"%c",&doublequote_char);
    }
    else if (!strncmp(argv[curr_arg],"-file_max",9))
      sscanf(&argv[curr_arg][9],"%d",&file_max);
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 2;
  }

  if (argc - curr_arg == 2)
    fptr0 = stdin;
  else if ((fptr0 = fopen(argv[argc - 1],"r")) == NULL) {
    printf(couldnt_open,argv[argc - 1]);
    return 3;
  }

  sscanf(argv[curr_arg],"%d",&window_length);

  string_arg = curr_arg + 1;

  if (bSpaceChar || bDoubleQuoteChar || !case_sens) {
    for (n = 0; (chara = argv[string_arg][n]); n++) {
      if (bSpaceChar) {
        if (chara == space_char)
          argv[string_arg][n] = ' ';
      }

      if (bDoubleQuoteChar) {
        if (chara == doublequote_char)
          argv[string_arg][n] = '"';
      }

      if (!case_sens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          argv[string_arg][n] = (char)(chara - 'A' + 'a');
      }
    }
  }

  searchlen = strlen(argv[string_arg]);
  put_search_string = 0;

  if (file_max != -1)
    file_count = 0;

  for ( ; ; ) {
    fscanf(fptr0,"%s",title);

    if (feof(fptr0)) {
      if (fptr0 != stdin)
        fclose(fptr0);

      break;
    }

    if (file_max != -1) {
      file_count++;

      if (file_count > file_max)
        break;
    }

    if ((fptr = fopen(title,"r")) == NULL) {
      printf(couldnt_open,title);

      continue;
    }

    if (stat(title,&statbuf)) {
      printf(couldnt_get_status,title);

      continue;
    }

    cpt = ctime(&statbuf.st_mtime);
    cpt[strlen(cpt) - 1] = 0;
    sprintf(exp_title,"%s %s",cpt,title);

    if (bTitle)
      put_title = 0;

    last_match_line_no = 0;

    for (line_no = 1; ; line_no++) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr)) {
        if (bTitle && put_title)
          putchar(0x0a);

        fclose(fptr);

        break;
      }

      if ((last_match_line_no) &&
        (line_no - last_match_line_no < window_length))
        put_line = 1;
      else {
        put_line = 0;

        if (linelen >= searchlen) {
          tries = linelen - searchlen + 1;

          for (m = 0; m < tries; m++) {
            for (n = 0; n < searchlen; n++) {
              chara = line[m+n];

              if (!case_sens)
                if ((chara >= 'A') && (chara <= 'Z'))
                  chara -= 'A' - 'a';

              if (chara != argv[string_arg][n])
                break;
            }

            if (n == searchlen)
              break;
          }

          if (n == searchlen) {
            put_line = 1;
            last_match_line_no = line_no;
          }
        }
      }

      if (put_line) {
        if (!put_search_string) {
          printf("search string: %s\n\n",argv[string_arg]);
          put_search_string = 1;
        }

        if (bTitle && !put_title) {
          printf("%s\n",exp_title);

          for (n = strlen(exp_title); (n); n--)
            putchar('=');

          putchar(0x0a);

          put_title = 1;
        }

        if (line_numbers) {
          if (line_no == last_match_line_no)
            printf(fmt_str,line_no,line);
          else
            printf(fmt_str2,line);
        }
        else
          printf("%s\n",line);
      }
    }
  }

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}
