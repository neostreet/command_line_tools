#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 65530
char *line;

static char usage[] = "\
usage: grep_window (-c) (-ss) (-t) (-l) (-nss) (-nt) (-nl)\n\
  (-rev) (-hex) (-chopn) (-off) (-space_charc) (-doublequote_charc)\n\
  (-tab_charc) window_length str file (file ...)\n";

static char fmt_str[] = "%03d:%s\n";
static char fmt_str2[] = "    %s\n";

void GetLine(FILE *fptr,char *line,unsigned int *line_len,int maxllen);

#ifndef WIN32
#define strcmpi strcmp
#define strnicmp strncmp
#endif

int get_nib(int chara)
{
  if ((chara >= '0') && (chara <= '9'))
    return chara - '0';

  if ((chara >= 'a') && (chara <= 'f'))
    return chara - 'a' + 10;

  if ((chara >= 'A') && (chara <= 'F'))
    return chara - 'A' + 10;

  return -1;
}

int main(int argc,char **argv)
{
  FILE *fptr;
  int chara;
  unsigned int linelen;
  unsigned int searchlen;
  unsigned int line_no;
  unsigned int first_line_no;
  unsigned int put_title;
  unsigned int tries;
  unsigned int m;
  unsigned int n;
  unsigned int case_sens;
  int curr_arg;
  unsigned int show_title;
  unsigned int line_numbers;
  unsigned int string_arg;
  unsigned int show_search_string;
  unsigned int put_search_string;
  unsigned int reverse;
  unsigned int put_line;
  unsigned int bStdin;
  unsigned int ss_specified;
  unsigned int t_specified;
  unsigned int l_specified;
  unsigned int bHex;
  unsigned int bChop;
  unsigned int chop;
  unsigned int bOffset;
  unsigned int bSpaceChar;
  char space_char;
  unsigned int bDoubleQuoteChar;
  char doublequote_char;
  unsigned int bTabChar;
  char tab_char;
  int nibbles[2];
  int which_nib;
  unsigned int window_length;
  unsigned int last_match_line_no;

  case_sens = FALSE;
  ss_specified = FALSE;
  t_specified = FALSE;
  l_specified = FALSE;
  reverse = FALSE;
  bHex = FALSE;
  bChop = FALSE;
  bOffset = FALSE;
  bSpaceChar = FALSE;
  bDoubleQuoteChar = FALSE;
  bTabChar = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (argv[curr_arg][0] != '-')
      break;

    if (!strcmpi(argv[curr_arg],"-c"))
      case_sens = TRUE;
    else if (!strcmpi(argv[curr_arg],"-ss")) {
      show_search_string = TRUE;
      ss_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-t")) {
      show_title = TRUE;
      t_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-l")) {
      line_numbers = TRUE;
      l_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-nss")) {
      show_search_string = FALSE;
      ss_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-nt")) {
      show_title = FALSE;
      t_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-nl")) {
      line_numbers = FALSE;
      l_specified = TRUE;
    }
    else if (!strcmpi(argv[curr_arg],"-rev"))
      reverse = TRUE;
    else if (!strcmpi(argv[curr_arg],"-hex"))
      bHex = TRUE;
    else if (!strnicmp(argv[curr_arg],"-chop",5)) {
      bChop = TRUE;
      sscanf(&argv[curr_arg][5],"%u",&chop);
    }
    else if (!strcmpi(argv[curr_arg],"-off")) {
      bOffset = TRUE;
      line_numbers = FALSE;
      l_specified = TRUE;
    }
    else if (!strnicmp(argv[curr_arg],"-space_char",11)) {
      bSpaceChar = TRUE;
      sscanf(&argv[curr_arg][11],"%c",&space_char);
    }
    else if (!strnicmp(argv[curr_arg],"-doublequote_char",17)) {
      bDoubleQuoteChar = TRUE;
      sscanf(&argv[curr_arg][17],"%c",&doublequote_char);
    }
    else if (!strnicmp(argv[curr_arg],"-tab_char",9)) {
      bTabChar = TRUE;
      sscanf(&argv[curr_arg][9],"%c",&tab_char);
    }
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 1;
  }

  sscanf(argv[curr_arg],"%d",&window_length);

  string_arg = curr_arg + 1;

  if (bHex) {
    case_sens = TRUE;
    searchlen = strlen(argv[string_arg]);

    if (searchlen % 2) {
      printf("hex string must have an even number of hex digits\n");
      return 2;
    }

    m = 0;

    for (n = 0; n < searchlen; n++) {
      which_nib = n % 2;

      if ((nibbles[which_nib] = get_nib(argv[string_arg][n])) == -1) {
        printf("illegal hex string\n");
        return 3;
      }

      if (which_nib)
        argv[string_arg][m++] = (nibbles[0] << 4) + nibbles[1];
    }

    argv[string_arg][m] = 0;
  }
  else if (bSpaceChar || bDoubleQuoteChar || bTabChar || !case_sens) {
    for (n = 0; (chara = argv[string_arg][n]); n++) {
      if (bSpaceChar) {
        if (chara == space_char)
          argv[string_arg][n] = ' ';
      }

      if (bDoubleQuoteChar) {
        if (chara == doublequote_char)
          argv[string_arg][n] = '"';
      }

      if (bTabChar) {
        if (chara == tab_char)
          argv[string_arg][n] = '\t';
      }

      if (!case_sens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          argv[string_arg][n] = (char)(chara - 'A' + 'a');
      }
    }
  }

  searchlen = strlen(argv[string_arg]);

  if (!strcmpi(argv[string_arg],"doublequote")) {
    argv[string_arg][0] = '"';
    searchlen = 1;
  }

  curr_arg += 2;

  if (curr_arg == argc) {
    bStdin = TRUE;

    if (!ss_specified)
      show_search_string = FALSE;

    /*if (!t_specified)*/
      show_title = FALSE;

    if (!l_specified)
      line_numbers = FALSE;
  }
  else {
    if (argc - curr_arg == 1) {
      /* if wildcard expansion failed, exit with no error message */
      for (n = 0; (chara = argv[curr_arg][n]); n++) {
        if (chara == '*')
          return 4;
      }
    }

    bStdin = FALSE;

    if (!ss_specified) {
      if (argc - string_arg > 2)
        show_search_string = TRUE;
      else
        show_search_string = FALSE;
    }

    if (!t_specified) {
      if (argc - string_arg > 2)
        show_title = TRUE;
      else
        show_title = FALSE;
    }

    if (!l_specified)
      line_numbers = TRUE;
  }

  if ((line = (char *)malloc(MAX_LINE_LEN)) == NULL) {
    printf("malloc of %u bytes failed\n",MAX_LINE_LEN);
    return 5;
  }

  if (show_search_string)
    put_search_string = FALSE;

  /* start of argument loop: */
  for ( ; ; ) {

  if (bStdin)
    fptr = stdin;
  else if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    curr_arg++;

    if (curr_arg == argc)
      break;

    continue;
  }

  if (show_title)
    put_title = FALSE;

  if (bOffset)
    first_line_no = -1;

  last_match_line_no = 0;

  for (line_no = 1; ; line_no++) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr)) {
      if (!bStdin) {
        if (show_title && put_title)
          putchar(0x0a);

        fclose(fptr);
      }

      break;
    }

    if ((last_match_line_no) && (line_no - last_match_line_no < window_length))
      put_line = TRUE;
    else {
      put_line = FALSE;

      if (linelen < searchlen) {
        if (reverse) {
          put_line = TRUE;
          last_match_line_no = line_no;
        }
      }
      else {
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
          if (!reverse) {
            put_line = TRUE;
            last_match_line_no = line_no;
          }
        }
        else if (reverse) {
          put_line = TRUE;
          last_match_line_no = line_no;
        }
      }
    }

    if (put_line) {
      if (show_search_string) {
        if (!put_search_string) {
          printf("search string: %s\n\n",argv[string_arg]);
          put_search_string = TRUE;
        }
      }

      if (show_title) {
        if (!put_title) {
          printf("%s\n",argv[curr_arg]);

          for (n = strlen(argv[curr_arg]); (n); n--)
            putchar('=');

          putchar(0x0a);

          put_title = TRUE;
        }
      }

      if (bChop)
        line[chop] = 0;

      if (line_numbers) {
        if (line_no == last_match_line_no)
          printf(fmt_str,line_no,line);
        else
          printf(fmt_str2,line);
      }
      else if (bOffset) {
        if (first_line_no == -1) {
          printf(fmt_str,0,line);
          first_line_no = line_no;
        }
        else
          printf(fmt_str,line_no - first_line_no,line);
      }
      else
        printf("%s\n",line);
    }
  }

  if (bStdin)
    break;
  else {
    curr_arg++;

    if (curr_arg == argc)
      break;
  }

  /* end of argument loop: */
  }

  free(line);

  return 0;
}

void GetLine(FILE *fptr,char *line,unsigned int *line_len,int maxllen)
{
  int chara;
  unsigned int local_line_len;

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
