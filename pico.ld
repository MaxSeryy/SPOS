MEMORY
{
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 264K
}

ENTRY(_start)

SECTIONS
{
    .text : {
        . = ALIGN(4);
        *(.entry)     
        
        /* ВАЖЛИВО: ARM вимагає, щоб таблиця векторів була вирівняна на 256 байт! */
        . = ALIGN(256);
        *(.vectors)   
        
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > RAM

    .data : {
        . = ALIGN(4);
        *(.data*)
        . = ALIGN(4);
    } > RAM

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
}