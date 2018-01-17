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
    unsigned long Pre, Post;
    int baz=99;

    ResultAdd  = profile (AddSomething);
    ResultSub  = profile (SubSomething);

    /* Do it inline */
    getcycles(&Post);
    baz++;
    getcycles(&Post);
    ++baz;
    getcycles(&Pre);

    printf ("Add:%ld, Sub:%ld\n", ResultAdd, ResultSub);
    printf ("Pre,Post %ld,%ld\n", Pre, Post);

    exit (EXIT_SUCCESS);
}

/* To build and run 
ca65 profile.s
cl65 -c -O -t sim6502 proftest.c 
cl65 -t sim6502 proftest.o profile.o sim6502.lib
sim65 proftest
*/
