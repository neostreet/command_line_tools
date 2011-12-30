#include <stdio.h>
#include <string.h>

char usage[] = "usage: mycksum (-binary) (-detail) (-titleTITLE) (-terse)\n"
"  filename (filename ...)\n";

char buf[256];

int main(int argc,char **argv)
{
  int n;
  int bBinary;
  int bDetail;
  int title_arg;
  char *style;
  FILE *fptr;
  int chara;
  int cksum;
  int bytes;
  int tot_cksum;
  int tot_files;
  int tot_bytes;
  int bStdin;
  int bTerse;
  char *curr_file;

  bBinary = 0;
  bDetail = 0;
  title_arg = 0;
  bStdin = 0;
  bTerse = 0;

  if (argc < 2)
    bStdin = 1;
  else {
    for (n = 1; n < argc; n++) {
      if (!strcmp(argv[n],"-binary"))
        bBinary = 1;
      else if (!strcmp(argv[n],"-detail"))
        bDetail = 1;
      else if (!strncmp(argv[n],"-title",6))
        title_arg = n;
      else if (!strcmp(argv[n],"-terse"))
        bTerse = 1;
      else
        break;
    }

    if (n == argc)
      bStdin = 1;
  }

  if (bDetail)
    tot_cksum = 0;
  else
    cksum = 0;

  tot_files = 0;
  tot_bytes = 0;

  for ( ; ; ) {
    if (!bStdin) {
      if (n == argc)
        break;

      curr_file = argv[n];
    }
    else {
      fscanf(stdin,"%s",buf);
      curr_file = buf;

      if (feof(stdin))
        break;
    }

    if (bBinary)
      style = "rb";
    else
      style = "r";

    if ((fptr = fopen(curr_file,style)) == NULL) {
#ifdef UNDEF
      printf("couldn't open %s\n",curr_file);
#endif

      if (!bStdin) {
        n++;

        if (n == argc)
          break;
      }

      continue;
    }

    tot_files++;

    if (bDetail) {
      cksum = 0;
      bytes = 0;
    }

    for ( ; ; ) {
      chara = fgetc(fptr);

      if (feof(fptr))
        break;

      cksum += chara;

      if (bDetail)
        bytes++;
      else
        tot_bytes++;
    }

    if (bDetail)
      tot_bytes += bytes;

    fclose(fptr);

    if (bDetail) {
      printf("%08x %10d %s\n",cksum,bytes,curr_file);
      tot_cksum += cksum;
    }

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  if (bDetail)
    printf("\n%08x %10d (%d files)\n",
      tot_cksum,tot_bytes,tot_files);
  else if (title_arg)
    printf("\n%08x %10d %s (%d files)\n",
      cksum,tot_bytes,&argv[title_arg][6],tot_files);
  else if (bTerse)
    printf("%08x\n",cksum);
  else
    printf("%08x %10d (%d files)\n",
      cksum,tot_bytes,tot_files);

  return 1;
}
