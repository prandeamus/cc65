#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Profile definitions */
/* There is no long long or 8-byte type in cc65, so use a buffer */
#define CYCLE_BUFFER_SIZE 8
typedef union {
    uint8_t by [sizeof(uint32_t)*2];
    struct longs {
        uint32_t lo;
        uint32_t hi;
    };
} CycleBuffer;

/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__ getcycles(CycleBuffer * buf);
/* End profile definitions */

/* Support functions inside the simulated processor */

static unsigned long __fastcall__ CycleBufferDiff (CycleBuffer *BufExit, CycleBuffer *BufEntry)
/* Calculate difference between two cycle buffers. Ignores overflow */
{
    unsigned long Diff;
    if(BufExit->hi == BufEntry->hi)
    {
        Diff = BufExit->lo - BufEntry->lo;
    }
    else if (BufExit->hi == BufEntry->hi-1)
    {
        Diff = (~BufEntry->lo)+1+BufExit->lo;
    }
    else
    {
        return UINT32_MAX;
    }
    return Diff;
}

static CycleBuffer CyclesEntry;
static CycleBuffer CyclesExit;

unsigned long __fastcall__ profile(void (*proc)(void))
/* Profile a block of code: a pointer to a fastcall function with no parameters and no return value */
{   
    /* The two calls to getcycles() have a fixed overhead that will cancel out on subtraction*/
    /* The call to proc will have the overhead of an indirect call on a stack variable */
    getcycles (&CyclesEntry);
    proc ();
    getcycles (&CyclesExit);

    return CycleBufferDiff(&CyclesExit, &CyclesEntry);
}

/***** USER CODE *******/

void FuncToProfile(void)
/* Test function to profile. Nothing special */
{
    int a,b,c;
    a=2;
    b=7;
    c=a+b;
}

int main(void)
/* Entry point */
{
    unsigned long Result;
    printf ("Hello, profiler\n");
    Result = profile(FuncToProfile);
    printf ("Bye, profiler (%ld)\n", Result);
    exit (EXIT_SUCCESS);
}

/* To build cl65 -t sim6502 profile.c && sim65 profile */