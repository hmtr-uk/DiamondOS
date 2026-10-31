#include "arch/i686/gdt/gdt.h"

const static gdt_t gdt = {
    .null = {
        .limit_low = 0,
        .base_low = 0,
        .base_middle = 0,
        .access = 0,
        .granularity = 0,
        .base_high = 0
    },
    .kernel_code = {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x9A,
        .granularity = 0xCF,
        .base_high = 0
    },
    .kernel_data = {
        .limit_low = 0xFFFF,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x92,
        .granularity = 0xCF,
        .base_high = 0
    }
};

static gdt_ptr_t gdt_ptr = {
    .base = (uintptr_t)&gdt,
    .limit = sizeof(gdt) - 1
};

void gdt_init()
{
    load_gdt_ptr(&gdt_ptr);
}
