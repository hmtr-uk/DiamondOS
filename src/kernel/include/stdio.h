#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

typedef enum
{
    INFO,
    WARN,
    ERROR,
    FATAL
} log_level_t;

typedef enum
{
    VGA_BLACK         = 0,
    VGA_BLUE          = 1,
    VGA_GREEN         = 2,
    VGA_CYAN          = 3,
    VGA_RED           = 4,
    VGA_MAGENTA       = 5,
    VGA_BROWN         = 6,
    VGA_LIGHT_GREY    = 7,
    VGA_DARK_GREY     = 8,
    VGA_LIGHT_BLUE    = 9,
    VGA_LIGHT_GREEN   = 10,
    VGA_LIGHT_CYAN    = 11,
    VGA_LIGHT_RED     = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW        = 14,
    VGA_WHITE         = 15
} vga_colour_t;

#define MAKE_COLOR(fg, bg) ((bg << 4) | fg)

void set_color(uint8_t color);

void clrscr();
void putc(char c);
void puts(const char* str);
void putchr(int x, int y, char c);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);

void kprintf(log_level_t level, const char* fmt, ...);
void vprintf(const char* fmt, va_list args);
