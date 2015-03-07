#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_SPECIAL_CHARS 20

struct pair {
  int val1;
  char val2;
};

#define TITLELEN 1024
static char title[TITLELEN];
static char exp_title[TITLELEN];

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "\
usage: fgrep (-c) (-nl) (-nt) (-nd) (-special_charhhc) (-file_maxn) (-anchor)\n"
"  string filename\n";
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
  int p;
  int case_sens;
  int line_numbers;
  bool bTitle;
  bool bDate;
  bool bAnchor;
  int string_arg;
  int put_search_string;
  int put_line;
  int file_max;
  int file_count;
  int num_special_chars;
  struct pair special_chars[MAX_SPECIAL_CHARS];
  char buf[3];

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  case_sens = 0;
  line_numbers = 1;
  bTitle = true;
  bDate = true;
  num_special_chars = 0;
  file_max = -1;
  bAnchor = false;

  for (curr_arg = 1; ; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-c"))
      case_sens = 1;
    else if (!strcmp(argv[curr_arg],"-nl"))
      line_numbers = 0;
    else if (!strcmp(argv[curr_arg],"-nt"))
      bTitle = false;
    else if (!strcmp(argv[curr_arg],"-nd"))
      bDate = false;
    else if (!strncmp(argv[curr_arg],"-special_char",13)) {
      if (num_special_chars == MAX_SPECIAL_CHARS) {
        printf("num_special_chars may not exceed %d\n",MAX_SPECIAL_CHARS);
        return 2;
      }

      buf[0] = argv[curr_arg][13];
      buf[1] = argv[curr_arg][14];
      buf[2] = 0;

      sscanf(buf,"%x",&special_chars[num_special_chars].val1);
      sscanf(&argv[curr_arg][15],"%c",&special_chars[num_special_chars++].val2);
    }
    else if (!strncmp(argv[curr_arg],"-file_max",9))
      sscanf(&argv[curr_arg][9],"%d",&file_max);
    else if (!strcmp(argv[curr_arg],"-anchor"))
      bAnchor = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 3;
  }

  if (argc - curr_arg == 1)
    fptr0 = stdin;
  else if ((fptr0 = fopen(argv[argc - 1],"r")) == NULL) {
    printf(couldnt_open,argv[argc - 1]);
    return 4;
  }

  string_arg = curr_arg;

  if (num_special_chars || !case_sens) {
    for (n = 0; (chara = argv[string_arg][n]); n++) {
      for (p = 0; p < num_special_chars; p++) {
        if (chara == special_chars[p].val2) {
          argv[string_arg][n] = (char)special_chars[p].val1;
          break;
        }
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

    if (bDate) {
      cpt = ctime(&statbuf.st_mtime);

      if (cpt[8] == ' ')
        cpt[8] = '0';

      cpt[strlen(cpt) - 1] = 0;
      sprintf(exp_title,"%s %s",cpt,title);
    }
    else
      strcpy(exp_title,title);

    if (bTitle)
      put_title = 0;

    for (line_no = 1; ; line_no++) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr)) {
        if (bTitle && put_title)
          putchar(0x0a);

        fclose(fptr);

        break;
      }

      put_line = 0;

      if (linelen >= searchlen) {
        if (!bAnchor)
          tries = linelen - searchlen + 1;
        else
          tries = 1;

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

        if (n == searchlen)
          put_line = 1;
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

        if (line_numbers)
          printf("%03d:%s\n",line_no,line);
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
