#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <dos.h>
#define _finddata_t find_t
#endif
#include <direct.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>

char current_dir[] = ".";
char parent_dir[] = "..";
char allfiles[] = "*.*";
int bKeepDir;
char save_dir[_MAX_PATH];

void zap_dir(char *dirname);

void main(int argc,char **argv)
{
  struct _finddata_t findt;
#ifdef WIN32
  long search_handle;
#endif

  if (argc < 2) {
    printf("usage: zap (-k) directory\n");
    return;
  }

  if (!strcmpi(argv[1],"-k"))
    bKeepDir = 1;
  else
    bKeepDir = 0;

  getcwd(save_dir,_MAX_PATH);

  if (chdir(argv[1+bKeepDir])) {
    printf("%s is not a directory\n",argv[1+bKeepDir]);
    return;
  }

#ifdef WIN32
  if ((search_handle = _findfirst(allfiles,&findt)) != -1) {
#else
  if (!(_dos_findfirst(allfiles,/*_A_ARCH |*/ _A_SUBDIR,&findt))) {
#endif
    for ( ; ; ) {
      if (findt.attrib & _A_SUBDIR) {
        if (strcmp(findt.name,current_dir) && strcmp(findt.name,parent_dir)) {
          zap_dir(findt.name);

          if (!bKeepDir)
            rmdir(findt.name);
        }
      }
      else {
        chmod(findt.name,S_IREAD | S_IWRITE);
        remove(findt.name);
      }

#ifdef WIN32
      if (_findnext(search_handle,&findt))
#else
      if (_dos_findnext(&findt))
#endif
        break;
    }

#ifdef WIN32
    _findclose(search_handle);
#endif
  }

  chdir(save_dir);

  if (!bKeepDir)
    rmdir(argv[1+bKeepDir]);
}

void zap_dir(char *dirname)
{
  struct _finddata_t findt;
#ifdef WIN32
  long search_handle;
#endif

  chdir(dirname);

#ifdef WIN32
  if ((search_handle = _findfirst(allfiles,&findt)) != -1) {
#else
  if (!(_dos_findfirst(allfiles,/*_A_ARCH |*/ _A_SUBDIR,&findt))) {
#endif
    for ( ; ; ) {
      if (findt.attrib & _A_SUBDIR) {
        if (strcmp(findt.name,current_dir) && strcmp(findt.name,parent_dir)) {
          zap_dir(findt.name);

          if (!bKeepDir)
            rmdir(findt.name);
        }
      }
      else {
        chmod(findt.name,S_IREAD | S_IWRITE);
        remove(findt.name);
      }

#ifdef WIN32
      if (_findnext(search_handle,&findt))
#else
      if (_dos_findnext(&findt))
#endif
        break;
    }

#ifdef WIN32
    _findclose(search_handle);
#endif
  }

  chdir(parent_dir);
}
