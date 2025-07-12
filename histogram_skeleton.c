#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_elo_diff (-terse) (-neg_only) (-pos_only) (-zero_only) (-date) (-anchor)\n"
"  (-opponent_elo_first) (-opponent_elo_geval) (-elo_diff_leval) (-elo_diff_geval)\n"
"  (-is_neg) (-is_pos) (-filename_only) (-i_am_white) (-i_am_black)\n"
"  (-is_win) (-is_draw) (-is_loss) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

#define RESULT_EMPTY 0
#define RESULT_WIN   1
#define RESULT_DRAW  2
#define RESULT_LOSS  3

static char white[] = "White";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "Black";
#define BLACK_LEN (sizeof (black) - 1)
static char result_str[] = "Result \"";
#define RESULT_STR_LEN (sizeof (result_str) - 1)
static char white_wins[] = "1-0";
#define WHITE_WINS_LEN (sizeof (white_wins) - 1)
static char black_wins[] = "0-1";
#define BLACK_WINS_LEN (sizeof (black_wins) - 1)
static char white_elo[] = "WhiteElo \"";
#define WHITE_ELO_LEN (sizeof (white_elo) - 1)
static char black_elo[] = "BlackElo \"";
#define BLACK_ELO_LEN (sizeof (black_elo) - 1)
static char utcdate[] = "UTCDate";
#define UTCDATE_LEN (sizeof (utcdate) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static void get_date(char *date,char *line);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bNegOnly;
  bool bPosOnly;
  bool bZeroOnly;
  bool bDate;
  bool bAnchor;
  bool bOpponentEloFirst;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  int ix;
  int elo;
  int opponent_elo;
  int elo_diff;
  int opponent_elo_geval;
  bool bHaveLEVal;
  int elo_diff_leval;
  bool bHaveGEVal;
  int elo_diff_geval;
  bool bIsNeg;
  bool bIsPos;
  bool bFilenameOnly;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bIsWin;
  bool bIsDraw;
  bool bIsLoss;
  int result;

  if ((argc < 3) || (argc > 21)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bNegOnly = false;
  bPosOnly = false;
  bZeroOnly = false;
  bDate = false;
  bAnchor = false;
  bOpponentEloFirst = false;
  opponent_elo_geval = -1;
  bHaveLEVal = false;
  bHaveGEVal = false;
  bIsNeg = false;
  bIsPos = false;
  bFilenameOnly = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bIsWin = false;
  bIsDraw = false;
  bIsLoss = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-neg_only"))
      bNegOnly = true;
    else if (!strcmp(argv[curr_arg],"-pos_only"))
      bPosOnly = true;
    else if (!strcmp(argv[curr_arg],"-zero_only"))
      bZeroOnly = true;
    else if (!strcmp(argv[curr_arg],"-date"))
      bDate = true;
    else if (!strcmp(argv[curr_arg],"-anchor"))
      bAnchor = true;
    else if (!strcmp(argv[curr_arg],"-opponent_elo_first"))
      bOpponentEloFirst = true;
    else if (!strncmp(argv[curr_arg],"-opponent_elo_ge",16))
      sscanf(&argv[curr_arg][16],"%d",&opponent_elo_geval);
    else if (!strncmp(argv[curr_arg],"-elo_diff_le",12)) {
      sscanf(&argv[curr_arg][12],"%d",&elo_diff_leval);
      bHaveLEVal = true;
    }
    else if (!strncmp(argv[curr_arg],"-elo_diff_ge",12)) {
      sscanf(&argv[curr_arg][12],"%d",&elo_diff_geval);
      bHaveGEVal = true;
    }
    else if (!strcmp(argv[curr_arg],"-is_neg"))
      bIsNeg = true;
    else if (!strcmp(argv[curr_arg],"-is_pos"))
      bIsPos = true;
    else if (!strcmp(argv[curr_arg],"-filename_only"))
      bFilenameOnly = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-is_win"))
      bIsWin = true;
    else if (!strcmp(argv[curr_arg],"-is_draw"))
      bIsDraw = true;
    else if (!strcmp(argv[curr_arg],"-is_loss"))
      bIsLoss = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bNegOnly && bPosOnly) {
    printf("can't specify both -neg_only and -pos_only\n");
    return 3;
  }

  if (bIsPos && bIsNeg) {
    printf("can't specify both -is_pos and -is_neg\n");
    return 4;
  }

  if (bIAmWhite && bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 5;
  }

  if ((bIsWin && bIsDraw) || (bIsWin && bIsLoss) || (bIsDraw && bIsLoss)) {
    printf("can only specify one of -is_win, -is_draw, and -is_loss\n");
    return 6;
  }

  player_name_ix = curr_arg;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg + 1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg + 1]);
    return 7;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bDate)
          get_date(date,line);
      }
      else if (Contains(true,
        line,line_len,
        white_elo,WHITE_ELO_LEN,
        &ix)) {

        if (bPlayerIsWhite)
          sscanf(&line[ix + WHITE_ELO_LEN],"%d",&elo);
        else
          sscanf(&line[ix + WHITE_ELO_LEN],"%d",&opponent_elo);
      }
      else if (Contains(true,
        line,line_len,
        black_elo,BLACK_ELO_LEN,
        &ix)) {

        if (!bPlayerIsWhite)
          sscanf(&line[ix + BLACK_ELO_LEN],"%d",&elo);
        else
          sscanf(&line[ix + BLACK_ELO_LEN],"%d",&opponent_elo);

        if (opponent_elo < opponent_elo_geval)
          break;

        elo_diff = elo - opponent_elo;

        if (bZeroOnly && elo_diff)
          break;

        if (bNegOnly && (elo_diff >= 0))
          break;

        if (bPosOnly && (elo_diff <= 0))
          break;

        if (bHaveLEVal && (elo_diff > elo_diff_leval))
          break;

        if (bHaveGEVal && (elo_diff < elo_diff_geval))
          break;

        if (bIAmWhite && !bPlayerIsWhite)
          break;

        if (bIAmBlack && bPlayerIsWhite)
          break;

        if (bIsWin && (result != RESULT_WIN))
          break;

        if (bIsDraw && (result != RESULT_DRAW))
          break;

        if (bIsLoss && (result != RESULT_LOSS))
          break;

        if (bFilenameOnly) {
          printf("%s\n",filename);
        }
        else if (!bTerse) {
          if (!bDate) {
            if (!bOpponentEloFirst) {
              if (bIsNeg) {
                printf("%d %s%d (%d %d) %s\n",
                  (elo_diff < 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename);
              }
              else if (bIsPos) {
                printf("%d %s%d (%d %d) %s\n",
                  (elo_diff > 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename);
              }
              else {
                printf("%s%d (%d %d) %s\n",
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename);
              }
            }
            else {
              if (bIsNeg) {
                printf("%d %s%d (%d %d) %s\n",
                  (elo_diff < 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename);
              }
              else if (bIsPos) {
                printf("%d %s%d (%d %d) %s\n",
                  (elo_diff > 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename);
              }
              else {
                printf("%s%d (%d %d) %s\n",
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename);
              }
            }
          }
          else {
            if (!bOpponentEloFirst) {
              if (bIsNeg) {
                printf("%d %s%d (%d %d) %s %s\n",
                  (elo_diff < 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename,date);
              }
              else if (bIsPos) {
                printf("%d %s%d (%d %d) %s %s\n",
                  (elo_diff > 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename,date);
              }
              else {
                printf("%s%d (%d %d) %s %s\n",
                  (bAnchor ? "# " : ""),
                  elo_diff,elo,opponent_elo,filename,date);
              }
            }
            else {
              if (bIsNeg) {
                printf("%d %s%d (%d %d) %s %s\n",
                  (elo_diff < 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename,date);
              }
              else if (bIsPos) {
                printf("%d %s%d (%d %d) %s %s\n",
                  (elo_diff > 0 ? 1 : 0),
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename,date);
              }
              else {
                printf("%s%d (%d %d) %s %s\n",
                  (bAnchor ? "# " : ""),
                  elo_diff,opponent_elo,elo,filename,date);
              }
            }
          }
        }
        else
          printf("%d\n",elo_diff);

        break;
      }
      else if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white,WHITE_LEN,
          &ix)) {

          bPlayerIsWhite = true;
        }
        else if (Contains(true,
          line,line_len,
          black,BLACK_LEN,
          &ix)) {

          bPlayerIsWhite = false;
        }
        else {
          printf("%s: couldn't determine whether %s is white or black\n",
            filename,argv[player_name_ix]);
          return 8;
        }
      }
      else if (Contains(true,
        line,line_len,
        result_str,RESULT_STR_LEN,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white_wins,WHITE_WINS_LEN,
          &ix)) {

          if (bPlayerIsWhite)
            result = RESULT_WIN;
          else
            result = RESULT_LOSS;
        }
        else if (Contains(true,
          line,line_len,
          black_wins,BLACK_WINS_LEN,
          &ix)) {

          if (!bPlayerIsWhite)
            result = RESULT_WIN;
          else
            result = RESULT_LOSS;
        }
        else
            result = RESULT_DRAW;
      }
    }

    fclose(fptr);
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

static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    return false;

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
      return true;
    }
  }

  return false;
}

static void get_date(char *date,char *line)
{
  strncpy(date,&line[10],10);
  date[4] = '-';
  date[7] = '-';
}
