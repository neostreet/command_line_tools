#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "str_list.h"

bool add_str_list_elem(struct str_list *str_list,char *str,
  int bNoDuplicates)
{
  int ix;
  int size;
  struct str_list_elem *work_elem;
  char *cpt;

  if (bNoDuplicates) {
    if (member_of_str_list(str_list,str,&ix))
      return true;
  }

  size = strlen(str) + 1 + sizeof (struct str_list_elem);

  if ((work_elem = (struct str_list_elem *)malloc(size)) == NULL)
    return false;

  cpt = (char *)work_elem + sizeof (struct str_list_elem);
  work_elem->str = cpt;
  strcpy(cpt,str);
  work_elem->next_elem = NULL;

  if (!str_list->num_elems) {
    work_elem->prev_elem = NULL;
    str_list->first_elem = work_elem;
  }
  else {
    work_elem->prev_elem = str_list->last_elem;
    str_list->last_elem->next_elem = work_elem;
  }

  str_list->last_elem = work_elem;
  str_list->num_elems++;

  return true;
}

bool add_info_list_elem(struct info_list *info_list,char *str,
  int int1,int int2,int int3,int int4,int bNoDuplicates)
{
  int ix;
  int size;
  struct info_list_elem *work_elem;
  char *cpt;

  if (bNoDuplicates) {
    if (member_of_info_list(info_list,str,&ix))
      return true;
  }

  size = strlen(str) + 1 + sizeof (struct info_list_elem);

  if ((work_elem = (struct info_list_elem *)malloc(size)) == NULL)
    return false;

  cpt = (char *)work_elem + sizeof (struct info_list_elem);
  work_elem->str = cpt;
  strcpy(cpt,str);
  work_elem->int1 = int1;
  work_elem->int2 = int2;
  work_elem->int3 = int3;
  work_elem->int4 = int4;
  work_elem->next_elem = NULL;

  if (!info_list->num_elems) {
    work_elem->prev_elem = NULL;
    info_list->first_elem = work_elem;
  }
  else {
    work_elem->prev_elem = info_list->last_elem;
    info_list->last_elem->next_elem = work_elem;
  }

  info_list->last_elem = work_elem;
  info_list->num_elems++;

  return true;
}

bool add_info2_list_elem(struct info2_list *info2_list,char *str,
  int int1,int int2,int int3,void *void_ptr)
{
  int size;
  struct info2_list_elem *work_elem;
  char *cpt;

  size = strlen(str) + 1 + sizeof (struct info2_list_elem);

  if ((work_elem = (struct info2_list_elem *)malloc(size)) == NULL)
    return false;

  cpt = (char *)work_elem + sizeof (struct info2_list_elem);
  work_elem->str = cpt;
  strcpy(cpt,str);
  work_elem->int1 = int1;
  work_elem->int2 = int2;
  work_elem->int3 = int3;
  work_elem->void_ptr = void_ptr;
  work_elem->next_elem = NULL;

  if (!info2_list->num_elems) {
    work_elem->prev_elem = NULL;
    info2_list->first_elem = work_elem;
  }
  else {
    work_elem->prev_elem = info2_list->last_elem;
    info2_list->last_elem->next_elem = work_elem;
  }

  info2_list->last_elem = work_elem;
  info2_list->num_elems++;

  return true;
}

int get_str_list_elem(struct str_list *str_list,int ix,
  struct str_list_elem **str_list_elem)
{
  int n;
  struct str_list_elem *work_elem;

  if (ix >= str_list->num_elems)
    return false;

  work_elem = str_list->first_elem;

  for (n = 0; n < ix; n++)
    work_elem = work_elem->next_elem;

  *str_list_elem = work_elem;

  return true;
}

char *get_str_list_elem_str(struct str_list *str_list,int ix)
{
  int n;
  struct str_list_elem *work_elem;

  if (ix >= str_list->num_elems)
    return NULL;

  work_elem = str_list->first_elem;

  for (n = 0; n < ix; n++)
    work_elem = work_elem->next_elem;

  return work_elem->str;
}

int get_info_list_elem(struct info_list *info_list,int ix,
  struct info_list_elem **info_list_elem)
{
  int n;
  struct info_list_elem *work_elem;

  if (ix >= info_list->num_elems)
    return false;

  work_elem = info_list->first_elem;

  for (n = 0; n < ix; n++)
    work_elem = work_elem->next_elem;

  *info_list_elem = work_elem;

  return true;
}

int get_info2_list_elem(struct info2_list *info2_list,int ix,
  struct info2_list_elem **info2_list_elem)
{
  int n;
  struct info2_list_elem *work_elem;

  if (ix >= info2_list->num_elems)
    return false;

  work_elem = info2_list->first_elem;

  for (n = 0; n < ix; n++)
    work_elem = work_elem->next_elem;

  *info2_list_elem = work_elem;

  return true;
}

bool member_of_str_list(struct str_list *str_list,char *str,int *ix)
{
  int n;
  struct str_list_elem *work_elem;

  work_elem = str_list->first_elem;

  for (n = 0; n < str_list->num_elems; n++) {
    if (!strcmp(work_elem->str,str)) {
      *ix = n;

      return true;
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

bool member_of_str_list_from_ix(struct str_list *str_list,int in_ix,
  char *str,int *ix)
{
  int n;
  struct str_list_elem *work_elem;

  work_elem = str_list->first_elem;

  for (n = 0; n < str_list->num_elems; n++) {
    if (n >= in_ix) {
      if (!strcmp(work_elem->str,str)) {
        *ix = n;

        return true;
      }
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

bool member_of_info_list(struct info_list *info_list,char *str,int *ix)
{
  int n;
  struct info_list_elem *work_elem;

  work_elem = info_list->first_elem;

  for (n = 0; n < info_list->num_elems; n++) {
    if (!strcmp(work_elem->str,str)) {
      *ix = n;

      return true;
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

bool member_of_info_list_from_ix(struct info_list *info_list,int in_ix,
  char *str,int *ix)
{
  int n;
  struct info_list_elem *work_elem;

  work_elem = info_list->first_elem;

  for (n = 0; n < info_list->num_elems; n++) {
    if (n >= in_ix) {
      if (!strcmp(work_elem->str,str)) {
        *ix = n;

        return true;
      }
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

bool member_of_info2_list(struct info2_list *info2_list,int int2,
  void *void_ptr,int *ix)
{
  int n;
  struct info2_list_elem *work_elem;

  work_elem = info2_list->first_elem;

  for (n = 0; n < info2_list->num_elems; n++) {
    if ((work_elem->int2 == int2) && (work_elem->void_ptr == void_ptr)) {
      *ix = n;

      return true;
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

bool member_of_info2_list_from_ix(struct info2_list *info2_list,
  int in_ix,int int2,void *void_ptr,int *ix)
{
  int n;
  struct info2_list_elem *work_elem;

  work_elem = info2_list->first_elem;

  for (n = 0; n < info2_list->num_elems; n++) {
    if (n >= in_ix) {
      if ((work_elem->int2 == int2) && (work_elem->void_ptr == void_ptr)) {
        *ix = n;

        return true;
      }
    }

    work_elem = work_elem->next_elem;
  }

  return false;
}

void remove_str_list_elem(struct str_list *str_list,int ix)
{
  int n;
  struct str_list_elem *prev_elem;
  struct str_list_elem *work_elem;

  if (!str_list->num_elems)
    return;

  if (!ix) {
    str_list->first_elem = str_list->first_elem->next_elem;
    str_list->num_elems--;

    return;
  }

  if (ix == str_list->num_elems - 1) {
    str_list->last_elem = str_list->last_elem->prev_elem;
    str_list->last_elem->next_elem = NULL;
    str_list->num_elems--;

    return;
  }

  work_elem = str_list->first_elem;

  for (n = 0; n < ix; n++) {
    prev_elem = work_elem;
    work_elem = work_elem->next_elem;
  }

  prev_elem->next_elem = work_elem->next_elem;

  if (work_elem->next_elem)
    work_elem->next_elem->prev_elem = prev_elem;

  str_list->num_elems--;
}

void print_str_list(struct str_list *str_list)
{
  int n;
  struct str_list_elem *work_elem;

  work_elem = str_list->first_elem;

  for (n = 0; n < str_list->num_elems; n++) {
    printf("%s\n",work_elem->str);
    work_elem = work_elem->next_elem;
  }
}

void print_info_list(struct info_list *info_list)
{
  int n;
  struct info_list_elem *work_elem;

  work_elem = info_list->first_elem;

  for (n = 0; n < info_list->num_elems; n++) {
    printf("%s %d %d\n",work_elem->str,
      work_elem->int1,work_elem->int2);
    work_elem = work_elem->next_elem;
  }
}

void print_info2_list(struct info2_list *info2_list)
{
  int n;
  struct info2_list_elem *work_elem;

  work_elem = info2_list->first_elem;

  for (n = 0; n < info2_list->num_elems; n++) {
    printf("%s %d %d %d %p\n",work_elem->str,
      work_elem->int1,work_elem->int2,work_elem->int3,
      work_elem->void_ptr);
    work_elem = work_elem->next_elem;
  }
}

void free_str_list(struct str_list *str_list)
{
  int n;
  struct str_list_elem *work_elem;
  struct str_list_elem *free_elem;

  work_elem = str_list->first_elem;

  for (n = 0; n < str_list->num_elems; n++) {
    free_elem = work_elem;
    work_elem = work_elem->next_elem;
    free(free_elem);
  }

  str_list->num_elems = 0;
}

void free_info_list(struct info_list *info_list)
{
  int n;
  struct info_list_elem *work_elem;
  struct info_list_elem *free_elem;

  work_elem = info_list->first_elem;

  for (n = 0; n < info_list->num_elems; n++) {
    free_elem = work_elem;
    work_elem = work_elem->next_elem;
    free(free_elem);
  }

  info_list->num_elems = 0;
}

void free_info2_list(struct info2_list *info2_list)
{
  int n;
  struct info2_list_elem *work_elem;
  struct info2_list_elem *free_elem;

  work_elem = info2_list->first_elem;

  for (n = 0; n < info2_list->num_elems; n++) {
    free_elem = work_elem;
    work_elem = work_elem->next_elem;
    free(free_elem);
  }

  info2_list->num_elems = 0;
}
