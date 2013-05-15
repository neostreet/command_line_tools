#include <stdio.h>

static char *token_types[] = {
  "EQ",
  "NOT",
  "GREATER",
  "LESS",
  "PLUS",
  "MINUS",
  "MULT",
  "DIV",
  "MOD",
  "AND",
  "OR",
  "XOR",
  "RSHIFT",
  "LSHIFT",
  "COMPL",
  "AND_AND",
  "OR_OR",
  "QUERY",
  "COLON",
  "COMMA",
  "OPEN_PAREN",
  "CLOSE_PAREN",
  "EOF",
  "EQ_EQ",
  "NOT_EQ",
  "GREATER_EQ",
  "LESS_EQ",
  "PLUS_EQ",
  "MINUS_EQ",
  "MULT_EQ",
  "DIV_EQ",
  "MOD_EQ",
  "AND_EQ",
  "OR_EQ",
  "XOR_EQ",
  "RSHIFT_EQ",
  "LSHIFT_EQ",
  "HASH",
  "PASTE",
  "OPEN_SQUARE",
  "CLOSE_SQUARE",
  "OPEN_BRACE",
  "CLOSE_BRACE",
  "SEMICOLON",
  "ELLIPSIS",
  "PLUS_PLUS",
  "MINUS_MINUS",
  "DEREF",
  "DOT",
  "SCOPE",
  "DEREF_STAR",
  "DOT_STAR",
  "ATSIGN",
  "NAME",
  "AT_NAME",
  "NUMBER",
  "CHAR",
  "WCHAR",
  "CHAR16",
  "CHAR32",
  "OTHER",
  "STRING",
  "WSTRING",
  "STRING16",
  "STRING32",
  "UTF8STRING",
  "OBJC_STRING",
  "HEADER_NAME",
  "COMMENT",
  "MACRO_ARG",
  "PRAGMA",
  "PRAGMA_EOL",
  "PADDING",
  "PADDING2",
  "KEYWORD"
};
#define NUM_TOKEN_TYPES (sizeof token_types / sizeof (char *))

static char *keywords[] = {
  "STATIC",
  "UNSIGNED",
  "LONG",
  "CONST",
  "EXTERN",
  "REGISTER",
  "TYPEDEF",
  "SHORT",
  "INLINE",
  "VOLATILE",
  "SIGNED",
  "AUTO",
  "RESTRICT",
  "COMPLEX",
  "THREAD",
  "SAT",
  "FRIEND",
  "VIRTUAL",
  "EXPLICIT",
  "EXPORT",
  "MUTABLE",
  "IN",
  "OUT",
  "INOUT",
  "BYCOPY",
  "BYREF",
  "ONEWAY",
  "IMAGINARY",
  "INT",
  "CHAR",
  "FLOAT",
  "DOUBLE",
  "VOID",
  "ENUM",
  "STRUCT",
  "UNION",
  "IF",
  "ELSE",
  "WHILE",
  "DO",
  "FOR",
  "SWITCH",
  "CASE",
  "DEFAULT",
  "BREAK",
  "CONTINUE",
  "RETURN",
  "GOTO",
  "SIZEOF",
  "ASM",
  "TYPEOF",
  "ALIGNOF",
  "ATTRIBUTE",
  "VA_ARG",
  "EXTENSION",
  "IMAGPART",
  "REALPART",
  "LABEL",
  "CHOOSE_EXPR",
  "TYPES_COMPATIBLE_P",
  "DFLOAT32",
  "DFLOAT64",
  "DFLOAT128",
  "FRACT",
  "ACCUM",
  "CXX_COMPAT_WARN",
  "FUNCTION_NAME",
  "PRETTY_FUNCTION_NAME",
  "C99_FUNCTION_NAME",
  "BOOL",
  "WCHAR",
  "CLASS",
  "PUBLIC",
  "PRIVATE",
  "PROTECTED",
  "TEMPLATE",
  "NULL",
  "CATCH",
  "DELETE",
  "FALSE",
  "NAMESPACE",
  "NEW",
  "OFFSETOF",
  "OPERATOR",
  "THIS",
  "THROW",
  "TRUE",
  "TRY",
  "TYPENAME",
  "TYPEID",
  "USING",
  "CHAR16",
  "CHAR32",
  "CONSTCAST",
  "DYNCAST",
  "REINTCAST",
  "STATCAST",
  "HAS_NOTHROW_ASSIGN",
  "HAS_NOTHROW_CONSTRUCTOR",
  "HAS_NOTHROW_COPY",
  "HAS_TRIVIAL_ASSIGN",
  "HAS_TRIVIAL_CONSTRUCTOR",
  "HAS_TRIVIAL_COPY",
  "HAS_TRIVIAL_DESTRUCTOR",
  "HAS_VIRTUAL_DESTRUCTOR",
  "IS_ABSTRACT",
  "IS_BASE_OF",
  "IS_CONVERTIBLE_TO",
  "IS_CLASS",
  "IS_EMPTY",
  "IS_ENUM",
  "IS_POD",
  "IS_POLYMORPHIC",
  "IS_STD_LAYOUT",
  "IS_TRIVIAL",
  "IS_UNION",
  "STATIC_ASSERT",
  "CONSTEXPR",
  "DECLTYPE",
  "AT_ENCODE",
  "AT_END",
  "AT_CLASS",
  "AT_ALIAS",
  "AT_DEFS",
  "AT_PRIVATE",
  "AT_PROTECTED",
  "AT_PUBLIC",
  "AT_PROTOCOL",
  "AT_SELECTOR",
  "AT_THROW",
  "AT_TRY",
  "AT_CATCH",
  "AT_FINALLY",
  "AT_SYNCHRONIZED",
  "AT_INTERFACE",
  "AT_IMPLEMENTATION",
  "ADDR_SPACE_0",
  "ADDR_SPACE_1",
  "ADDR_SPACE_2",
  "ADDR_SPACE_3",
  "ADDR_SPACE_4",
  "ADDR_SPACE_5",
  "ADDR_SPACE_6",
  "ADDR_SPACE_7",
  "ADDR_SPACE_8",
  "ADDR_SPACE_9",
  "ADDR_SPACE_10",
  "ADDR_SPACE_11",
  "ADDR_SPACE_12",
  "ADDR_SPACE_13",
  "ADDR_SPACE_14",
  "ADDR_SPACE_15",
  "MAX"
};
#define NUM_KEYWORDS (sizeof keywords / sizeof (char *))

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: translate_tokens filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int token_type;
  int keyword;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&token_type);

    if ((token_type >= 0 ) && (token_type < NUM_TOKEN_TYPES)) {
      if (token_type == 74) {
        GetLine(fptr,line,&line_len,MAX_LINE_LEN);

        if (feof(fptr))
          break;

        line_no++;

        sscanf(line,"%d",&keyword);

        if ((keyword >= 0) && (keyword < NUM_KEYWORDS))
          printf("%s\n",keywords[keyword]);
        else
          printf("invalid keyword index %d on line %d\n",keyword,line_no);
      }
      else
        printf("%s\n",token_types[token_type]);
    }
    else
      printf("invalid token type index %d on line %d\n",token_type,line_no);
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
