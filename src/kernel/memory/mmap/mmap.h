#pragma once

#define MMAP_MAX_FIRMWARE_REGIONS   256 // dont need 256, just a precaution
#define MMAP_MAX_SOFTWARE_REGIONS   32

#include <stdint.h>
typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} mmap_entry_t;

typedef enum {
    MMAP_REGION_FREE = 1,
    MMAP_REGION_RESERVED,
    MMAP_REGION_ACPI_RECLAIMABLE,
    MMAP_REGION_ACPI_NVS,
    MMAP_REGION_FAULTY
} mmap_region_types;

void mmap_register_software_region(uint64_t addr, uint64_t length, uint32_t type);
void mmap_register_firmware_region(uint64_t addr, uint64_t length, uint32_t type);
mmap_entry_t* mmap_get_firmware_regions();
mmap_entry_t* mmap_get_software_regions();

void mmap_init();
