ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.text.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        _sidata = .;
    } > FLASH

    .data : AT (_sidata)
    {
        _sdata = .;
        *(.data)
        *(.data*)
        _edata = .;
    } > RAM

    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM
}
