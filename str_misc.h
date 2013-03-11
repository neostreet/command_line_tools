#define LINES_PER_TRANSLATION 2

struct straight_translation {
 char *old_text;
 char *new_text;
};

bool find_substring(char *buf,int *ixpt,char *substring,bool bCaseSens,
  bool bWord);
bool is_word_delim(int chara);
bool get_word(char *buf,int *ixpt,char *word,int max_word_len,int *word_len,
  bool bSkipToWhiteSpace);
bool my_strcmpi(char *str1,char *str2);
bool read_straight_translations(char *filename,char *line,int max_line_len,
  struct straight_translation **straight_transl_ptr,
  int *num_straight_transl_ptr);
bool do_straight_translations(char *line,int line_len,int max_line_len,
  int comment_start,int comment_stop,
  struct straight_translation *straight_translations,
  int num_straight_translations);
