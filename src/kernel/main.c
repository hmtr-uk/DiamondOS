#include <stdint.h>
#include <stdio.h>

#include "arch/i686/multiboot/multiboot.h"
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

    multiboot_parse((multiboot_info_t*)multiboot_info);

    mmap_init();

    pmm_init();
    kprintf(INFO, "Memory initialised:\n");
    kprintf(INFO, "\tFree pages in memory: %u\n", pmm_get_free_pages());
    kprintf(INFO, "\tFree KB    in memory: %u\n", pmm_get_free_pages() / 4);
    kprintf(INFO, "\tFree MB    in memory: %u\n", (pmm_get_free_pages() / 4) / 1024);

    vmm_init();

    kprintf(INFO, "Memory initialised:\n");
    kprintf(INFO, "\tFree pages in memory: %u\n", pmm_get_free_pages());
    kprintf(INFO, "\tFree KB    in memory: %u\n", pmm_get_free_pages() / 4);
    kprintf(INFO, "\tFree MB    in memory: %u\n", (pmm_get_free_pages() / 4) / 1024);

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
