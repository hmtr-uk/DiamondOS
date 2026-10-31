#pragma once

#include <stdint.h>

#define IDT_GATE_INTERRUPT32    0x8E

typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attributes;
    uint16_t offset_high;
}  __attribute__((packed)) idt_gate_t;

typedef struct
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t vector;
    uint32_t errorcode;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed)) idt_regs_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t flags);
void idt_init();
void idt_lidt(idt_ptr_t* idt);
void idt_c_unmapped();
