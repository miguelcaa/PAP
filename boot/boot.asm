; boot/boot.asm
[org 0x7C00]
[bits 16]

KERNEL_OFF     equ 0x1000
KERNEL_SECTORS equ 32        ; ajuste se o kernel crescer

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; guarda drive (BIOS passa em DL)
    mov [boot_drive], dl

    mov si, msg
    call print

    ; Ler kernel do disco para 0000:1000 (a partir do setor 2)
    mov bx, KERNEL_OFF
    mov ax, 0x0000
    mov es, ax

    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0x00
    mov dh, 0x00
    mov cl, 0x02
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    ; A20 (fast)
    in  al, 0x92
    or  al, 00000010b
    out 0x92, al

    ; GDT
    lgdt [gdt_descriptor]

    ; Protected Mode ON
    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    ; Far jump para carregar CS com selector code (0x08)
    jmp 0x08:protected_mode

; --------------------------
; 32-bit
; --------------------------
[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    ; jump para kernel em 0x1000
    jmp 0x00001000

; --------------------------
; 16-bit helpers
; --------------------------
[bits 16]
print:
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .next
.done:
    ret

disk_error:
    mov si, err
    call print
    jmp $

msg db "Boot OK -> PM32...", 0
err db 13,10,"DISK ERROR",0

boot_drive db 0

; --------------------------
; GDT: null, code, data
; --------------------------
align 8
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF   ; code: base0 limit4G
    dq 0x00CF92000000FFFF   ; data: base0 limit4G
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55
