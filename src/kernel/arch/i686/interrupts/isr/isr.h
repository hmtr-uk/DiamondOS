#pragma once

#include "arch/i686/interrupts/idt/idt.h"

void isr_c_handler(idt_regs_t* regs);
