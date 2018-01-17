        .constructor    initprof

        .export         _profile

        .import         _getcycles
        .import         jmpvec
        .importzp       sreg

.bss

cycnul: .dword 0
cycles: .dword 0

; unsigned long __fastcall__ profile(void (*proc)(void))

.code

.proc   _profile

        ; Save proc for later call
        sta jmpvec+1
        stx jmpvec+2

        ; getcycles(&cycles); 
        ; (This value is ignored, but is symmetric with second getcycles call)
        lda #<cycles
        ldx #>cycles
        jsr _getcycles

        ; Call the code being tested
        jsr jmpvec

        ; getcycles(&cycles);
        lda #<cycles
        ldx #>cycles
        jsr _getcycles

        ; cycles = cycles - cycnul
        
        sec
        lda cycles
        sbc cycnul
        sta cycles
        lda cycles+1
        sbc cycnul+1
        sta cycles+1
        lda cycles+2
        sbc cycnul+2
        sta cycles+2
        lda cycles+3
        sbc cycnul+3
        sta cycles+3

        lda cycles+3
        sta sreg+1
        lda cycles+2
        sta sreg
        ldx cycles+1
        lda cycles

        rts

.endproc

; Initialization
.segment        "ONCE"

; Calibrate the profiler with a null procedure
.proc   initprof

        lda #<nullproc
        sta jmpvec+1
        ldx #>nullproc
        stx jmpvec+2

        ; getcycles(&cycles)
        lda #<cycles
        ldx #>cycles
        jsr _getcycles
        
        ; nullproc();
        jsr jmpvec

        ; getcycles(&cycnul)
        lda #<cycnul
        ldx #>cycnul
        jsr _getcycles
nullproc:
        rts

.endproc
