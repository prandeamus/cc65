#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "profile.h"

void AddSomething(void)
/* Example to profile */
{
    int a,b,c;

    a=2;
    b=7;
    c=a+b;
}

void SubSomething(void)
/* Example to profile */
{
    int a,b,c;
    a=2;
    b=7;
    c=a-b;
}

int main(void)
/* Entry point */
{
    unsigned long ResultAdd;
    unsigned long ResultSub;
    unsigned long PreLocal, PostLocal, PreStatic, PostStatic;
    int baz=99;
    static int zab=100;

    ResultAdd  = profile (AddSomething);
    ResultSub  = profile (SubSomething);

    /* Do it inline */
    getcycles(&PostLocal);
    baz++;
    getcycles(&PostLocal);
    ++baz;
    getcycles(&PreLocal);
    zab++;
    getcycles(&PostStatic);
    ++zab;
    getcycles(&PreStatic);

    printf ("Add:%ld, Sub:%ld\n", ResultAdd, ResultSub);
    printf ("Pre-increment (local),Post-increment (local) %ld,%ld\n", PreLocal, PostLocal);
    printf ("Pre-increment (static),Post-increment (static) %ld,%ld\n", PreStatic, PostStatic);

    exit (EXIT_SUCCESS);
}

/* To build and run 
cl65 -c -O -t sim6502 proftest.c 
cl65 -t sim6502 proftest.o sim6502.lib
sim65 proftest
*/
