#include "vga.h"
#include "io.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_COLOR  0x07

static volatile unsigned short* const VGA = (unsigned short*)0xB8000;
static int cursor = 0;

/* Cria uma entrada VGA (caractere + cor) */
static inline unsigned short vga_entry(char c) {
    return (unsigned short)c | ((unsigned short)VGA_COLOR << 8);
}

/* Atualiza o cursor de HARDWARE */
static void vga_update_cursor(int pos) {
    unsigned short p = (unsigned short)pos;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(p & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((p >> 8) & 0xFF));
}

/* Limpa o ecrã */
void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = vga_entry(' ');
    }
    cursor = 0;
    vga_update_cursor(cursor);
}

/* Scroll da tela (quando chega ao fundo) */
static void scroll_if_needed(void) {
    if (cursor < VGA_WIDTH * VGA_HEIGHT)
        return;

    /* Move tudo uma linha para cima */
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[(y - 1) * VGA_WIDTH + x] =
                VGA[y * VGA_WIDTH + x];
        }
    }

    /* Limpa a última linha */
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ');
    }

    cursor = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

/* Escreve UM carácter */
void putchar(char c) {
    if (c == '\n') {
        cursor += VGA_WIDTH - (cursor % VGA_WIDTH);
        scroll_if_needed();
        vga_update_cursor(cursor);
        return;
    }

    if (c == '\b') {  /* BACKSPACE */
        if (cursor > 0) {
            cursor--;
            VGA[cursor] = vga_entry(' ');
            vga_update_cursor(cursor);
        }
        return;
    }

    VGA[cursor++] = vga_entry(c);
    scroll_if_needed();
    vga_update_cursor(cursor);
}

/* Escreve uma string */
void print(const char* s) {
    while (*s) {
        putchar(*s++);
    }
}
