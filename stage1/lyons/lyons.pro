TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    kernel/kernel_main.c \
    drivers/text_console/text_console.c \
    kernel/kprintf.c \
    lib/string.c \
    kernel/kpanic.c \
    arch/i386/proc.c \
    arch/i386/interrupt.c \
    arch/i386/exception.c \
    arch/i386/pmem_manager.c \
    arch/i386/vmem_manager.c \
    arch/i386/syscall.c \
    arch/i386/elf.c

OTHER_FILES += \
    arch/i386/boot.S \
    Makefile \
    arch/i386/make.config \
    arch/i386/set_tss.S \
    arch/i386/set_idt.S \
    arch/i386/set_gdt.S \
    arch/i386/set_data.S \
    arch/i386/set_code.S \
    arch/i386/linker.ld \
    arch/i386/crtn.S \
    arch/i386/crti.S

INCLUDEPATH = \
    $$PWD/include \
    $$PWD

HEADERS += \
    include/lyons/text_console.h \
    include/lyons/kernel.h \
    include/lyons/string.h \
    include/grub/multiboot.h \
    include/lyons/proc.h \
    include/lyons/interrupt.h \
    include/lyons/syscall.h \
    include/lyons/exception.h \
    include/lyons/pmem_manager.h \
    include/lyons/vmem_manager.h \
    include/lyons/elf.h
