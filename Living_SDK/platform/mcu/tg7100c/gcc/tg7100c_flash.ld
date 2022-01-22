/****************************************************************************************
* @file map.txt
*
* @brief This file is the map file (gnuarm or armgcc).
*
* Copyright (C) BouffaloLab 2018
*
****************************************************************************************
*/

/* configure the CPU type */
OUTPUT_ARCH( "riscv" )
/* link with the standard c library */
INPUT(-lc)
/* link with the standard GCC library */
INPUT(-lgcc)
/* configure the entry point */
ENTRY(_enter)

StackSize = 0x0400; /*  1KB */

MEMORY
{
    xip_memory (rx) : ORIGIN = 0x23000000, LENGTH = 1024K
    itcm_memory (rx) : ORIGIN = 0x22010000, LENGTH = 16K
    ocram_memory (!rx) :   ORIGIN = 0x42020000, LENGTH = 128K-4
}

SECTIONS
{
    PROVIDE(__metal_chicken_bit = 0);
    
    .text :
    {
        KEEP (*(.text.metal.init.enter))
		    KEEP (*(SORT_NONE(.init)))
        
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        
    } > xip_memory

    . = ALIGN(4);
    __etext0 = .;

	.tcm_code_region : AT (__etext0)
	{
		. = ALIGN(4);
		__tcm_code_start__ = .;
        
		*(.tcm_code)
		*(.tcm_const)
		*(.sclock_rlt_code)
		*(.sclock_rlt_const)
        
		. = ALIGN(4);
		__tcm_code_end__ = .;

	} > itcm_memory
    
    __etext1 = __etext0 + SIZEOF(.tcm_code_region);

	.ocram_data_region : AT (__etext1)
	{
        . = ALIGN(4);
		__ocram_data_start__ = .;
        
		*(.system_ram)
        
		. = ALIGN(4);
		__ocram_data_end__ = .;

	} > ocram_memory
    
    __etext2 = __etext1 + SIZEOF(.ocram_data_region);
    
    /* Data section */
    RAM_DATA : AT (__etext2)
    {
        . = ALIGN(4);
        PROVIDE( __global_pointer$ = . + 0x800 );
        __data_start__ = .;
        
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        *(.sdata2)
        *(.sdata2.*)
        
        . = ALIGN(4);
        __data_end__ = .;
    } > ocram_memory
    
    .bss (NOLOAD) :
	{
		. = ALIGN(4);
		__bss_start__ = .;
        
		*(.bss*)
        *(.sbss*)
		*(COMMON)
        
		. = ALIGN(4);
		__bss_end__ = .;
	} > ocram_memory
    
    .noinit_data (NOLOAD) :
	{
		. = ALIGN(4);        
		*(.noinit_data*)
		. = ALIGN(4);
	} > ocram_memory

    
	.heap (NOLOAD):
	{
        . = ALIGN(4);
		__HeapBase = .;
        
		/*__end__ = .;*/
		/*end = __end__;*/
		KEEP(*(.heap*))
        
        . = ALIGN(4);
		__HeapLimit = .;
		PROVIDE( heap_len = __HeapLimit - __HeapBase);
	} > ocram_memory
    

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (NOLOAD):
	{
        . = ALIGN(0x4);
        . = . + StackSize;
        . = ALIGN(0x4);
        PROVIDE( _sp = . );
	} > ocram_memory

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(ocram_memory) + LENGTH(ocram_memory);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
    
}

