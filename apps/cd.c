#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>

void cd(char* dirname){
  int size = strnlen(curdir, 1500);

  if(ext2_get_dirent_from_path(filesystem, curdir, dirname)->filetype ==
     EXT2_FT_DIR ){
    memcpy(curdir+size, dirname, strnlen(dirname, 1500));
  } else printf("No such directory");

}
