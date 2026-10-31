#include "arch/i686/paging/paging.h"
#include "arch/i686/control_registers/control_registers.h"
#include "stdio.h"
#include <stdint.h>

void enable_paging(uint32_t page_directory)
{
    kprintf(INFO, "Enabling paging with directory %x\n", page_directory);
    uint32_t cr3 = read_cr3();

    cr3 &= 0xFFF; // clear value already inside it
    cr3 |= page_directory;

    write_cr3(cr3);

    uint32_t cr0 = read_cr0();
    cr0 |= (1 << 31); // set CR0.PG
    write_cr0(cr0);
}
