#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

static char usage[] = "\
usage: peek (-title) (-get_date_from_cwd) filename (filename ...) start_line num_lines\n";
static char couldnt_open[] = "couldn't open %s\n";

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_date_from_cwd(char *cwd,char **date_string_ptr);

int main(int argc,char **argv)
{
  int n;
  char *date_string;
  int retval;
  FILE *fptr;
  bool bTitle;
  bool bGetDateFromCwd;
  bool bStdin;
  int curr_arg;
  int startl;
  int num_lines;
  int endl;
  int lineno;
  int linelen;
  int titlelen;
  
  if (argc < 3) {
    printf(usage);
    return 1;
  }

  bTitle = false;
  bGetDateFromCwd = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-title"))
      bTitle = true;
    else if (!strcmp(argv[curr_arg],"-get_date_from_cwd"))
      bGetDateFromCwd = true;
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 2;
  }

  if (bGetDateFromCwd) {
    getcwd(save_dir,_MAX_PATH);

    retval = get_date_from_cwd(save_dir,&date_string);

    if (retval) {
      printf("get_date_from_cwd() failed: %d\n",retval);
      return 3;
    }
  }

  if (argc - curr_arg == 2) {
    bStdin = true;
    fptr = stdin;
    bTitle = false;
  }
  else {
    bStdin = false;

    if (argc - curr_arg > 3)
      bTitle = true;
  }

  sscanf(argv[argc - 2],"%d",&startl);
  sscanf(argv[argc - 1],"%d",&num_lines);
  endl = startl + num_lines - 1;

  for ( ; ; ) {
    if (!bStdin) {
      if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
        printf(couldnt_open,argv[curr_arg]);

        curr_arg++;

        if (curr_arg == argc - 2)
          break;

        continue;
      }
    }

    if (bTitle) {
      if (num_lines == 1)
        printf("%s ",argv[curr_arg]);
      else {
        printf("%s\n",argv[curr_arg]);
        titlelen = strlen(argv[curr_arg]);

        for (n = 0; n < titlelen; n++)
          putchar('=');

        putchar(0x0a);
      }
    }

    lineno = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      lineno++;

      if ((lineno >= startl) && (lineno <= endl)) {
        if (!bGetDateFromCwd)
          printf("%s\n",line);
        else
          printf("%s %s\n",line,date_string);
      }

      if (lineno == endl)
        break;
    }

    if (bStdin)
      break;

    fclose(fptr);

    if (bTitle && num_lines != 1)
      putchar(0x0a);

    curr_arg++;

    if (curr_arg == argc - 2)
      break;
  }

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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

static char sql_date_string[11];

static int get_date_from_cwd(char *cwd,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(cwd);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (cwd[n] == '/') {
      slash_count++;

      if (slash_count == 2)
        break;
    }
  }

  if (slash_count != 2)
    return 1;

  if (cwd[n+5] != '/')
    return 2;

  strncpy(sql_date_string,&cwd[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&cwd[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&cwd[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

  return 0;
}
