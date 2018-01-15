#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions */
/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(uint32_t *cycles);
/* Profile a parameterless call */
extern unsigned long __fastcall__ prof(void (*proc)(void));
/* End profile definitions */

/* Support functions inside the simulated processor */
#if 0
static void NullProc(void)
/* For comparison, a function that does nothing */
{
    /* Body deliberately left blank */
}
#endif

/* Start and stop cycle types, store here to keep the calls to getcycles equal overhead */

unsigned long __fastcall__ profile(void (*proc)(void))
/* Profile a block of code: a pointer to a fastcall function with no parameters and no return value */
{   
    static uint32_t Cycles;

    /* Save proc parameter, in AX, to the jmpvec */
    __asm__ ("sta jmpvec+1");
    __asm__ ("stx jmpvec+2");
    /* Satisfy compiler that proc has been used */
    (void)proc;

    /* The two calls to getcycles() have an identical overhead so they cancel each other out */
    /* To be precise, calibrate by calling a null procedure and using that. */
    getcycles (NULL);
    __asm__("jsr jmpvec");
    getcycles (&Cycles);

    return Cycles;
}

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

    printf ("Add:%08lX, Sub:%08lX\n", ResultAdd, ResultSub);
    printf ("Pre,Post %ld,%ld\n", Pre, Post);
    printf ("Bye, profiler\n");

    exit (EXIT_SUCCESS);
}

/* To build and run cl65 -t sim6502 profile.c sim6502.lib && sim65 profile */
