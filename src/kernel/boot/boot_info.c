#include "boot/boot_info.h"

static boot_info_t info;

boot_info_t* get_boot_info()
{
    return &info;
}
