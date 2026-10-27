#pragma once
#include <stdint.h>

typedef struct
{
    uint32_t flags;             // 0
    uint32_t mem_lower;         // 4
    uint32_t mem_upper;         // 8
    uint32_t boot_device;       // 12
    uint32_t cmdline;           // 16
    uint32_t mods_count;        // 20
    uint32_t mods_addr;         // 24

    union
    {
        struct
        {
            uint32_t tabsize;
            uint32_t strsize;
            uint32_t addr;
            uint32_t reserved;
        } aout;

        struct
        {
            uint32_t num;
            uint32_t size;
            uint32_t addr;
            uint32_t shndx;
        } elf;
    } syms;                     // 28–43

    uint32_t mmap_length;       // 44
    uint32_t mmap_addr;         // 48
    uint32_t drives_length;     // 52
    uint32_t drives_addr;       // 56
    uint32_t config_table;      // 60
    uint32_t boot_loader_name;  // 64
    uint32_t apm_table;         // 68

    uint32_t vbe_control_info;  // 72
    uint32_t vbe_mode_info;     // 76
    uint16_t vbe_mode;          // 80
    uint16_t vbe_interface_seg; // 82
    uint16_t vbe_interface_off; // 84
    uint16_t vbe_interface_len; // 86

    uint64_t framebuffer_addr;  // 88
    uint32_t framebuffer_pitch; // 96
    uint32_t framebuffer_width; // 100
    uint32_t framebuffer_height;// 104
    uint8_t  framebuffer_bpp;   // 108
    uint8_t  framebuffer_type;  // 109
    uint8_t  color_info[6];     // 110–115
}  __attribute__((packed)) multiboot_info_t;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} multiboot_mmap_entry_t;

void multiboot_parse(multiboot_info_t* info);
void multiboot_parse_mmap(multiboot_info_t* info);
