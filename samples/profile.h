/* Paravirtualisation entry point to get get current cycle count */
extern void __fastcall__          getcycles(uint32_t *cycles);

/* Profile a parameterless call */
extern unsigned long __fastcall__ profile(void (*proc)(void));