#include "lyons/elf.h"
#include <lyons/kernel.h>

#define ELF32_IDENTITY_COUNT    16
#define ELF32_MAGIC_COUNT        4

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6
#define PT_LOPROC   0x70000000

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Offset;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

unsigned char magic[ELF32_MAGIC_COUNT] = {0x7F, 'E', 'L', 'F'};
typedef uint8_t fh_class_t;
fh_class_t FH_BIT32 = 0x01, FH_BIT64=0x02;
typedef uint8_t fh_endedness_t;
fh_endedness_t FH_LITTLE = 0x01, FH_BIG = 0x02;
typedef uint8_t fh_version_t;
fh_version_t FH_ORIGINAL = 0x01;
typedef uint8_t fh_abi_t;
fh_abi_t FH_SYSTEM_V = 0x00;
typedef uint16_t fh_type_t;
fh_type_t FH_RELOC = 0x01, FH_EXE = 0x02, FH_SHARE = 0x03, FH_CORE = 0x04;
typedef uint16_t fh_arch_t;
fh_arch_t FH_X86 = 0x03;
typedef uint32_t fh_version_32_t;
fh_version_32_t FH_ORIGINAL_32 = 0x01;

typedef struct {
    unsigned char identity[ELF32_IDENTITY_COUNT];
    Elf32_Half object_type;
    Elf32_Half architecture;
    Elf32_Word version;
    Elf32_Addr entry_point;
    Elf32_Offset program_header_offset;
    Elf32_Offset section_header_offset;
    Elf32_Word flags;
    Elf32_Half elf_header_size;
    Elf32_Half program_header_size;
    Elf32_Half program_header_count;
    Elf32_Half section_header_size;
    Elf32_Half section_header_count;
    Elf32_Half string_table_index;
} Elf32_Elf_Header;

typedef uint32_t ph_type_t;
ph_type_t
PH_NULL         = 0x0,
PH_LOAD         = 0x1,
PH_DYNAMIC      = 0x2,
PH_INTERPRETER  = 0x3,
PH_NOTE         = 0x4;

typedef uint32_t ph_flags_t;
ph_flags_t
PH_EXECUTABLE   = 0x1,
PH_WRITABLE     = 0x2,
PH_READABLE     = 0x4;

typedef struct{
    Elf32_Word      type;
    Elf32_Offset    offset;
    Elf32_Addr      virtual_address;
    Elf32_Addr      physical_address;
    Elf32_Word      file_size;
    Elf32_Word      memsz;
    Elf32_Word      flags;
    Elf32_Word      alignment;
} Elf32_Program_Header;
typedef uint32_t sh_type_t;
sh_type_t
SHT_NULL    = 0x0,
SHT_PROGBITS =0x1,
SHT_SYMTAB  = 0x2,
SHT_STRTAB  = 0x3,
SHT_RELA    = 0x4,
SHT_HASH    = 0x5,
SHT_DYNAMIC = 0x6,
SHT_NOTE    = 0x7,
SHT_NOBITS  = 0x8,
SHT_REL     = 0x9,
SHT_SHLIB   = 0xA,
SHT_DYNSYM  = 0xB,
SHT_LOPROC  = 0x70000000,
SHT_HIPROC  = 0x7fffffff,
SHT_LOUSER  = 0x80000000,
SHT_HIUSER  = 0xffffffff;
typedef uint32_t sh_flags_t;
sh_flags_t
SHF_WRITE       = 0x01,
SHF_ALLOC       = 0x02,
SHF_EXECINSTR   = 0x04,
SHF_MASKPROC    = 0xF0000000;

struct section_header_type
{
    uint32_t    name;
    sh_type_t   type;
    sh_flags_t  flags;
    uint32_t    addr;
    uint32_t    offset;
    uint32_t    size;
    uint32_t    link;
    uint32_t    info;
    uint32_t    addralign;
    uint32_t    entsize;
};

uint32_t k_elf_load(uint32_t base, uint32_t limit)
{
    Elf32_Elf_Header *elf_header = base;
    for(int i = 0; i < ELF32_MAGIC_COUNT; i ++)
    {
        if(elf_header->identity[i] != magic[i])
        {
            kprintf("ELF loader: incorrect magic string %x, %x, %x, %x expected %x, %x, %x, %x.\n",
                    elf_header->identity[0], elf_header->identity[1], elf_header->identity[2],
                    elf_header->identity[3], magic[0], magic[1], magic[2], magic[3]);
            return 0;
        }
    }
    kprintf("ELF header magic string OK\n");

    kprintf("\nProgram headers\n");
    for(int i = 0; i < elf_header->program_header_count; i ++)
    {
        Elf32_Program_Header *prog_header = base + elf_header->program_header_offset *
                elf_header->program_header_size;

        const char* type_string_null = "PT_NULL";
        const char* type_string_load = "PT_LOAD";
        const char* type_string_dynamic = "PT_DYNAMIC";
        const char* type_string_interpreted = "PT_INTERP";
        const char* type_string_note = "PT_NOTE";
        const char* type_string_shlib = "PT_SHLIB";
        const char* type_string_phlib = "PT_PHDR";
        const char* type_string_loproc = ">=PT_LOPROC";

        char* type_string;
        if(prog_header->type < PT_LOPROC)
        {
            switch(prog_header->type)
            {
                case PT_NULL: type_string = type_string_null; break;
                case PT_LOAD: type_string = type_string_load; break;
                case PT_DYNAMIC: type_string = type_string_dynamic; break;
                case PT_INTERP: type_string = type_string_interpreted; break;
                case PT_NOTE: type_string = type_string_note; break;
                case PT_SHLIB: type_string = type_string_shlib; break;
                case PT_PHDR: type_string = type_string_phlib; break;
            }
        }
        else
        {
            type_string = type_string_loproc;
        }

        kprintf("type:%s offset:0x%X vaddr:0x%X paddr:0x%X filesz:0x%X memsz:0x%X flags:0x%X align:0x%X\n",
                type_string,
                prog_header->offset,
                0,
                0,
                0,
                0,
                0,
                0);
    }

    kprintf("\nSection headers\n");
    for(int i = 0; i < elf_header->section_header_count; i ++)
    {
        kprintf("type:%s offset:0x%X vaddr:0x%X paddr:0x%X filesz:0x%X memsz:0x%X flags:0x%X align:0x%X\n",
                "",
                0,
                0,
                0,
                0,
                0,
                0,
                0);
    }

    return elf_header->entry_point;
}
