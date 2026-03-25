[bits 32]
global isr_keyboard
extern keyboard_handler

isr_keyboard:
    pusha
    call keyboard_handler

    mov al, 0x20
    out 0x20, al      ; EOI PIC master

    popa
    iret
