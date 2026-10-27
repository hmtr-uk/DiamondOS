#include "memory/pmm/pmm.h"
#include "memory/mmap/mmap.h"
#include "utils/math.h"
#include <stdint.h>
#include <stdio.h>

static uint8_t pages[PMM_BITMAP_SIZE];

uint32_t free_pages;

void pmm_internal_free_page(uint32_t page);
void pmm_internal_reserve_page(uint32_t page);

void pmm_init()
{
    free_pages = 0;
    for (int i = 0; i < PMM_BITMAP_SIZE; i++)
    {
        pages[i] = 0xFF;
    }

    // free memory firmware says is ok
    mmap_entry_t* firmware_regions = mmap_get_firmware_regions();

    // you have to free memory, then reserve it so all the reservations have the most priority.
    // this means we free firmware memory, then reserve firmware memory, then reserve software memory.

    // free firmware memory
    int i = 0;
    while (firmware_regions[i].type != 0) // 0 means an unused firmware region slot. genuinely. go check!
    {
        if (firmware_regions[i].type == MMAP_REGION_FREE)
        {
            uint64_t start = firmware_regions[i].base_addr;
            uint64_t end = start + firmware_regions[i].length;

            if (firmware_regions[i].length > UINT64_MAX - start)
            {
                kprintf(ERROR, "PMM: Firmware region exceeds memory bounds!\n");
                i++;
                continue;
            }

            if (start >= PMM_MAX_ADDR)
            {
                i++;
                continue;
            }

            if (end >= PMM_MAX_ADDR)
            {
                end = PMM_MAX_ADDR;
            }

            uint32_t start_page = ALIGN_UP(start, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;
            uint32_t end_page = ALIGN_DOWN(end, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;

            // if these are the same its too small
            if (start_page >= end_page)
            {
                i++;
                continue;
            }

            // now we free from start to end - 1
            for (uint32_t p = start_page; p < end_page; p++)
            {
                pmm_internal_free_page(p);
            }
        }
        i++;
    }

    i = 0;

    // reserve firmware memory
    while (firmware_regions[i].type != 0) // 0 means an unused firmware region slot. genuinely. go check!
    {
        if (firmware_regions[i].type != MMAP_REGION_FREE)
        {
            uint64_t start = firmware_regions[i].base_addr;
            uint64_t end = start + firmware_regions[i].length;

            if (firmware_regions[i].length > UINT64_MAX - start)
            {
                kprintf(ERROR, "PMM: Firmware region exceeds memory bounds!\n");
                i++;
                continue;
            }

            if (start >= PMM_MAX_ADDR)
            {
                i++;
                continue;
            }

            if (end >= PMM_MAX_ADDR)
            {
                end = PMM_MAX_ADDR;
            }

            uint32_t start_page = ALIGN_DOWN(start, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;
            uint32_t end_page = ALIGN_UP(end, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;

            // now we free from start to end - 1
            for (uint32_t p = start_page; p < end_page; p++)
            {
                pmm_internal_reserve_page(p);
            }
        }
        i++;
    }

    mmap_entry_t* software_regions = mmap_get_software_regions();

    i = 0;

    // reserve software memory
    while (software_regions[i].type != 0)
    {
        if (software_regions[i].type != MMAP_REGION_FREE)
        {
            uint64_t start = software_regions[i].base_addr;
            uint64_t end = start + software_regions[i].length;

            if (software_regions[i].length > UINT64_MAX - start)
            {
                kprintf(ERROR, "PMM: Software region exceeds memory bounds!\n");
                i++;
                continue;
            }

            if (start >= PMM_MAX_ADDR)
            {
                i++;
                continue;
            }

            if (end >= PMM_MAX_ADDR)
            {
                end = PMM_MAX_ADDR;
            }

            uint32_t start_page = ALIGN_DOWN(start, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;
            uint32_t end_page = ALIGN_UP(end, PMM_PAGE_SIZE) / PMM_PAGE_SIZE;

            // now we free from start to end - 1
            for (uint32_t p = start_page; p < end_page; p++)
            {
                pmm_internal_reserve_page(p);
            }
        }
        i++;
    }

    pmm_internal_reserve_page(0); // always reserve page 0
}

void pmm_internal_free_page(uint32_t page)
{
    if (page > PMM_PAGE_COUNT)
    {
        return;
    }
    uint32_t addr = page / 8;
    uint32_t bit  = page % 8;

    uint8_t before = pages[addr];
    pages[addr] &= ~(1 << bit);

    if (before != pages[addr]) // this function actually did something
        free_pages++;
}

void pmm_internal_reserve_page(uint32_t page)
{
    if (page > PMM_PAGE_COUNT)
    {
        return;
    }
    uint32_t addr = page / 8;
    uint32_t bit  = page % 8;

    uint8_t before = pages[addr];
    pages[addr] |= (1 << bit);

    if (before != pages[addr]) // this function actually did something
        free_pages--;
}

uint32_t pmm_get_free_pages()
{
    return free_pages;
}

uint32_t pmm_alloc_page()
{
    for (uint32_t page = 1; page < PMM_PAGE_COUNT; page++)
    {
        uint32_t addr = page / 8;
        uint32_t bit  = page % 8;

        if ((pages[addr] & (1 << bit)) == 0)
        {
            pmm_internal_reserve_page(page);
            return page * PMM_PAGE_SIZE;
        }
    }

    // no free pages!
    kprintf(FATAL, "A FAULT WILL OCCOUR SOON, ALL PAGES ARE FILLED!!\n");
    return PMM_ERROR;
}

void pmm_free_page(uint32_t addr)
{
    if (addr == PMM_ERROR)
    {
        kprintf(ERROR, "PMM: Given ERROR address");
        return;
    }

    if (addr % PMM_PAGE_SIZE != 0)
    {
        kprintf(ERROR, "PMM: Given incorrectly offset address");
        return;
    }

    uint32_t page = addr / PMM_PAGE_SIZE;

    if (page > PMM_PAGE_COUNT)
    {
        kprintf(ERROR, "PMM: Given page above page count!");
        return;
    }

    pmm_internal_free_page(page);
}
