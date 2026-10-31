#include <stdint.h>
#include <stdio.h>

#include "arch/i686/multiboot/multiboot.h"
#include "arch/i686/gdt/gdt.h"
#include "arch/i686/interrupts/idt/idt.h"

#include "memory/mmap/mmap.h"
#include "memory/pmm/pmm.h"
#include "memory/vmm/vmm.h"

void kernel_main(uint32_t magic, uint32_t multiboot_info)
{
    if (magic != 0x2BADB002) {
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    clrscr();


    gdt_init();
    idt_init();
    kprintf(INFO, "Loaded GDT and IDT\n");

    multiboot_parse((multiboot_info_t*)multiboot_info);

    mmap_init();
    pmm_init();
    vmm_init();

    uint32_t free_pages = pmm_get_free_pages();

    kprintf(INFO, "Memory initialised:\n");
    kprintf(INFO, "\tFree pages in memory: %u\n", free_pages);
    kprintf(INFO, "\tFree KiB   in memory: %u\n", free_pages / 4);
    kprintf(INFO, "\tFree MiB   in memory: %u\n", (free_pages / 4) / 1024);



    for (;;) {
        __asm__ volatile ("hlt");
    }
}
