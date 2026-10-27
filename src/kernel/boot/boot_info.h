#pragma once
#include <stdint.h>
#include <stdbool.h>

#define BOOTINFO_MAX_BOOTLOADER_NAME    64
#define BOOTINFO_MAX_CMDLINE_STRING     256

typedef struct
{
    char bootloader_name[BOOTINFO_MAX_BOOTLOADER_NAME];
    bool has_bootloader_name;

    char command_line[BOOTINFO_MAX_CMDLINE_STRING];
    bool has_command_line;

    uint32_t boot_device;
    bool has_boot_device;

    uint32_t module_count;
    void* modules; // TODO: make module descriptor type
    bool has_modules;

    uint32_t elf_section_count;
    uint32_t elf_section_size;
    uint64_t elf_section_addr;
    uint32_t elf_string_index;
    bool has_elf;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    bool has_framebuffer;
} boot_info_t;

boot_info_t* get_boot_info();
