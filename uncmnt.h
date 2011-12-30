#ifndef _UNCMNT_
#define _UNCMNT_

#if defined(HPUX) || defined(SOLARIS)
#if defined(HPUX)
#include <limits.h>
#else
#include <sys/param.h>
#endif
#define MAX_PATH MAXPATHLEN
#else
#define MAX_PATH _MAX_PATH
#endif

char work_filename[MAX_PATH];

#define MAX_LINE_LEN 512
char line[MAX_LINE_LEN];
char uncommented_line[MAX_LINE_LEN];

int get_uncommented_line_info(FILE *fptr,char *line,int *line_len,int maxllen,
  int *comment_start,int *comment_stop,
  int *bInComment,int *bInDoubleQuote,int *curly_brace_depth);
int uncommented_line_blank(char *line,int line_len,
  int comment_start,int comment_stop);
void make_uncommented_line(char *uncommented_line,char *line,int line_len,
  int comment_start,int comment_stop);

#endif  /* _UNCMNT_ */
