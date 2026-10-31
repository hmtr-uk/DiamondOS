#include "arch/i686/control_registers/control_registers.h"
#include <stdint.h>

uint32_t read_cr0()
{
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    return cr0;
}

void write_cr0(uint32_t cr0)
{
    __asm__ volatile ("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

uint32_t read_cr3()
{
    uint32_t cr3;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

void write_cr3(uint32_t cr3)
{
    __asm__ volatile ("mov %0, %%cr3" :: "r"(cr3) : "memory");
}
