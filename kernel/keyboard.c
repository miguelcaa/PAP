#include "keyboard.h"
#include "io.h"

static unsigned char keymap[128] = {
  0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,
  '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
  'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
  'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

static volatile char last_char = 0;

void keyboard_handler(void) {
    unsigned char sc = inb(0x60);
    if (sc & 0x80) return;
    if (sc < 128) last_char = (char)keymap[sc];
}

char keyboard_getchar(void) {
    char c = last_char;
    last_char = 0;
    return c;
}
