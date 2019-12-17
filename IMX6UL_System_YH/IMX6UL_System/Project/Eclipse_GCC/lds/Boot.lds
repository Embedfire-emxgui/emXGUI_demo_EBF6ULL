
/*============================================================================*/	 

MEMORY
{
	ROM1(RX)		: ORIGIN  = 0x80300000, LENGTH  = 128K
	RAM1(RXW)		: ORIGIN  = 0x80320000, LENGTH  = 512K
}

ENTRY(isr_vector)

/*============================================================================*/	 

SECTIONS
{
	__text_area :
	{
		
		__text_base = .;
		
		KEEP( * (.isr_vector))
		KEEP( * (._start))
		*/startup/*.o (.text .text.* .rodata .rodata.*)			
		* (.text .text.* .rodata .rodata.*)
		
		__text_end = .;		

	}>ROM1 
	
	__data_area :
	{
		__data_base = ALIGN(8);	
		
		* (.data .data.*)
		
		__data_end = ALIGN(8);
	}>RAM1 AT >ROM1
	

	__bss_area (NOLOAD) :
	{
		__bss_base = ALIGN(8);
		
		* (.bss .bss.*)	
	
		__bss_end  = ALIGN(8);
		
	}>RAM1 
	
	ARM_EXIDX : 
	{
	     __exidx_start = .;
	      *(.ARM.exidx*)
	     __exidx_end = .;
	} > ROM1
	
/*============================================================================*/	 

	/*__initial_sp = __bss_end + 256;*/
	__data_load_addr = LOADADDR(__data_area);
	
	.copy.table :
	{
		. = ALIGN(4);
		__copy_tabel_start__ = .;
		
		
		LONG(ADDR(__data_area))
		LONG(LOADADDR(__data_area))
		LONG(SIZEOF(__data_area))
	
		__copy_tabel_end__ = .;
		
	} > ROM1
	
	.clr.table :
	{
		. = ALIGN(4);
		__clr_tabel_start__ = .;
		
		LONG(ADDR(__bss_area))
		LONG(SIZEOF(__bss_area))
		
		__clr_tabel_end__ = .;
		
	} > ROM1
	
/*============================================================================*/	 
	
	
}

/**** The End ****/
