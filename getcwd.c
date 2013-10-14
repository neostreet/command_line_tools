#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#define MAX_BUF_LEN 4096
static char buf[MAX_BUF_LEN];

int main(int argc,char **argv)
{
  if (getcwd(buf,MAX_BUF_LEN) == NULL) {
    printf("getcwd() failed\n");
    return 1;
  }

  printf("%s\n",buf);

  return 0;
}
