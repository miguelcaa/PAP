#include "idt.h"

#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

extern void isr_keyboard(void);

static inline void lidt(idt_ptr_t* p) {
    __asm__ volatile("lidt (%0)" : : "r"(p));
}

void idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init(void) {
    idt_ptr.limit = (uint16_t)(sizeof(idt_entry_t)*IDT_ENTRIES - 1);
    idt_ptr.base  = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    // IRQ1 -> INT 33 (0x21) após remap
    idt_set_gate(33, (uint32_t)isr_keyboard);

    lidt(&idt_ptr);
}
