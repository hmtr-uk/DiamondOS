bits 32

global load_gdt_ptr
load_gdt_ptr:
    mov eax, [esp + 4] ; pointer to the GDTR structure
    lgdt [eax]

    ; Reload CS using a far jump
    jmp 0x08:.reload_segments

.reload_segments:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret
