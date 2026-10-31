#include "utils/panic/panic.h"
#include <stdio.h>

void __attribute__((noreturn)) boot_panic(const char* fmt, ...)
{
    __asm__ volatile ("cli");

    va_list args;
    va_start(args, fmt);

    kprintf(FATAL, "KERNEL PANIC IN BOOT:\n\n");
    vprintf(fmt, args);

    for (;;)
        __asm__ volatile ("hlt");
}
