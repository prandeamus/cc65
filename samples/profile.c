#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions */
/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(uint32_t *cycles);
/* Profile a parameterless call */
extern unsigned long __fastcall__ prof(void (*proc)(void));
/* End profile definitions */


/***** USER CODE *******/

void AddSomething(void)
/* To profile */
{
    int a,b,c;

    //fprintf (stderr, "Hello mum ++\n");

    a=2;
    b=7;
    c=a+b;
}

void SubSomething(void)
/* To profile */
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
    printf ("Hello, profiler\n");

    ResultAdd  = prof (AddSomething);
    ResultSub  = prof (SubSomething);

    /* Do it inline */
    getcycles(NULL);
    baz++;
    getcycles(&Post);
    ++baz;
    getcycles(&Pre);

    printf ("Add:%ld, Sub:%ld\n", ResultAdd, ResultSub);
    printf ("Pre,Post %ld,%ld\n", Pre, Post);
    printf ("Bye, profiler\n");

    exit (EXIT_SUCCESS);
}

/* To build and run cl65 -t sim6502 profile.c sim6502.lib && sim65 profile */
