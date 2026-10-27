#include "utils/panic/panic.h"
#include <stdio.h>

__attribute__((noreturn)) void kernel_panic(const char* message)
{
    // Disable interrupts so normal interrupt handlers
    // cannot run while the kernel is halted.
    __asm__ volatile ("cli");

    set_color(MAKE_COLOR(VGA_WHITE, VGA_RED));
    clrscr();

    puts("KERNEL PANIC!\n\n");
    puts(message);

    for (;;)
        __asm__ volatile ("hlt");
}
