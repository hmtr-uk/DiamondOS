#include "arch/i686/interrupts/idt/idt.h"
#include <stdio.h>
#include <stdint.h>

extern uint32_t isr_stub_table[32];
extern uint32_t irq_stub_table[16];

idt_gate_t idt[256];

idt_ptr_t idt_ptr = {
    .base = (uintptr_t)&idt[0],
    .limit = sizeof(idt) - 1
};

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t flags)
{
    idt[vector].zero = 0;
    idt[vector].offset_low = (uint16_t)handler;
    idt[vector].offset_high = (uint16_t)(handler >> 16);
    idt[vector].selector = selector;
    idt[vector].type_attributes = flags;
}

void idt_init()
{
    // set up isr
    for (int i = 0; i < 32; i++)
        idt_set_gate(i, isr_stub_table[i], 0x08, IDT_GATE_INTERRUPT32);

    idt_lidt(&idt_ptr);
}

void idt_c_unmapped()
{
    // TODO: DO THIS
}
