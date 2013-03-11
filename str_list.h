#ifndef _STR_LIST_
#define _STR_LIST_

struct str_list_elem {
  char *str;
  struct str_list_elem *next_elem;
  struct str_list_elem *prev_elem;
};

struct str_list {
  int num_elems;
  struct str_list_elem *first_elem;
  struct str_list_elem *last_elem;
};

struct info_list_elem {
  char *str;
  int int1;
  int int2;
  struct info_list_elem *next_elem;
  struct info_list_elem *prev_elem;
};

struct info_list {
  int num_elems;
  struct info_list_elem *first_elem;
  struct info_list_elem *last_elem;
};

struct info2_list_elem {
  char *str;
  int int1;
  int int2;
  int int3;
  void *void_ptr;
  struct info2_list_elem *next_elem;
  struct info2_list_elem *prev_elem;
};

struct info2_list {
  int num_elems;
  struct info2_list_elem *first_elem;
  struct info2_list_elem *last_elem;
};

bool add_str_list_elem(struct str_list *str_list,char *str,
  int bNoDuplicates);
bool add_info_list_elem(struct info_list *info_list,char *str,
  int int1,int int2,int bNoDuplicates);
bool add_info2_list_elem(struct info2_list *info2_list,char *str,
  int int1,int int2,int int3,void *void_ptr);
int get_str_list_elem(struct str_list *str_list,int ix,
  struct str_list_elem **str_list_elem);
char *get_str_list_elem_str(struct str_list *str_list,int ix);
int get_info_list_elem(struct info_list *info_list,int ix,
  struct info_list_elem **info_list_elem);
int get_info2_list_elem(struct info2_list *info2_list,int ix,
  struct info2_list_elem **info2_list_elem);
bool member_of_str_list(struct str_list *str_list,char *str,int *ix);
bool member_of_str_list_from_ix(struct str_list *str_list,int in_ix,
  char *str,int *ix);
bool member_of_info_list(struct info_list *info_list,char *str,int *ix);
bool member_of_info_list_from_ix(struct info_list *info_list,int in_ix,
  char *str,int *ix);
bool member_of_info2_list(struct info2_list *info2_list,int int2,
  void *void_ptr,int *ix);
bool member_of_info2_list_from_ix(struct info2_list *info2_list,
  int in_ix,int int2,void *void_ptr,int *ix);
void remove_str_list_elem(struct str_list *str_list,int ix);
void print_str_list(struct str_list *str_list);
void print_info2_list(struct info2_list *info2_list);
void print_info_list(struct info_list *info_list);
void free_str_list(struct str_list *str_list);
void free_info_list(struct info_list *info_list);
void free_info2_list(struct info2_list *info2_list);

#endif  /* _STR_LIST_ */
