#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions - this needs to move to a header file */
/* There is no long long or 8-byte type in cc65, so use a structure */
typedef struct {
    uint32_t lo;
    uint32_t hi;
} Long64;

/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(Long64 * buf);
/* End profile definitions */


/* Support functions inside the simulated processor */

static unsigned long __fastcall__ Long64Diff (Long64 *Exit, Long64 *Entry)
/* Calculate difference between two Long64 values */
{
    unsigned long Diff;
    if(Exit->hi == Entry->hi)
    {
        Diff = Exit->lo - Entry->lo;
    }
    else if (Exit->hi == Entry->hi - 1)
    {
        Diff = (~Entry->lo)+1+Exit->lo;
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

unsigned long __fastcall__ profile(void (*proc)(void))
/* Profile a block of code: a pointer to a fastcall function with no parameters and no return value */
{   
    Long64 CyclesEntry;
    Long64 CyclesExit;

    /* The two calls to getcycles() have a nearly identical overhead so they nearly cancel each other out */
    /* To be precise, calibrate by calling a null procedure and using that */
    /* The call to proc will have the overhead of an indirect call on a stack variable */
    getcycles (&CyclesEntry);
    proc ();
    getcycles (&CyclesExit);

    /* Calculate difference, or overflow */
    return Long64Diff(&CyclesExit, &CyclesEntry);
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
