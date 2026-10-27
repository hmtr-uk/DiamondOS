#include "arch/i686/multiboot/multiboot.h"
#include "utils/binary.h"
#include "utils/panic/panic.h"
#include "memory/mmap/mmap.h"
#include "boot/boot_info.h"
#include <stdint.h>
#include <stdio.h>


void multiboot_parse(multiboot_info_t* info)
{
    boot_info_t* boot_info = get_boot_info();

    // memory info
    if (IS_BIT_SET(info->flags, 6))
    {
        multiboot_parse_mmap(info);
    }
    else
    {
        kernel_panic("Boot cannot continue safely!\nMemory map info is not found.");
    }

    // i dont want to use basic memory info, because its SHIT.
    // if (IS_BIT_SET(info->flags, 0))
    // {
    //     kprintf(INFO, "Basic memory information is available\n");
    // }


    if (IS_BIT_SET(info->flags, 1))
    {
        boot_info->boot_device = info->boot_device;
        boot_info->has_boot_device = true;
    }
    else
    {
        kprintf(WARN, "No boot device info found!\n");
    }

    if (IS_BIT_SET(info->flags, 2))
    {
        const char* cmdline = (const char*)info->cmdline;
        int i = 0;
        while (*(cmdline + i))
        {
            if (i >= BOOTINFO_MAX_CMDLINE_STRING - 1)
            {
                kprintf(WARN, "Command line string over %d characters long!", BOOTINFO_MAX_CMDLINE_STRING);
                break;
            }
            *(boot_info->command_line + i) = *(cmdline + i);
            i++;
        }
        *(boot_info->command_line + i) = '\0';
        boot_info->has_command_line = true;
    }
    else
    {
        kprintf(WARN, "No command line info found!\n");
    }

    if (IS_BIT_SET(info->flags, 3))
    {
        boot_info->modules = (void*)info->mods_addr;
        boot_info->module_count = info->mods_count;
    }
    else
    {
        kprintf(WARN, "No boot module info found!\n");
    }
    if (IS_BIT_SET(info->flags, 5))
    {
        boot_info->elf_section_count = info->syms.elf.num;
        boot_info->elf_section_size = info->syms.elf.size;
        boot_info->elf_section_addr = info->syms.elf.addr;
        boot_info->elf_string_index = info->syms.elf.shndx;
        boot_info->has_elf = true;
    }

    // dont need these
    // if (IS_BIT_SET(info->flags, 7))
    // {
    //     kprintf(INFO, "Drive information is available\n");
    // }
    // if (IS_BIT_SET(info->flags, 8))
    // {
    //     kprintf(INFO, "Configuration information is available\n");
    // }
    //
    if (IS_BIT_SET(info->flags, 9))
    {
        const char* bootname = (const char*)info->boot_loader_name;
        int i = 0;
        while (*(bootname + i))
        {
            if (i >= BOOTINFO_MAX_BOOTLOADER_NAME - 1)
            {
                kprintf(WARN, "Bootloader name string over %d characters long!", BOOTINFO_MAX_BOOTLOADER_NAME);
                break;
            }
            *(boot_info->bootloader_name + i) = *(bootname + i);
            i++;
        }
        *(boot_info->bootloader_name + i) = '\0';
        boot_info->has_bootloader_name = true;
    }
    // if (IS_BIT_SET(info->flags, 10))
    // {
    //     kprintf(INFO, "APM information is available\n");
    // }
    // if (IS_BIT_SET(info->flags, 11))
    // {
    //     kprintf(INFO, "Video information is available\n");
    // }

    if (IS_BIT_SET(info->flags, 12))
    {
        boot_info->framebuffer_addr = info->framebuffer_addr;
        boot_info->framebuffer_bpp = info->framebuffer_bpp;
        boot_info->framebuffer_height = info->framebuffer_height;
        boot_info->framebuffer_width = info->framebuffer_width;
        boot_info->framebuffer_pitch = info->framebuffer_pitch;
        boot_info->framebuffer_type = info->framebuffer_type;
        boot_info->has_framebuffer = true;

    }
    else
    {
        kprintf(ERROR, "No framebuffer found!!\n");
        kprintf(ERROR, "The kernel will resort to VGA text mode.\n");
    }
}

void multiboot_parse_mmap(multiboot_info_t* info)
{
    uint32_t length = info->mmap_length;
    uint32_t addr = info->mmap_addr;

    uint32_t starting_addr = addr;

    while (addr < (starting_addr + length))
    {
        uint32_t size = *((uint32_t*)addr); // the value at address is size
        addr += 4; // skip size variable
        multiboot_mmap_entry_t entry = *((multiboot_mmap_entry_t*)addr);

        mmap_register_firmware_region(entry.base_addr, entry.length, entry.type);

        addr += size;
    }
}
