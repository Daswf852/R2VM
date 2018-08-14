mov sp, 0
jmp init

;port numbers
mov r12, 0
random:    dw 1

;arguments
colors:    dw 0x0000, 0x000F
coords:    dw 0x0000, 0x0000
strptr:    dw 0x0000
gotchar:   dw 0x0000 

;predefined variables
declookup:
hexlookup: dw "0123456789ABCDEF"

;user defined variables
message:   dw "Hello",0


; -> move
; * Takes in [coords] and [coords+1] as
;   coordinates to move the cursor of
;   the r2term to
move:
    push r0
    mov r0, coords
    and [r0], 15         ; * Set the row limit (16 for r2term)
    add r0, 1            ; * Offset the pointer
    and [r0], 11         ; * Set the column limit (12 for r2term)
    mov r0, [r0-1]       ; * get the x value
    shl r0, 5            ; * Make space for y
    or r0, [coords]      ; * Combine x and y
    or r0, 0x1000        ; * Add the command 0x1000 according to the 
                         ;   screen's protocol
    send r12, r0      ; * Send the move command to the screen port
    pop r0
    ret


; -> uintmul -- DEPRECATED
; * Takes in r0 and r1, multiplies
;   and stores in r0.
uintmul: ;(int r0, int r1)
    push r1
    push r2
    mov r2, r0

    cmp r1, 0
    jne .loop
        ret
    .loop:
        add r0, r2
        sub r1, 1
        cmp r1, 1
        jne .loop
    pop r2
    pop r1
    ret


;int pmul(unsigned int a, unsigned int b){
;    unsigned int res;
;    while (b > 0) {
;        if (b & 1)
;            res = res + a;
;        a <<= 1;
;        b >>= 1;
;    }
;    return res;
;}

; -> russian
; * Russian peasant multiplication.
; * Takes in r0 and r1, multiplies
;   and stores in r2.
; * C code is above, went off that
;; TODO: optimise
russian:
    push r0
    push r1
    mov r2, 0
    .while:
        cmp [sp], 0
        jng .while_end
        .if:
            mov r1, [sp]
            and r1, 1
            cmp r1, 0
            je .if_end
            add r2, [sp+1]
        .if_end:
        shl [sp+1], 1
        shr [sp], 1
        jmp .while
    .while_end:
    pop r1
    pop r0
    ret

; -> printhex16
; * Prints a value given by r0 in base16
;; TODO: optimise
printhex16:
    ;taking a simple approach since its better on this slow processor
    ;(instead of the below:
    ;for(uint16_t i = 0; i<4; i++){
    ;    send(*port, hexlookup[r0>>(i*4)&0x000F]);
    ;})
    push r0
    push r1
    push r2
    push r3
    push r4
    mov r4, hexlookup

    mov r1, r0
    mov r2, r0
    mov r3, r0

    and r0, 0x000F
    mov r0, [r4+r0]
    
    and r1, 0x00F0
    shr r1, 4
    mov r1, [r4+r1]
    
    and r2, 0x0F00
    shr r2, 8
    mov r2, [r4+r2]
    
    and r3, 0xF000
    shr r3, 12
    mov r3, [r4+r3]

    send r12, r3
    send r12, r2
    send r12, r1
    send r12, r0

    pop r4
    pop r3
    pop r2
    pop r1
    pop r0

    ret


; -> printstring
; * Prints a null-terminated string
;   from a memory location pointed
;   by [strptr]
printstring:
    push r0
    mov r0, [strptr]
    .loop:
        and [r0], 0x00FF ; * Remove unnecessary data if provided
        send r12, [r0]    ; * Send the character value to the screen port
        add r0, 1        ; * Incerment the character pointer
        cmp [r0], 0      ; * Compare the next value with 0 to check if the
                         ;   string ended
        jne .loop        ; * If the value is 0, continue printing

    pop r0
    ret


; -> color
; * Sets the color of the r2term 
;   according to the irgb values given
;   by [colors] and [color+1]
; irgb: intensity, red, green, blue
color:
    push r1
    push r0
    mov r0, colors
    mov r1, [r0]

    shl r1, 4            ; * Make space for the foreground irgb value
    or r1, [r0+1]     ; * Combine the irgb values
    and r1, 0x00FF       ; * Remove unnecessary data if provided
    or r1, 0x2000        ; * Add the command 0x2000 according to the 
                         ;   screen's protocol
    send r12, r1      ; * Send the color value to the screen port
    pop r0
    pop r1
    ret


; -> clear
; * Clears the r2term by sending empty
;   characters
clear:
    push r0              ; * Backup the temporarily used registers
    push r1
    mov r0, 0            ; * Set the initial coordinates
    mov r1, 0
    call move            ; * Move to the initial coordinates
    .loop:               ; * Begin the clearing loop
        send r12, 32  ; * Send an empty character to the screen port
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        send r12, 32
        add r0, 1        ; * Incerment the row counter, fill the next line
        cmp r0, 12       ; * If we hit the bottom of the screen, stop
        jne .loop
    pop r1
    pop r0
    ret


; -> getchar
getchar:
    .wait_loop:
        wait r0            ; * Wait for a bump.
        js .wait_loop      ; * The sign bit is unset if bumped
        cmp r0, r12
        jnz .wait_loop
        bump r0            ; * Ask for character code.
    .recv_loop:
        recv r0, r12 ; * Receive character code.
        jnc .recv_loop     ; * The carry bit it set if something is received.
    ret


init:
    ;call color
    ;call clear
    ;call move
main:
    ;mov [strptr], str
    mov r0, str
    mov [strptr], r0
    call printstring
hlt

str:
    dw "Hello, world!", 0
