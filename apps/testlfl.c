/*
 *
 */
#include <lfl.h>
#include <lfs.h>
#include <lfilesys.h>
#include <conf.h>

int testlfl(void){


//  lfsInit(0);
  int f;
  //  printf("getdev %d\n", getdev("EXTROOTFS"));
  f = lfsOpen(getdev("EXTROOTFS"), "./test" , 0);
  printf( "File0: %d\n", getdev("LFILE0"));

  printf( "Char %c\n", lflGetc( &devtab[getdev("LFILE0")] ));

  char testing[14];

  lflRead( &devtab[getdev("LFILE0")], (void*)testing, 20);
  printf("String %s\n", testing);

  lflPutc( &devtab[getdev("LFILE0")], '!');

  printf( "Char %c\n", lflGetc( &devtab[getdev("LFILE0")] ));

  lfltab[0].lfpos=0;

  char reading[30];
  lflRead( &devtab[getdev("LFILE0")], (void*)reading, 20);
  printf("NS: %s\n", reading);

  char lolface[40] = "HELLLLLOOO";;
  lflWrite( &devtab[getdev("LFILE0")], (void*)lolface, 20);

  lfltab[0].lfpos=0;
  lflRead( &devtab[getdev("LFILE0")], (void*)reading, 30);
  printf("NS: %s\n", reading);

  lflClose( &devtab[getdev("LFILE0")]);

  lfsOpen(getdev("EXTROOTFS"), "./test", 0);
  lflRead( &devtab[getdev("LFILE0")], (void*)reading, 30);
  printf("NS: %s\n", reading);

  return 0;
}
