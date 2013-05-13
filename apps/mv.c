#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ext2.h>
#include <ext2_write.h>
#include <ext2_read.h>
#include <ext2_common.h>
#include <ext2cheating.h>
#include "helper.h"

//void mv( struct ext2_filesystem *fs, char *pathS,
 //        char *nameS, char *pathD, char *nameD ) {
 void mv( char* nameS, char* nameD){

    cp( nameS, nameD );
    rm( nameS );

}

