#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

static char usage[] =
"usage: fmtime (-noctime) (-suppress_errors) (-size_only) (-terse) (-size_first) (-db_date) filename\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

void get_year_month_day(time_t *timtpt,int *yearpt,int *monthpt,int *daypt);

static char *months[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#define NUM_MONTHS (sizeof months / sizeof (char *))

int main(int argc,char **argv)
{
  int n;
  bool bNoCtime;
  bool bSuppressErrors;
  bool bSizeOnly;
  bool bTerse;
  bool bSizeFirst;
  bool bDbDate;
  FILE *fptr;
  int line_no;
  int linelen;
  struct stat stat_buf;
  char *cpt;
  int year;
  int month;
  int day;

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bNoCtime = false;
  bSuppressErrors = false;
  bSizeOnly = false;
  bTerse = false;
  bSizeFirst = false;
  bDbDate = false;

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"-noctime"))
      bNoCtime = true;
    else if (!strcmp(argv[n],"-suppress_errors"))
      bSuppressErrors = true;
    else if (!strcmp(argv[n],"-size_only"))
      bSizeOnly = true;
    else if (!strcmp(argv[n],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[n],"-size_first"))
      bSizeFirst = true;
    else if (!strcmp(argv[n],"-db_date"))
      bDbDate = true;
    else
      break;
  }

  if (argc - n != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[n],"r")) == NULL) {
    printf("couldn't open %s\n",argv[n]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (stat(line,&stat_buf) != -1) {
      if (bSizeOnly) {
        if (!bTerse)
          printf("%d %s\n",stat_buf.st_size,line);
        else
          printf("%d\n",stat_buf.st_size);
      }
      else if (bNoCtime)
        printf("%10d %s\n",stat_buf.st_mtime,line);
      else if (bDbDate) {
        get_year_month_day(&stat_buf.st_mtime,&year,&month,&day);
        printf("%d-%02d-%02d %s\n",year,month,day,line);
      }
      else {
        cpt = ctime(&stat_buf.st_mtime);
        cpt[strlen(cpt) - 1] = 0;

        if (!bSizeFirst)
          printf("%s %10d ",cpt,stat_buf.st_size);
        else
          printf("%10d %s ",stat_buf.st_size,cpt);

        printf("%s\n",line);
      }
    }
    else {
      if (!bSuppressErrors)
        printf("stat() failed on %s\n",line);
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

void get_year_month_day(time_t *timtpt,int *yearpt,int *monthpt,int *daypt)
{
  int m;
  char *cpt;
  int month;

  cpt = ctime(timtpt);

  sscanf(&cpt[20],"%d",yearpt);

  for (month = 0; month < NUM_MONTHS; month++) {
    for (m = 0; m < 3; m++) {
      if (months[month][m] != cpt[4+m])
        break;
    }

    if (m == 3)
      break;
  }

  if (month == NUM_MONTHS)
    month = 1;
  else
    month++;

  *monthpt = month;

  sscanf(&cpt[8],"%d",daypt);
}
