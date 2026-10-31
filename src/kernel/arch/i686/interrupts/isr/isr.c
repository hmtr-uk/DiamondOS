#include "arch/i686/interrupts/isr/isr.h"
#include "arch/i686/interrupts/idt/idt.h"
#include "utils/panic/panic.h"

#include <stdio.h>

void isr_c_handler(idt_regs_t* regs)
{
    // this is part of the interrupt handler
    // but this is called by the stubs, and not the cpu

    kprintf(ERROR, "Unhandled exception %u with error code 0x%x\n", regs->vector, regs->errorcode);
    kprintf(ERROR, PANIC_PRINTF_REGS_FMT, regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esp, regs->ebp, regs->esi, regs->edi);

    boot_panic("Unhandled exception during boot!");
}
