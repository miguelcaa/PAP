#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "io.h"

void kmain(void) {
    clear_screen();
    print("Nano OS - PM32 + IRQ Keyboard\n");

    idt_init();
    pic_remap(0x20, 0x28);

    pic_set_mask(0xFF);   // mascara tudo
    pic_clear_mask(1);    // libera IRQ1 (teclado)

    sti();

    print("Digite: ");

    while (1) {
        char c = keyboard_getchar();
        if (c) putchar(c);
        hlt(); // dorme até a próxima interrupção
    }
}
