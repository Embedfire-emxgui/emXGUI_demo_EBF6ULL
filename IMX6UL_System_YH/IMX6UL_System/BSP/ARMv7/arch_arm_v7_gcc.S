				
	.section .text,"ax"

/*============================================================================*/
	.global	mmu_set_mtt_base
mmu_set_mtt_base:

   	mcr p15,0,r0,c2,c0,0
	bx	lr


	.global	mmu_get_mtt_base
mmu_get_mtt_base:

   	mrc p15,0,r0,c2,c0,0
	bx	lr

/*============================================================================*/

	.global	mmu_set_domain
mmu_set_domain:
	mcr	p15, 0, r0, c3, c0
	bx	lr
 
/*============================================================================*/

	.global	mmu_get_prefetch_fault_status
mmu_get_prefetch_fault_status:	//获得预取指错误状态
	mrc p15, 0, r0, c5, c0, 1
	bx	lr

	.global	mmu_get_data_fault_status
mmu_get_data_fault_status:	//获得数据访问错误状态
	mrc p15, 0, r0, c5, c0, 0
	bx	lr

	.global	mmu_get_fault_address
mmu_get_fault_address:		//获得错误地址
	mrc p15, 0, r0, c6, c0, 0
	bx	lr
	
/*============================================================================*/
    .global	mmu_flush_icache
mmu_flush_icache:
    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 0       // invalidate all instruction caches

    mov     r2, #0
    mcr     p15, 0, r2, c7, c5, 4       // prefetch flush to sync the change to the cachesize id reg
    .long     0xf57ff06f                  // ISB (Instruction Synchronization Barrier)
    mcr     p15, 0, r2, c7, c5, 6       // flush entire branch target cache

    .long     0xf57ff04f                  // DSB (Data Synchronization Barrier)
    .long     0xf57ff06f                  // ISB (Instruction Synchronization Barrier)
	bx		lr

/*============================================================================*/

	.global	mmu_flush_dcache
mmu_flush_dcache:
	stmfd   sp!, {r4-r6, r7, r9-r11, lr}    // store off registers to stack

	.long     0xf57ff05f                      // DMB (Data Memory Barrier)

	mrc     p15, 1, r0, c0, c0, 1           // read clidr
	ands    r3, r0, #0x7000000
	mov     r3, r3, lsr #23                 // cache level value
	beq     donea

	mov     r10, #0                         // start clean at cache level 0
loop1a:
	add     r2, r10, r10, lsr #1            // work out 3x current cache level
	mov     r1, r0, lsr r2                  // extract cache type bits from clidr
	and     r1, r1, #7                      // mask of the bits for current cache only
	cmp     r1, #2                          // see what cache we have at this level
	blt     skipa                           // skip if no cache, or just i-cache

	mcr     p15, 2, r10, c0, c0, 0          // select current cache level in cssr
	mov     r1, #0
	//;mcr     p15, 0, r1, c7, c5, 4        // prefetch flush to sync the change to the cachesize id reg
	.long     0xf57ff06f                      // ISB (Instruction Synchronization Barrier)
	mrc     p15, 1, r1, c0, c0, 0           // read the new csidr
	and     r2, r1, #7                      // extract the length of the cache lines
	add     r2, r2, #4                      // add 4 (line length offset)
	ldr     r4, =0x3ff
	ands    r4, r4, r1, lsr #3              // r4 is maximum number on the way size
	clz     r5, r4                          // r5 find bit position of way size increment
	ldr     r7, =0x7fff
	ands    r7, r7, r1, lsr #13             // r7 extract max number of the index size

loop2a:
	mov     r9, r4                          // r9 is working copy of max way size
loop3a:
	orr     r11, r10, r9, lsl r5            // factor way and cache number into r11
	orr     r11, r11, r7, lsl r2            // factor index number into r11

	mcr     p15, 0, r11, c7, c14, 2         // clean and invalidate by set/way

	subs    r9, r9, #1                      // decrement the way
	bge     loop3a

	subs    r7, r7, #1                      // decrement the index
	bge     loop2a

	//;check if L2 cache enable.
	mov		r5, #0x0
	mrc		p15, 0, r5, c1, c0, 1		// read Auxiliary Control Register
	and		r6, r5, #2					// [1] bit indicates L2EN.
	cmp		r6, #2
	bne		donea						// done if L2 cache disable.

skipa:
	add     r10, r10, #2                // increment cache number
	cmp     r3, r10
	bgt     loop1a

donea:
	mov     r10, #0                     // swith back to cache level 0
	mcr     p15, 2, r10, c0, c0, 0      // select current cache level in cssr

	.long     0xF57FF04F              	// DSB (Data Synchronization Barrier)
	.long     0xF57FF06F              	// ISB (Instruction Synchronization Barrier

	ldmfd   sp!, {r4-r6, r7, r9-r11, lr}    // restore registers
	bx		lr

/*============================================================================*/

	.global	mmu_flush_itlb
mmu_flush_itlb:
    mov     r0, #0
    mcr     p15, 0, r0, c8, c5, 0   // flush instruction TLB

    .long     0xF57FF04F              // DSB (Data Synchronization Barrier)
    .long     0xF57FF06F              // ISB (Instruction Synchronization Barrier
	bx		lr

	.global	mmu_flush_dtlb
mmu_flush_dtlb:
    mov     r0, #0
    mcr     p15, 0, r0, c8, c6, 0   // flush data TLB

    .long     0xF57FF04F              // DSB (Data Synchronization Barrier)
    .long     0xF57FF06F              // ISB (Instruction Synchronization Barrier
	bx		lr

/*============================================================================*/

     .global ARM_EnableIRQ
ARM_EnableIRQ:
	mrs	r0,cpsr
	bic	r0,#(1<<7)
	msr	cpsr,r0
	bx	lr

     .global ARM_DisableIRQ
ARM_DisableIRQ:
	mrs	r0,cpsr
	orr	r0,#(1<<7)
	msr	cpsr,r0
	bx	lr

     .global ARM_EnableFIQ
ARM_EnableFIQ:
	mrs	r0,cpsr
	bic	r0,#(1<<6)
	msr	cpsr,r0
	bx	lr

     .global ARM_DisableFIQ
ARM_DisableFIQ:
	mrs	r0,cpsr
	orr	r0,#(1<<6)
	msr	cpsr,r0
	bx	lr
/*============================================================================*/

	.end
