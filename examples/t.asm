;some sort of torture program for R2(VM)
call alu
hlt

alu:
    add r2, 512 ;length
    .mloop:
        mov r0, 1
        mov r1, 0
        .loop1:
            ands r1, r0
            ors r1, r0
            xors r1, r0
            add r1, r0
            jnc .loop1
        mov r0, 0
        mov r1, 1
        sub r0, 1
        .loop2:
            ands r1, r0
            ors r1, r0
            xors r1, r0
            sub r0, r1
            jnc .loop2
        sub r2, 1
        jnz .mloop
    ret
