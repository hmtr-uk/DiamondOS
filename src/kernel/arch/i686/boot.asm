bits 32

MB_MAGIC    equ 0x1BADB002
MB_FLAGS    equ 0x00000003
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
align 4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .bss
align 16
stack_bottom: resb 16384
stack_top:

section .text
global _start
extern kernel_main
extern __bss_start
extern __bss_end

_start:
    cli

    ; preserve multiboot values
    mov esi, eax
    mov ebp, ebx

    ; clear bss
    cld
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    mov esp, stack_top

    push ebp
    push esi
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
