#pragma once

#include <stdarg.h>
#define PANIC_PRINTF_REGS_FMT   ("EAX: 0x%x, EBX: 0x%x, ECX: 0x%x, EDX: 0x%x\nESP: 0x%x, EBP: 0x%x\nESI: 0x%x, EDI: 0x%x\n\n")


void __attribute__((noreturn)) boot_panic(const char* fmt, ...);
