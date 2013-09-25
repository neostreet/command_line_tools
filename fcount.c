#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 2048
char line[MAX_LINE_LEN];

static char usage[] =
"usage: fcount (-c) (-terse) (-total) (-widthwidth) string filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char fmt_str[] = "%7d %s\n";
static char fmt_str_terse[] = "%d\n";
static char fmt_str2[128];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bCaseSens;
  bool bTerse;
  bool bTotal;
  bool bWidth;
  int width;
  FILE *fptr0;
  FILE *fptr;
  int chara;
  int offset;
  int num_files;
  int num_files_containing;
  int fnamlen;
  int linelen;
  int searchlen;
  int line_no;
  int tries;
  int m;
  int n;
  int count;
  int tcount;
  int string_arg;

  if ((argc < 3) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bCaseSens = false;
  bTerse = false;
  bTotal = false;
  bWidth = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-c"))
      bCaseSens = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-total"))
      bTotal = true;
    else if (!strncmp(argv[curr_arg],"-width",6)) {
      bWidth = true;
      sscanf(&argv[curr_arg][6],"%d",&width);
      sprintf(fmt_str2,"%%%dd %%s\n",width);
    }
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  string_arg = curr_arg;

  for (offset = 0; (chara = argv[curr_arg][offset]); offset++) {
    if (!bCaseSens) {
      if ((chara >= 'A') && (chara <= 'Z'))
        argv[curr_arg][offset] = (char)(chara - 'A' + 'a');
    }
  }

  searchlen = offset;

  curr_arg++;

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  tcount = 0;
  num_files = 0;
  num_files_containing = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&fnamlen,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    num_files++;
    count = 0;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    for (line_no = 1; ; line_no++) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr)) {
        if (count) {
          if (!bTerse) {
            if (!bWidth)
              printf(fmt_str,count,filename);
            else
              printf(fmt_str2,count,filename);
          }
          else
            printf(fmt_str_terse,count);

          tcount += count;
        }

        fclose(fptr);
        break;
      }

      if (linelen >= searchlen) {
        tries = linelen - searchlen + 1;

        for (m = 0; m < tries; m++) {
          for (n = 0; n < searchlen; n++) {
            chara = line[m+n];

            if (!bCaseSens)
              if ((chara >= 'A') && (chara <= 'Z'))
                chara -= 'A' - 'a';

            if (chara != argv[string_arg][n])
              break;
          }

          if (n == searchlen) {
            if (!count)
              num_files_containing++;

            count++;
          }
        }
      }
    }
  }

  fclose(fptr0);

  if (bTotal && num_files > 1) {
    printf("\n%7d %4d files (%4d total files)\n",
      tcount,num_files_containing,num_files);
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
