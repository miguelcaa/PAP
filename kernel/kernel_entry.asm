[bits 32]
global _start
extern kmain

_start:
    call kmain
.hang:
    hlt
    jmp .hang
