#include "lyons/text_console.h"

#include <stddef.h>

static size_t terminal_row;
static size_t terminal_column;
static color_t terminal_fore;
static color_t terminal_back;
static uint16_t* terminal_buffer;

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

void scroll();
uint16_t pack_entry(char c, color_t fore, color_t back);

void k_console_initialize()
{
    terminal_buffer = (uint16_t*) 0xC00B8000;
    k_console_set_color(COLOR_WHITE, COLOR_BLACK);
    k_console_clear();
}

void k_console_set_color(color_t fore, color_t back)
{
    terminal_fore = fore;
    terminal_back = back;
}

void k_console_line_feed()
{
    terminal_column = 0;
    if ( ++terminal_row == VGA_HEIGHT )
    {
        terminal_row --;
        scroll();
    }
}

void k_console_put_char(char c)
{
    if ( c == '\n' )
    {
        k_console_line_feed();
    }
    else
    {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = pack_entry(c, terminal_fore, terminal_back);
        if ( ++terminal_column == VGA_WIDTH )
        {
            k_console_line_feed();
        }
    }
}

void k_console_clear()
{
    terminal_row = 0;
    terminal_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y ++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x ++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = pack_entry(' ', terminal_fore, terminal_back);
        }
    }
}

void scroll()
{
    uint16_t* read_head = terminal_buffer + VGA_WIDTH;
    uint16_t* write_head = terminal_buffer;
    while (read_head < terminal_buffer + (VGA_HEIGHT * VGA_WIDTH))
    {
        *write_head = *read_head;
        write_head ++;
        read_head ++;
    }
    while (write_head < terminal_buffer + (VGA_HEIGHT * VGA_WIDTH))
    {
        *write_head = pack_entry(' ', terminal_fore, terminal_back);
        write_head ++;
    }
}

uint16_t pack_entry(char c, color_t fore, color_t back)
{
    return (c & 0xFF) | ((fore | (back << 4)) << 8);
}
