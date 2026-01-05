#include <stdio.h>
#include <string.h>

#ifndef WIN32
#define strcmpi strcmp
#endif

#define FALSE 0
#define TRUE  1

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 65536
static char in_line[MAX_LINE_LEN];
static char out_line[MAX_LINE_LEN];

#define MAX_SEARCH_STRING_LENGTH   8192
#define MAX_REPLACE_STRING_LENGTH  8192
#define MAX_REPLACES 50

#define BEGINNING_OF_LINE           0
#define END_OF_LINE                 1
#define BEGINNING_OF_FILE           2
#define END_OF_FILE                 3
#define NUM_SPECIAL_SEARCH_STRINGS  4

static char usage[] = "\
usage: srchrepl (-c) (-bak) srchrepl_file file\n";

static char couldnt_open[] = "couldn't open %s\n";
static char new_line_too_long[] = "new line too long\n";
static char tempname[] = "srchrepl.tmp";

struct search_and_replace {
  int special_search;
  char search_string[MAX_SEARCH_STRING_LENGTH];
  char replace_string[MAX_REPLACE_STRING_LENGTH];
};

static struct search_and_replace replaces[MAX_REPLACES];
static int num_replaces;

static char *special_search_strings[] = {
  "linebegin",
  "lineend",
  "filebegin",
  "fileend"
};

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int GetSearchAndReplace(int bCaseSens,char *line);
int DoReplaces(int bCaseSens,char *in_line,int bFirstLine,
  char *out_line);
int Contains(int bCaseSens,char *line,char *string,int *index);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bCaseSens;
  int bBak;
  FILE *fptr0;
  int filename_len;
  FILE *fptr1;
  FILE *fptr2;
  char bakfilename[128];
  int linel;
  int bFirstLine;
  int tot_repls;

  bCaseSens = FALSE;
  bBak = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmpi(argv[curr_arg],"-c"))
      bCaseSens = TRUE;
    else if (!strcmpi(argv[curr_arg],"-bak"))
      bBak = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr1 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 2;
  }

  num_replaces = 0;

  for ( ; ; ) {
    GetLine(fptr1,in_line,&linel,MAX_LINE_LEN);

    if (feof(fptr1))
      break;

    if (num_replaces == MAX_REPLACES) {
      printf("MAX_REPLACES exceeded\n");
      break;
    }

    if (!GetSearchAndReplace(bCaseSens,in_line)) {
      printf("faulty specification: %s\n",in_line);
      printf("both strings must be surrounded by double quotes\n");
      return 3;
    }
  }

  fclose(fptr1);
  curr_arg++;

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr1 = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      return 5;
    }

    remove(tempname);

    if ((fptr2 = fopen(tempname,"w")) == NULL) {
      fclose(fptr1);
      printf(couldnt_open,tempname);
      return 6;
    }

    tot_repls = 0;

    if (bBak) {
      strcpy(bakfilename,filename);

      for (n = strlen(bakfilename) - 1; n >= 0; n--) {
        if (bakfilename[n] == '.')
          break;
      }

      strcpy(&bakfilename[n],".bak");
    }

    bFirstLine = TRUE;

    for ( ; ; ) {
      GetLine(fptr1,in_line,&linel,MAX_LINE_LEN);

      if (feof(fptr1))
        break;

      tot_repls += DoReplaces(bCaseSens,in_line,bFirstLine,out_line);

      fprintf(fptr2,"%s\n",out_line);

      if (bFirstLine)
        bFirstLine = FALSE;
    }

    fclose(fptr1);
    fclose(fptr2);

    if (!tot_repls)
      remove(tempname);
    else {
      printf("%s: %d replacements\n",filename,tot_repls);

      if (bBak) {
        remove(bakfilename);
        rename(filename,bakfilename);
      }
      else
        remove(filename);

      rename(tempname,filename);
    }
  }

  fclose(fptr0);

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

int GetSearchAndReplace(int bCaseSens,char *line)
{
  int m;
  int n;
  char chara;

  for (n = 0; (chara = line[n]); n++)
    if (chara == '"')
      break;

  if (!chara)
    return FALSE;

  n++;
  m = 0;

  for ( ; (chara = line[n]); n++) {
    if (!bCaseSens) {
      if ((chara >= 'A') && (chara <= 'Z'))
        chara += 'a' - 'A';
    }

    if (chara == '"')
      break;
    else {
      if (m < MAX_SEARCH_STRING_LENGTH - 1)
        replaces[num_replaces].search_string[m++] = chara;
    }
  }

  if (!chara)
    return FALSE;

  replaces[num_replaces].search_string[m] = 0;
  n++;

  for ( ; (chara = line[n]); n++)
    if (chara == '"')
      break;

  if (!chara)
    return FALSE;

  n++;
  m = 0;

  for ( ; (chara = line[n]); n++) {
    if (chara == '"')
      break;
    else {
      if (m < MAX_REPLACE_STRING_LENGTH - 1)
        replaces[num_replaces].replace_string[m++] = chara;
    }
  }

  if (!chara)
    return FALSE;

  replaces[num_replaces].replace_string[m] = 0;
  replaces[num_replaces].special_search = -1;


  for (n = 0; n < NUM_SPECIAL_SEARCH_STRINGS; n++) {
    if (!strcmpi(replaces[num_replaces].search_string,
      special_search_strings[n])) {
      replaces[num_replaces].special_search = n;
      break;
    }
  }

  num_replaces++;

  return TRUE;
}

int DoReplaces(int bCaseSens,char *in_line,int bFirstLine,
  char *out_line)
{
  int m;
  int n;
  int p;
  int old_len;
  int srch_len;
  int repl_len;
  int new_len;
  int repls;
  int index;
  int tot_repls;

  tot_repls = 0;

  for (n = 0; n < num_replaces; n++) {
    old_len = strlen(in_line);
    repls = 0;

    for ( ; ; ) {
      if (Contains(bCaseSens,in_line,replaces[n].search_string,&index)) {
        srch_len = strlen(replaces[n].search_string);
        repl_len = strlen(replaces[n].replace_string);

        new_len = old_len - srch_len + repl_len;

        if (new_len > MAX_LINE_LEN)
          printf(new_line_too_long);
        else {
          for (m = 0; m < index; m++)
            out_line[m] = in_line[m];

          for (p = 0; p < repl_len; p++)
            out_line[m++] = replaces[n].replace_string[p];

          for (p = index + srch_len; (in_line[p]); p++)
            out_line[m++] = in_line[p];

          out_line[m] = 0;
          repls++;
        }

        strcpy(in_line,out_line);
      }
      else {
        strcpy(out_line,in_line);
        break;
      }
    }

    tot_repls += repls;

    if (repls && (n < num_replaces - 1))
      strcpy(in_line,out_line);
  }

  return tot_repls;
}

int Contains(int bCaseSens,char *line,char *string,int *index)
{
  int m;
  int n;
  int line_len;
  int string_len;
  int tries;
  char chara;

  line_len = strlen(line);
  string_len = strlen(string);
  tries = line_len - string_len + 1;

  if (tries <= 0)
    return FALSE;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < string_len; n++) {
      chara = line[m + n];

      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          chara += 'a' - 'A';
      }

      if (chara != string[n])
        break;
    }

    if (n == string_len) {
      *index = m;
      return TRUE;
    }
  }

  return FALSE;
}
