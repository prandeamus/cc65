#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions - this needs to move to a header file */
/* There is no long long or 8-byte type in cc65, so use a structure */
typedef struct {
    uint32_t lo;
    uint32_t hi;
} ULong64;

static ULong64 Cycles1;
static ULong64 Cycles2;

/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(ULong64 * buf);
/* End profile definitions */


/* Support functions inside the simulated processor */

static unsigned long __fastcall__ ULong64Diff (ULong64 *Exit, ULong64 *Entry)
/* Calculate difference between two ULong64 values */
{
    unsigned long Diff;
    if(Exit->hi == Entry->hi)
    {
        Diff = Exit->lo - Entry->lo;
    }
    else if (Exit->hi == Entry->hi - 1)
    {
        Diff = (Exit->lo)-(Entry->lo)+1;
    }
    else
    {   
        /* Overflow. Why are you trying to profile something this big? :) */
        return UINT32_MAX;
    }

    return Diff;
}

static void NullProc(void)
/* For comparison, a function that does nothing */
{
    /* Body deliberately left blank */
}

/* Start and stop cycle types, store here to keep the calls to getcycles equal overhead */

unsigned long __fastcall__ profile(void (*proc)(void))
/* Profile a block of code: a pointer to a fastcall function with no parameters and no return value */
{   
    /* Save proc parameter, in AX, to the jmpvec */
    __asm__ ("sta jmpvec+1");
    __asm__ ("stx jmpvec+2");
    /* Satisfy compiler that proc has been used */
    (void)proc;

    /* The two calls to getcycles() have an identical overhead so they cancel each other out */
    /* To be precise, calibrate by calling a null procedure and using that. */
    getcycles (&Cycles1);
    __asm__("jsr jmpvec");
    getcycles (&Cycles2);

    /* Calculate difference, or overflow */
    return ULong64Diff(&Cycles2, &Cycles1);
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
    printf ("Hello, profiler\n");
    ResultNull = profile (NullProc);
    ResultAdd = profile (AddSomething) - ResultNull;
    ResultSub = profile (SubSomething) - ResultNull;
    printf ("Null: %ld, Add:%ld, Sub:%ld\n", ResultNull, ResultAdd, ResultSub);
    printf ("Bye, profiler\n");
    exit (EXIT_SUCCESS);
}

/* To build and run cl65 -t sim6502 profile.c sim6502.lib && sim65 profile */
