
INCLUDE libs.lds

/*============================================================================*/	 


MEMORY
{	
	FlashHeader(RX)	: ORIGIN  = 0x80000000, LENGTH  = 0xC00
	ROM1(RX)		: ORIGIN  = 0x80000C00, LENGTH  = 3M-0xC00
	RAM1(RXW)		: ORIGIN  = 0x80300000, LENGTH  = 1M	
	OCSRAM(RXW)		: ORIGIN  = 0xE0900000, LENGTH  = 128K
	
	SDRAM_WT(RXW)	: ORIGIN  = 0x80000000+128M, LENGTH  = 1024K
	
}

ENTRY(isr_vector)

/*============================================================================*/	 

SECTIONS
{

	__header :
	{
		
	KEEP( * (.flasheader))

	}>FlashHeader

	__text_0 :
	{
		
		__text_base = .;
			 
		KEEP( * (.isr_vector))
		KEEP( * (._start))
		*/startup/*.o (.text .text.* .rodata .rodata.*)			
		
		
		__text_end = .;		

	}>ROM1
	
	__text_copy : ALIGN(4)
	{
	
	
		*clock_config_ocram.o (.text .text.* .rodata .rodata.*)
	
/*
		*GL_*.o (.text .text.* .rodata .rodata.*)		
		*GUI_Message.o (.text .text.* .rodata .rodata.*)		
		*GUI_Window.o (.text .text.* .rodata .rodata.*)
		*GUI_Region.o (.text .text.* .rodata .rodata.*)

		*libMicroKernel_GCC_ARM7.a: (.text .text.* .rodata .rodata.*)
		
		*libKernel_GCC_ARM7.a: (.text .text.* .rodata .rodata.*) 

		*libEMXGUI_GCC_ARM7.a: (.text .text.*)
*/

	/*
		*GUI_DEMO*.o (.text .text.*) 
	*/
	
	}>OCSRAM AT >ROM1
	
	__text_others : ALIGN(4)
	{
		* (.text .text.* .rodata .rodata.*)	
	
	} >ROM1
	
	
	__data_copy :
	{
		__data_base = ALIGN(8);	
			
		* (.data .data.*)
		
		__data_end = ALIGN(8);
	}>RAM1 AT >ROM1
	

	__bss_clr (NOLOAD) :
	{
		__bss_base = ALIGN(8);
		
		* (.bss .bss.*)	
	
		__bss_end  = ALIGN(8);
		
	}>RAM1 
	
	__non_cache_init  : ALIGN(4)
	{	
		__noncachedata_start__ = .;	
           	
		 *(NonCacheable.init)
		 		 
	} >SDRAM_WT AT >ROM1
	
	__non_cache (NOLOAD) : ALIGN(4)
	{    	
		 *(NonCacheable)
		 __noncachedata_end__ = ALIGN(4);	
		 	 
	} >SDRAM_WT
	
	ARM_EXIDX : 
	{
	     __exidx_start = .;
	      *(.ARM.exidx*)
	     __exidx_end = .;
	} > ROM1
	
/*============================================================================*/	 

	.copy.table :
	{
		. = ALIGN(4);
		__copy_tabel_start__ = .;
		
		/****/
		LONG(ADDR(__text_copy))
		LONG(LOADADDR(__text_copy))
		LONG(SIZEOF(__text_copy))
		
		/****/
		LONG(ADDR(__data_copy))
		LONG(LOADADDR(__data_copy))
		LONG(SIZEOF(__data_copy))
		
		__copy_tabel_end__ = .;
		
	} > ROM1
	
	.clr.table :
	{
		. = ALIGN(4);
		__clr_tabel_start__ = .;
		
		/****/
		LONG(ADDR(__bss_clr))
		LONG(SIZEOF(__bss_clr))
		
		__clr_tabel_end__ = .;
		
	} > ROM1
	
	
/*============================================================================*/	 



/*============================================================================*/	 
	
	
}

/**** The End ****/
