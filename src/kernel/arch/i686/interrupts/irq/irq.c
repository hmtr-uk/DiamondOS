#include "arch/i686/interrupts/irq/irq.h"
#include "arch/i686/interrupts/idt/idt.h"

void irq_c_handler(idt_regs_t* regs)
{
    // this is part of the interrupt handler
    // but this is called by the stubs, and not the cpu
}
