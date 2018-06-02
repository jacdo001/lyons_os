#include "lyons/kernel.h"

#include <lyons/text_console.h>
#include <lyons/string.h>

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

static char digits[] =
{
    '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E',
    'F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z'
};

static char digitsl[] =
{
    '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e',
    'f','g','h','i','j','k','l','m','n','o','p','q','r','s','t',
    'u','v','w','x','y','z'
};

enum length_specifier {hh, h, none, l, ll, j, z, t, L};

static void print(const char* rg_data, size_t cb_data)
{
    for ( size_t i = 0; i < cb_data; i++ )
    {
        k_console_put_char(rg_data[i]);
    }
}

static int uint_to_hexstr(char* dst_str, unsigned long long n, bool caps)
{
    unsigned long long base = 16;
    unsigned long long test = n;
    char* p = dst_str;
    char* charset = digitsl;

    if( caps )
    {
        charset = digits;
    }

    do
    {
        ++p;
        test = test / base;
    } while ( test > 0 );

    *p = '\0';

    do
    {
        *--p = charset[n%base];
        n = n / base;
    } while ( n > 0 );

    return p - dst_str;
}

static int int_to_str(char* dst_str, int n)
{
    char* p = dst_str;
    if(n < 0)
    {
        *p++ = '-';
        n *= -1;
    }
    unsigned long base = 10;
    unsigned int test = n;
    do
    {
        ++p;
        test = test / base;
    } while ( test > 0 );
    *p = '\0';
    do
    {
        *--p = digits[n%base];
        n = n / base;
    } while ( n > 0 );
    return p - dst_str;
}

size_t kprintf(const char * __restrict format, ...)
{
    /* NOT STANDARDS COMPLIANT */
    va_list parameters;
    va_start (parameters, format);

    int written = 0;
    size_t amount;
    char buf[99];
    bool rejected_bad_specifier = false;

    while ( *format != '\0' )
    {
        if ( *format != '%' || rejected_bad_specifier)
        {
            amount = 1;
            /* grab as many chars as possible. */
            while ( format[amount] && format[amount] != '%' )
            {
                amount++;
            }
            print(format, amount);
            format += amount;
            written += amount;
            rejected_bad_specifier = false;
        }
        else
        {
            const char* format_start = format;
            format ++;
            bool alt = false;
            enum length_specifier arg_len = none;
            bool format_done = false;
            bool caps = false;

            while( !format_done )
            {
                switch( *format )
                {
                case 'l':
                {
                    if( arg_len != none )
                    {
                        rejected_bad_specifier = true;
                    }
                    if( *++format == 'l' )
                    {
                        format++;
                        arg_len = ll;
                    }
                    else
                    {
                        arg_len = l;
                    }
                    break;
                }
                case '#':
                {
                    format ++;
                    if( alt )
                    {
                        rejected_bad_specifier = true;
                    }
                    else
                    {
                        alt = true;
                    }
                    break;
                }
                case '%':
                {
                    if( arg_len != none )
                    {
                        rejected_bad_specifier = true;
                    }
                    else
                    {
                        print("%", 1);
                        format ++;
                        written ++;
                        format_done = true;
                    }
                    break;
                }
                case 'c':
                {
                    char c = (char)va_arg(parameters, int);
                    print(&c, 1);
                    format ++;
                    written ++;
                    format_done = true;
                    break;
                }
                case 's':
                {
                    format++;
                    const char* s = va_arg(parameters, const char*);
                    print(s, strlen(s));
                    written += strlen(s);
                    format_done = true;
                    break;
                }
                case 'X':
                    caps = true;
                case 'x':
                {
                    format++;
                    unsigned long long x = 0;
                    switch(arg_len)
                    {
                    case none:
                    {
                        x = va_arg(parameters, unsigned int);
                        break;
                    }
                    case ll:
                    {
                        x = va_arg(parameters, unsigned long long);
                        break;
                    }
                    default:
                    {
                        rejected_bad_specifier = true;
                    }
                    }
                    if( !rejected_bad_specifier )
                    {
                        uint_to_hexstr(buf, x, caps);
                        if( alt && caps )
                        {
                            print("0X", 2);
                        }
                        else if (alt)
                        {
                            print("0x", 2);
                        }
                        print(buf, strlen(buf));
                        written += strlen(buf);
                        format_done = true;
                    }
                    break;
                }
                case 'd':
                case 'i':
                {
                    format ++;
                    long long i = 0;
                    switch( arg_len )
                    {
                    case none:
                    {
                        i = va_arg(parameters, int);
                        break;
                    }
                    case ll:
                    {
                        i = va_arg(parameters, long long);
                        break;
                    }
                    default:
                    {
                        rejected_bad_specifier = true;
                    }
                    }
                    if( !rejected_bad_specifier )
                    {
                        int_to_str(buf, i);
                        print(buf, strlen(buf));
                        written += strlen(buf);
                        format_done = true;
                    }
                    break;
                }
                default:
                {
                    rejected_bad_specifier = true;
                    break;
                }
                }

                if( rejected_bad_specifier )
                {
                    format = format_start;
                    format_done = true;
                }
            }
        }
    }

    va_end(parameters);

    return written;
}
