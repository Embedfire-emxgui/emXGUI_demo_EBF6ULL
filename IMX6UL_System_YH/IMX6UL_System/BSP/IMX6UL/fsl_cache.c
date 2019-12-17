

#include "fsl_cache.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void ICACHE_InvalidateByRange(uint32_t address, uint32_t size_byte)
{
    L1CACHE_InvalidateICacheByRange(address, size_byte);
}

void DCACHE_InvalidateByRange(uint32_t address, uint32_t size_byte)
{
    L1CACHE_InvalidateDCacheByRange(address, size_byte);
}

void DCACHE_CleanByRange(uint32_t address, uint32_t size_byte)
{
    L1CACHE_CleanDCacheByRange(address, size_byte);
}

void DCACHE_CleanInvalidateByRange(uint32_t address, uint32_t size_byte)
{
    L1CACHE_CleanInvalidateDCacheByRange(address, size_byte);
}
