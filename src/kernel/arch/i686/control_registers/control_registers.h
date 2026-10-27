#pragma once

#include <stdint.h>

uint32_t read_cr3();
void write_cr3(uint32_t cr3);
