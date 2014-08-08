#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define SECONDS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define SECONDS_PER_HOUR (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)
#define SECONDS_PER_DAY (SECONDS_PER_HOUR * HOURS_PER_DAY)

static char usage[] = "usage: secdiff (-ge_secs) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int get_time(char *line,int line_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bGeSecs;
  int ge_secs;
  FILE *fptr;
  int line_len;
  int line_no;
  int old_time;
  int new_time;
  int diff;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bGeSecs = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-ge_",4)) {
      bGeSecs = true;
      sscanf(&argv[curr_arg][4],"%d",&ge_secs);
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  line_no = 0;
  old_time = -1;


  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    new_time = get_time(line,line_len);

    if (new_time != -1) {
       if (old_time != -1) {

         if (new_time < old_time)
           new_time += SECONDS_PER_DAY;

         diff = new_time - old_time;

         if (!bGeSecs || (diff >= ge_secs))
           printf("%5d %s\n",new_time - old_time,line);
       }

      old_time = new_time;
    }
  }

  fclose(fptr);

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

int get_time(char *line,int line_len)
{
  int n;
  char buf[3];
  int hours;
  int mins;
  int secs;
  int time;

  if (line_len < 20)
    return -1;

  if ((line[13] != ':') || (line[16] != ':') || (line[19] != '.'))
    return -1;

  buf[2] = 0;

  for (n = 0; n < 2; n++)
    buf[n] = line[11+n];

  sscanf(buf,"%d",&hours);

  for (n = 0; n < 2; n++)
    buf[n] = line[14+n];

  sscanf(buf,"%d",&mins);

  for (n = 0; n < 2; n++)
    buf[n] = line[17+n];

  sscanf(buf,"%d",&secs);

  time = hours * SECONDS_PER_HOUR + mins * SECONDS_PER_MINUTE + secs;

  return time;
}
