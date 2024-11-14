#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_FILENAME_LEN 512
static char file1[MAX_FILENAME_LEN];
static char file2[MAX_FILENAME_LEN];

static char usage[] =
"usage: fmycmp (-equal) (-verbose) (-quiet) (-size_only) file\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";
static char file_differs[] = "%s differs\n";
static char file_equal[] = "%s equal\n";
static char file_differs_verbose[] = "%s\\%s differs\n";
static char file_equal_verbose[] = "%s\\%s equal\n";

static char save_dir[_MAX_PATH];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int comp_files(char *file1,char *file2,bool bQuiet,bool bSizeOnly);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bEqual;
  bool bVerbose;
  bool bQuiet;
  bool bSizeOnly;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bEqual = false;
  bVerbose = false;
  bQuiet = false;
  bSizeOnly = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-equal"))
      bEqual = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-quiet"))
      bQuiet = true;
    else if (!strcmp(argv[curr_arg],"-size_only"))
      bSizeOnly = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bVerbose)
    getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%s %s",&file1[0],&file2[0]);

    retval = comp_files(file1,file2,bQuiet,bSizeOnly);

    if (!bEqual) {
      if ((retval == 3) || (retval == 7)) {
        if (!bVerbose)
          printf(file_differs,file2);
        else
          printf(file_differs_verbose,save_dir,file2);
      }
    }
    else {
      if ((retval != 3) && (retval != 7)) {
        if (!bVerbose)
          printf(file_equal,file2);
        else
          printf(file_equal_verbose,save_dir,file2);
      }
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

static int comp_files(char *file1,char *file2,bool bQuiet,bool bSizeOnly)
{
  int m;
  int n;
  char *files[2];
  struct stat statbuf[2];
  off_t bytes_to_read;
  char *buf[2];
  int fhndl;
  int bytes_read;

  files[0] = file1;
  files[1] = file2;

  for (n = 0; n < 2; n++) {
    if (stat(files[n],&statbuf[n]) == -1) {
      if (!bQuiet) {
        printf(couldnt_get_status,files[n]);
        return 1;
      }
      else
        return 0;
    }
  }

  if (statbuf[0].st_mode & S_IFDIR)
    return 2;

  if (statbuf[0].st_size != statbuf[1].st_size)
    return 3;
  else if (bSizeOnly)
    return 0;

  bytes_to_read = statbuf[0].st_size;

  for (n = 0; n < 2; n++) {
    if ((buf[n] = (char *)malloc(bytes_to_read)) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_read);

      for (m = 0; m < n; m++)
        free(buf[m]);

      return 4;
    }

    if ((fhndl = open(files[n],O_BINARY | O_RDONLY,0)) == -1) {
      printf(couldnt_open,files[n]);

      for (m = 0; m <= n; m++)
        free(buf[m]);

      return 5;
    }

    bytes_read = read(fhndl,buf[n],bytes_to_read);

    if (bytes_read != bytes_to_read) {
      printf("read of %d bytes failed\n",bytes_to_read);

      for (m = 0; m <= n; m++)
        free(buf[m]);

      return 6;
    }

    close(fhndl);
  }

  for (m = 0; m < bytes_to_read; m++) {
    if (buf[0][m] != buf[1][m])
      break;
  }

  for (n = 0; n < 2; n++)
    free(buf[n]);

  if (m < bytes_to_read)
    return 7;

  return 0;
}
