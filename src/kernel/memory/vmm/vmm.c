#include "memory/vmm/vmm.h"
#include "arch/i686/paging/paging.h"
#include "memory/mmap/mmap.h"
#include "memory/pmm/pmm.h"
#include "stdio.h"
#include "utils/binary.h"
#include "utils/math.h"
#include <stdint.h>
#include <stdbool.h>

uint32_t* page_directory;

typedef enum {
    VMM_PAGE_FLAGS_PRESENT          = (1 << 0),
    VMM_PAGE_FLAGS_WRITABLE         = (1 << 1),
    VMM_PAGE_FLAGS_USER             = (1 << 2),
    VMM_PAGE_FLAGS_WRITE_THROUGH    = (1 << 3),
    VMM_PAGE_FLAGS_CACHE_DISABLE    = (1 << 4),
    VMM_PAGE_FLAGS_ACCESSED         = (1 << 5),
    VMM_PAGE_FLAGS_DIRTY            = (1 << 6),
    VMM_PAGE_FLAGS_PAT              = (1 << 7),
    VMM_PAGE_FLAGS_GLOBAL           = (1 << 8),

    VMM_PAGE_RO                     = 0,
    VMM_PAGE_RW                     = (1 << 1),
} vmm_page_flags_t;

void vmm_init()
{
    // allocate the page directory
    uint32_t page_directory_addr = pmm_alloc_page();

    if (page_directory_addr == PMM_ERROR)
    {
        kprintf(ERROR, "VMM: PMM failed; can't continue!");
        return;
    }

    page_directory = (uint32_t*)(uintptr_t)page_directory_addr;

    // empty out
    for (int i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x2;
    }

    // identity map all of the mmap software reserved sections (so i dont have to reserve the kernel myself)
    mmap_entry_t* software_regions = mmap_get_software_regions();

    int i = 0;
    while (software_regions[i].type != 0)
    {
        if (software_regions[i].type != MMAP_REGION_RESERVED)
        {
            i++;
            continue;
        }

        vmm_identity_map(software_regions[i].base_addr, software_regions[i].length, VMM_PAGE_RW, true);
        i++;
    }


    // map page directory
    vmm_identity_map(page_directory_addr, 4096, VMM_PAGE_RW, true);

    // map all the page tables
    for (int i = 0; i < 1024; i++)
    {
        if ((page_directory[i] & 0xFFFFF000) != 0)
            vmm_identity_map(page_directory[i] & 0xFFFFF000, 4096, VMM_PAGE_RW, true);
    }

    enable_paging(page_directory_addr);
}

void vmm_create_page_table(uint32_t directory_index, bool kernel)
{
    if (directory_index >= 1024)
    {
        kprintf(WARN, "VMM: Cant create page table %u!\n", directory_index);
        return;
    }

    uint32_t page_table_addr = pmm_alloc_page();

    if (page_table_addr == PMM_ERROR)
    {
        kprintf(ERROR, "VMM: PMM failed; can't continue!\n");
        return;
    }

    uint32_t* page_table = (uint32_t*)(uintptr_t)page_table_addr;

    // init page table
    for (int i = 0; i < 1024; i++)
    {
        page_table[i] = 0;
    }

    // assign page table
    page_directory[directory_index] = page_table_addr | (0x3 | (!kernel << 2)) | 1;
}

void vmm_auto_map_page(uint32_t phys, uint32_t virt, uint32_t flags, bool kernel)
{
    // printf("%x ", virt);
    if (!IS_BIT_SET(page_directory[DIRECTORY_INDEX(virt)], 0)) // page table doesnt exist
    {
        vmm_create_page_table(DIRECTORY_INDEX(virt), kernel);
    }

    if (((page_directory[DIRECTORY_INDEX(virt)] & 0x4) > 0) == kernel) // its == because kernel bit is 0 when kernel is on
    {
        vmm_create_page_table(DIRECTORY_INDEX(virt), kernel);
    }

    uint32_t* page_table = (uint32_t*)(uintptr_t)(page_directory[DIRECTORY_INDEX(virt)] & 0xFFFFF000);

    // addresses given will be page-aligned
    page_table[TABLE_INDEX(virt)] = phys | (flags | (!kernel << 2)) | 1; // always present, kernel sets user bit
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
        return;
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

void vmm_identity_map(uint32_t start, uint32_t length, uint32_t flags, bool kernel)
{
    vmm_map_addresses(start, start, length, flags, kernel);
}
