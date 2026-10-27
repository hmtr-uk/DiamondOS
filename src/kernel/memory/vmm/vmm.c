#include "memory/vmm/vmm.h"
#include "arch/i686/control_registers/control_registers.h"
#include "memory/pmm/pmm.h"
#include "stdio.h"
#include "utils/math.h"
#include <stdint.h>
#include <stdbool.h>

uint32_t* page_directory;

void vmm_init()
{
    // allocate the page directory
    page_directory = (uint32_t*)(uintptr_t)pmm_alloc_page();

    // empty out
    for (int i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x2;
    }

    vmm_map_addresses(0xb8000, 0xd8000, 0xFFFF, 0x0, true);
}

void vmm_create_page_table(uint32_t directory_index, bool kernel)
{
    if (directory_index > 1024)
    {
        kprintf(WARN, "VMM: Cant create page table %u!\n");
    }

    uint32_t page_table_addr = pmm_alloc_page();
    uint32_t* page_table = (uint32_t*)(uintptr_t)page_table_addr;

    page_directory[directory_index] = page_table_addr | (0x3 | (kernel << 2)); // set supervisor if kernel mode
}


void vmm_auto_map_page(uint32_t phys, uint32_t virt, uint32_t flags, bool kernel)
{

    // im not merging these if statements for obvious reasons
    if (page_directory[DIRECTORY_INDEX(virt)] & 0x1) // page table doesnt exist
    {
        vmm_create_page_table(DIRECTORY_INDEX(virt), kernel);
    }

    else if (((page_directory[DIRECTORY_INDEX(virt)] & 0x4) > 0) != kernel) // page table has different mode
    {
        vmm_create_page_table(DIRECTORY_INDEX(virt), kernel);
    }
}

void vmm_map_addresses(uint32_t start, uint32_t vstart, uint32_t length, uint32_t flags, bool kernel)
{
    if (length == 0) return;

    if (length > UINT32_MAX - start)
    {
        kprintf(WARN, "VMM: Can't map address %x, as it will overflow with length %x\n", start, length);
        return;
    }

    if (length > UINT32_MAX - vstart)
    {
        kprintf(WARN, "VMM: Can't map virtual address %x, as it will overflow with length %x\n", vstart, length);
        return;
    }

    if ((start + length) % 4096 != 0)
    {
        if (start + length > UINT32_MAX - 4095)
        {
            kprintf(WARN, "VMM: Can't map address %x, as it will overflow with length %x\n", start, length);
            return;
        }
    }

    if ((vstart + length) % 4096 != 0)
    {
        if (vstart + length > UINT32_MAX - 4095)
        {
            kprintf(WARN, "VMM: Can't map virtual address %x, as it will overflow with length %x\n", vstart, length);
            return;
        }
    }

    if ((start % 4096) != (vstart % 4096))
    {
        kprintf(ERROR, "VMM: Addresses %x and %x have different page offsets. The Virtual Memory Mapper cannot create an entry!\n", start, vstart);
    }

    uint32_t align_start = ALIGN_DOWN(start, 4096);
    uint32_t align_end = ALIGN_UP(start + length, 4096);

    uint32_t virtual_align_start = ALIGN_DOWN(vstart, 4096);
    uint32_t virtual_align_end = ALIGN_UP(vstart + length, 4096);

    uint32_t pages = (align_end - align_start) / 4096;

    for (int i = 0; i < pages; i++)
    {
        vmm_auto_map_page(align_start + (i * 4096), virtual_align_start + (i * 4096), flags, kernel);
    }
}
