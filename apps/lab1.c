#include <thread.h>

int myfuncA()
{
    int b = 11;
    int c = 12;
    int d = 13;
    int e = 14;
    int f = 15;
    int A = 12;
    int q = 2;
    int z = 4;

  // retrieve pointer
  register int i asm("sp");

  // copy the pointer value so we don't modify the actual SP
  int *pointer = (int *)i;

  printf("Top of the stack - Location: 0x%x, Contents: 0x%x\n", pointer, *pointer);

  pointer++;

  int addr[5];
  int cont[5];

  int a;
  for(a = 0; a < 5; a++){
    addr[a] = pointer;
    cont[a] = *pointer;
    pointer++;
  }


  for(a = 0; a < 5; a++){
    printf("\tLocation: 0x%x, Contents: 0x%x\n", addr[a], cont[a]);
  }

  return 4;
}

int myprogA()
{
  
    int b = 11;
    int c = 12;
    int d = 13;
    int e = 14;
    int f = 15;
    int A = 12;
    int q = 2;
    int z = 4;


  // retrieve pointer
  register int i asm("sp");

  // copy the pointer value so we don't modify the actual SP
  int *pointer = (int *)i;


  printf("Top of the stack - Location: 0x%x, Contents: 0x%x\n", pointer, *pointer);

  pointer++;

  int addr[5];
  int cont[5];

  int a;
  for(a = 0; a < 5; a++){
    addr[a] = pointer;
    cont[a] = *pointer;
    pointer++;
  }


  for(a = 0; a < 5; a++){
    printf("\tLocation: 0x%x, Contents: 0x%x\n", addr[a], cont[a]);
  }

  return myfuncA(2, 4, 3, 5);
}

void printprocskts(){

  extern struct thrent thrtab[];

  // TODO: Resume causes our program to change in such a way 
  //       that everythign crasshes i belive it's altering the 
  //       return register (that's what the error says at least.)
  //       Don't know how to calculate the base Limit is base + length?
  //       How do you find the stakc pointer?
  //       asm("sp") will give the stak pointer of this function not 
  //       of myprogA

  tid_typ id1;
  resume(id1 = create((void*) myprogA, 1024, 20, "Thread 1", 0));


  printf("THREAD INFOZ:\n");
  printf("Proc: [%s] tid = %d\n", thrtab[id1].name, id1);
  printf("\tStack:\n");
  printf("\t\tBase = %d\n", thrtab[id1].stkbase);
  printf("\t\tLen = %d\n", thrtab[id1].stklen);
  printf("\t\tLimit = %d\n", thrtab[id1].stkbase - 1024);
  printf("\t\tStkPtr = %d\n", thrtab[id1].stkptr);
  printf("\t\tName: %s\n", thrtab[id1].name);

  tid_typ id2;
  resume(id2 = create((void*) myprogA, 1024, 20, "Thread 2", 0));

  printf("THREAD INFOZ:\n");
  printf("Proc: [%s] tid = %d\n", thrtab[id2].name, id2);
  printf("\tStack:\n");
  printf("\t\tBase = %d\n", thrtab[id2].stkbase);
  printf("\t\tLen = %d\n", thrtab[id2].stklen);
  printf("\t\tLimit = %d\n", thrtab[id2].stkbase - 1024);
  printf("\t\tStkPtr = %d\n", thrtab[id2].stkptr);
  printf("\t\tName: %s\n", thrtab[id2].name);

  tid_typ id3;
  resume(id3 = create((void*) myprogA, 1024, 20, "Thread 3", 0));

  printf("THREAD INFOZ:\n");
  printf("Proc: [%s] tid = %d\n", thrtab[id3].name, id3);
  printf("\tStack:\n");
  printf("\t\tBase = %d\n", thrtab[id3].stkbase);
  printf("\t\tLen = %d\n", thrtab[id3].stklen);
  printf("\t\tLimit = %d\n", thrtab[id3].stkbase - 1024);
  printf("\t\tStkPtr = %d\n", thrtab[id3].stkptr);
  printf("\t\tName: %s\n", thrtab[id3].name);

}

void printtos(void){
  resume(create((void*) myprogA, 1024, 20, "Clever String", 0));
}

