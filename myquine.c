#include <stdio.h>

#define SPACE         0x20
#define DOUBLE_QUOTE  0x22
#define COMMA         0x2c
#define NEWLINE       0x0a

void myprintstring(char *string,int bDecorate,int bLast);

static char *strings[] = {
  "#include <stdio.h>",
  "",
  "#define SPACE         0x20",
  "#define DOUBLE_QUOTE  0x22",
  "#define COMMA         0x2c",
  "#define NEWLINE       0x0a",
  "",
  "void myprintstring(char *string,int bDecorate,int bLast);",
  "",
  "static char *strings[] = {",
  "};",
  "#define NUM_STRINGS (sizeof strings / sizeof (char *))",
  "",
  "int main(int argc,char **argv)",
  "{",
  "  int m;",
  "  int n;",
  "",
  "  for (n = 0; n < 10; n++)",
  "    myprintstring(strings[n],0,0);",
  "",
  "  for (m = 0; m < NUM_STRINGS; m++)",
  "    myprintstring(strings[m],1,(m == NUM_STRINGS - 1));",
  "",
  "  for ( ; n < NUM_STRINGS; n++)",
  "    myprintstring(strings[n],0,0);",
  "}",
  "",
  "void myprintstring(char *string,int bDecorate,int bLast)",
  "{",
  "  int n;",
  "",
  "  if (bDecorate) {",
  "    putchar(SPACE);",
  "    putchar(SPACE);",
  "    putchar(DOUBLE_QUOTE);",
  "  }",
  "",
  "  for (n = 0; (string[n]); n++)",
  "    putchar(string[n]);",
  "",
  "  if (bDecorate) {",
  "    putchar(DOUBLE_QUOTE);",
  "",
  "    if (!bLast)",
  "      putchar(COMMA);",
  "  }",
  "",
  "  putchar(NEWLINE);",
  "}"
};
#define NUM_STRINGS (sizeof strings / sizeof (char *))

int main(int argc,char **argv)
{
  int m;
  int n;

  for (n = 0; n < 10; n++)
    myprintstring(strings[n],0,0);

  for (m = 0; m < NUM_STRINGS; m++)
    myprintstring(strings[m],1,(m == NUM_STRINGS - 1));

  for ( ; n < NUM_STRINGS; n++)
    myprintstring(strings[n],0,0);
}

void myprintstring(char *string,int bDecorate,int bLast)
{
  int n;

  if (bDecorate) {
    putchar(SPACE);
    putchar(SPACE);
    putchar(DOUBLE_QUOTE);
  }

  for (n = 0; (string[n]); n++)
    putchar(string[n]);

  if (bDecorate) {
    putchar(DOUBLE_QUOTE);

    if (!bLast)
      putchar(COMMA);
  }

  putchar(NEWLINE);
}
