#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions */
/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(uint32_t *cycles);
/* End profile definitions */

/* Support functions inside the simulated processor */

static void NullProc(void)
/* For comparison, a function that does nothing */
{
    /* Body deliberately left blank */
}

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
    getcycles (&Cycles);
    __asm__("jsr jmpvec");
    getcycles (&Cycles);

    return Cycles;
}

/***** USER CODE *******/

void AddSomething(void)
/* To profile */
{
    int a,b,c;
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
    unsigned long ResultNull;
    unsigned long ResultAdd;
    unsigned long ResultSub;
    unsigned long pre,post;
    int baz=99;
    printf ("Hello, profiler\n");

    ResultNull = profile (NullProc);
    ResultAdd  = profile (AddSomething) - ResultNull;
    ResultSub  = profile (SubSomething) - ResultNull;

    /* Do it inline */
    getcycles(NULL);
    baz++;
    getcycles(&post);
    ++baz;
    getcycles(&pre);

    printf ("Null: %ld, Add:%ld, Sub:%ld\n", ResultNull, ResultAdd, ResultSub);
    printf ("pre,post %ld,%ld\n", pre, post);
    printf ("Bye, profiler\n");

    exit (EXIT_SUCCESS);
}

/* To build and run cl65 -t sim6502 profile.c sim6502.lib && sim65 profile */
