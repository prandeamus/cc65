#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions */
/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__          getcycles(uint32_t *cycles);
/* Profile a parameterless call */
extern unsigned long __fastcall__ profile(void (*proc)(void));
/* End profile definitions */


/***** USER CODE *******/

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

/* To build and run cl65 -t sim6502 profile.c sim6502.lib && sim65 profile */
