#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char temp_filename[] = "rename_to_save_name.tmp";

#define MAX_LINE_LEN 2048
static char line1[MAX_LINE_LEN];
static char line2[MAX_LINE_LEN];

#define MAX_BUF_LEN 4096
static char buf[MAX_BUF_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

static char couldnt_open[] = "couldn't open %s\n";
static char unexpected_eof[] = "unexpected eof in %s\n";

int get_last_save_number(char *line,int line_len,int *last_save_number_ptr);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line1_len;
  int line2_len;
  int retval;
  int last_save_number;

  if (argc < 2) {
    printf("usage: rename_to_save_name filename (filename ...)\n");
    return 1;
  }

  for (n = 1; n < argc; n++) {
    sprintf(buf,"ls -1t %s* > %s",argv[n],temp_filename);
    system(buf);

    if ((fptr = fopen(temp_filename,"r")) == NULL) {
      printf(couldnt_open,temp_filename);
      continue;
    }

    GetLine(fptr,line1,&line1_len,MAX_LINE_LEN);

    if (feof(fptr)) {
      fclose(fptr);
      remove(temp_filename);
      printf(unexpected_eof,temp_filename);
      continue;
    }

    GetLine(fptr,line2,&line2_len,MAX_LINE_LEN);

    if (feof(fptr)) {
      // there aren't any save files yet; create the first one
      sprintf(line2,"%s.s1",line1);
      rename(line1,line2);

      fclose(fptr);
      remove(temp_filename);

      continue;
    }

    fclose(fptr);
    remove(temp_filename);

    if (strncmp(line1,line2,strlen(line1))) {
      printf("can't find earlier version of %s\n",line1);
      continue;
    }

    retval = get_last_save_number(line2,line2_len,&last_save_number);

    if (retval) {
      printf("get_last_save_number failed on %s: %d\n",line2,retval);
      continue;
    }

    sprintf(line2,"%s.s%d",line1,last_save_number+1);
    rename(line1,line2);
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

int get_last_save_number(char *line,int line_len,int *last_save_number_ptr)
{
  int n;

  for (n = line_len - 1; (n >= 0); n--) {
    if (line[n] == '.')
      break;
  }

  if (n < 0)
    return 1;

  if (line[n+1] != 's')
    return 2;

  sscanf(&line[n+2],"%d",last_save_number_ptr);

  return 0;
}
