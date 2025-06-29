;
; Christian Groessler, Dec-2001
;
; ucase_fn
; helper routine to convert a string (file name) to uppercase
; used by open.s and remove.s
;
; Calling parameters:
;       AX   - points to filename
;       tmp2 - 0/$80 for don't/do prepend default device if no device
;              is present in the passed string (only .ifdef DEFAULT_DEVICE)
; Return parameters:
;       C    - 0/1 for OK/Error (filename too long)
;       AX   - points to uppercased version of the filename on the stack
;       tmp3 - amount of bytes used on the stack (needed for cleanup)
; Uses:
;       ptr4 - scratch pointer used to remember original AX pointer
;
;

        .include        "atari.inc"

.ifdef  DEFAULT_DEVICE
        .importzp tmp2
        .import __defdev
.endif
        .importzp tmp3,ptr4,c_sp
        .import subysp,addysp
        .export ucase_fn

.proc   ucase_fn

        ; we make sure that the filename doesn't contain lowercase letters
        ; we copy the filename we got onto the stack, uppercase it and use this
        ; one to open the iocb
        ; we're using tmp3, ptr4

        ; save the original pointer
        sta     ptr4
        stx     ptr4+1

.ifdef  DEFAULT_DEVICE
        lda     tmp2
        beq     hasdev          ; don't fiddle with device part
        ; bit #0 of tmp2 is used as an additional flag whether device name is present in passed string (1 = present, 0 = not present)
        ldy     #1
        inc     tmp2            ; initialize flag: device present
        lda     #':'
        cmp     (ptr4),y
        beq     hasdev
        iny
        cmp     (ptr4),y
        beq     hasdev
        dec     tmp2            ; set flag: no device in passed string
hasdev:
.endif

        ldy     #128
        sty     tmp3            ; save size
        jsr     subysp          ; make room on the stack

        ; copy filename to the temp. place on the stack, while uppercasing it
        ldy     #0

loop2:  lda     (ptr4),y
        sta     (c_sp),y
        beq     copy_end
        bmi     L1              ; Not lowercase (also, invalid, should reject)
        cmp     #'a'
        bcc     L1              ; Not lowercase
        and     #$DF            ; make upper case char, assume ASCII chars
        sta     (c_sp),y        ; store back
L1:
        iny
        bpl     loop2           ; bpl: this way we only support a max. length of 127

        ; Filename too long
        jsr     addysp          ; restore the stack
        sec                     ; indicate error
        rts

copy_end:

.ifdef  DEFAULT_DEVICE
        lda     #1
        bit     tmp2            ; is a device present in the string?
        bne     hasdev2         ; yes, don't prepend something
        bpl     hasdev2         ; check input parameter (tmp2 != $80)

        ldy     #128+3          ; no, prepend "Dn:" (__defdev)
        sty     tmp3            ; adjust stack size used
        ldy     #3
        jsr     subysp          ; adjust stack pointer
        dey
cpdev:  lda     __defdev,y
        sta     (c_sp),y        ; insert device name, number and ':'
        dey
        bpl     cpdev
hasdev2:
.endif

        ; leave A and X pointing to the modified filename
        lda     c_sp
        ldx     c_sp+1
        clc                     ; indicate success
        rts

.endproc
