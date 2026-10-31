
bits 32

extern isr_c_handler
extern irq_c_handler
extern idt_c_unmapped

;
; void idt_lidt(idt_ptr* idt);
;
global idt_lidt
idt_lidt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret


global idt_unmapped
idt_unmapped:
    call idt_c_unmapped
    iret


; Exception handlers

%macro ISR_ERRORCODE 1
isr%1:
    push %1
    pusha
    push esp
    call isr_c_handler
    add esp, 4
    popa
    add esp, 8
    iret
%endmacro


%macro ISR_NOERRORCODE 1
isr%1:
    push 0
    push %1
    pusha
    push esp
    call isr_c_handler
    add esp, 4
    popa
    add esp, 8
    iret
%endmacro


; Hardware IRQ handlers

%macro IRQ 1
irq%1:
    push 0
    push %1
    pusha
    push esp
    call irq_c_handler
    add esp, 4
    popa
    add esp, 8
    iret
%endmacro


; CPU exception stubs (0-31)

ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE   8
ISR_NOERRORCODE 9
ISR_ERRORCODE   10
ISR_ERRORCODE   11
ISR_ERRORCODE   12
ISR_ERRORCODE   13
ISR_ERRORCODE   14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_ERRORCODE   17
ISR_NOERRORCODE 18
ISR_NOERRORCODE 19
ISR_NOERRORCODE 20
ISR_NOERRORCODE 21
ISR_NOERRORCODE 22
ISR_NOERRORCODE 23
ISR_NOERRORCODE 24
ISR_NOERRORCODE 25
ISR_NOERRORCODE 26
ISR_NOERRORCODE 27
ISR_NOERRORCODE 28
ISR_NOERRORCODE 29
ISR_NOERRORCODE 30
ISR_NOERRORCODE 31


; Generate IRQ stubs (0-15)

%assign i 0
%rep 16
    IRQ i
    %assign i i+1
%endrep


; Stub tables

section .rodata

global isr_stub_table
isr_stub_table:

%assign i 0
%rep 32
    dd isr%+i
    %assign i i+1
%endrep


global irq_stub_table
irq_stub_table:

%assign i 0
%rep 16
    dd irq%+i
    %assign i i+1
%endrep
