#include "memory/mmap/mmap.h"
#include "utils/panic/panic.h"
#include "boot/boot_info.h"
#include <stdint.h>
#include <stdio.h>

extern uint8_t __kernel_start[];
extern uint8_t __kernel_end[];

uintptr_t kernel_start = (uintptr_t)__kernel_start;
uintptr_t kernel_end = (uintptr_t)__kernel_end;

static mmap_entry_t firmware_regions[MMAP_MAX_FIRMWARE_REGIONS];
static mmap_entry_t software_regions[MMAP_MAX_SOFTWARE_REGIONS];
int firmware_region_count = 0;
int software_region_count = 0;

void mmap_register_firmware_region(uint64_t addr, uint64_t length, uint32_t type)
{
    if (firmware_region_count >= MMAP_MAX_FIRMWARE_REGIONS)
    {
        kernel_panic("Boot cannot continue safely!\nThe memory map can't be finished, or it'll overflow!"); // wont return
    }

    if (type > 5 || type < 1)
    {
        kprintf(ERROR, "Region type %u invalid!\n", type);
        type = MMAP_REGION_RESERVED;
    }

    if (length == 0 || addr > UINT64_MAX - length)
    {
        kprintf(ERROR, "Length 0x%llx invalid or will overflow 0x%llx past UINT64_MAX\n", length, addr);
        return;
    }

    mmap_entry_t entry = {
        .base_addr = addr,
        .length = length,
        .type = type
    };

    firmware_regions[firmware_region_count] = entry;
    firmware_region_count++;
}

void mmap_register_software_region(uint64_t addr, uint64_t length, uint32_t type)
{
    if (software_region_count >= MMAP_MAX_SOFTWARE_REGIONS)
    {
        kernel_panic("Boot cannot continue safely!\nThe memory map can't be finished, or it'll overflow!"); // wont return
    }

    if (type > 5 || type < 1)
    {
        kprintf(ERROR, "Region type %u invalid!\n", type);
        type = MMAP_REGION_RESERVED;
    }

    if (length == 0 || addr > UINT64_MAX - length)
    {
        kprintf(ERROR, "Length 0x%llx invalid or will overflow 0x%llx past UINT64_MAX\n", length, addr);
        return;
    }

    mmap_entry_t entry = {
        .base_addr = addr,
        .length = length,
        .type = type
    };

    software_regions[software_region_count] = entry;
    software_region_count++;
}

mmap_entry_t* mmap_get_software_regions()
{
    return software_regions;
}

mmap_entry_t* mmap_get_firmware_regions()
{
    return firmware_regions;
}

void mmap_init()
{
    boot_info_t *boot_info = get_boot_info();

    // the kernel itself!
    mmap_register_software_region(kernel_start, kernel_end - kernel_start, MMAP_REGION_RESERVED);

    // framebuffer
    if (boot_info->has_framebuffer)
        mmap_register_software_region(boot_info->framebuffer_addr, ((uint64_t)boot_info->framebuffer_pitch * (uint64_t)boot_info->framebuffer_height), MMAP_REGION_RESERVED);

    // vga text output
    mmap_register_software_region(0xB8000, 0x1000, MMAP_REGION_RESERVED);
}








