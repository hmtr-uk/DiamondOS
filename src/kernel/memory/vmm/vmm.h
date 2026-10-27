#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DIRECTORY_INDEX(x)  ((x) >> 22)
#define TABLE_INDEX(x)      (((x) >> 12) & 0x3FF)
#define TABLE_OFFSET(x)     ((x) & 0xFFF)

void vmm_init();
void vmm_map_addresses(uint32_t start, uint32_t vstart, uint32_t length, uint32_t flags, bool kernel);
void vmm_auto_map_page(uint32_t phys, uint32_t virt, uint32_t flags, bool kernel);
void vmm_create_page_table(uint32_t directory_index, bool kernel);
