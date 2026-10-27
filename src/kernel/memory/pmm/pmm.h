#pragma once
#include <stdint.h>

#define PMM_PAGE_COUNT      (1024 * 1024)
#define PMM_BITMAP_SIZE     (PMM_PAGE_COUNT / 8)

#define PMM_PAGE_SIZE       4096
#define PMM_MAX_ADDR        0x100000000ULL

#define PMM_ERROR           UINT32_MAX

void pmm_init();
uint32_t pmm_alloc_page();
void pmm_free_page(uint32_t addr);
uint32_t pmm_get_free_pages();
