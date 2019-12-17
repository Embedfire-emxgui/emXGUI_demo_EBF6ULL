#ifndef		__IMX_COMMON_H__
#define		__IMX_COMMON_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>

/* CMSIS-style register definitions */
#include "MCIMX6Y7.h"
/* CPU specific feature definitions */
#include "MCIMX6Y7_features.h"

#include "bsp.h"

/*=========================================================================================*/
#ifndef	__I
#define __I  volatile
#endif

#ifndef	__O
#define __O  const volatile
#endif

#ifndef	__IO
#define __IO volatile
#endif

#define assert(__e)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*! @brief Construct a status code value from a group and code number. */
#define MAKE_STATUS(group, code) ((((group)*100) + (code)))

/*=========================================================================================*/
/*=========================================================================================*/
#define	FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL (1)
#define	CPU_MCIMX6Y7DVK05 (1)


/*=========================================================================================*/
/*! @brief Status group numbers. */
enum _status_groups
{
    kStatusGroup_Generic = 0,                 /*!< Group number for generic status codes. */
    kStatusGroup_FLASH = 1,                   /*!< Group number for FLASH status codes. */
    kStatusGroup_LPSPI = 4,                   /*!< Group number for LPSPI status codes. */
    kStatusGroup_FLEXIO_SPI = 5,              /*!< Group number for FLEXIO SPI status codes. */
    kStatusGroup_DSPI = 6,                    /*!< Group number for DSPI status codes. */
    kStatusGroup_FLEXIO_UART = 7,             /*!< Group number for FLEXIO UART status codes. */
    kStatusGroup_FLEXIO_I2C = 8,              /*!< Group number for FLEXIO I2C status codes. */
    kStatusGroup_LPI2C = 9,                   /*!< Group number for LPI2C status codes. */
    kStatusGroup_UART = 10,                   /*!< Group number for UART status codes. */
    kStatusGroup_I2C = 11,                    /*!< Group number for UART status codes. */
    kStatusGroup_LPSCI = 12,                  /*!< Group number for LPSCI status codes. */
    kStatusGroup_LPUART = 13,                 /*!< Group number for LPUART status codes. */
    kStatusGroup_SPI = 14,                    /*!< Group number for SPI status code.*/
    kStatusGroup_XRDC = 15,                   /*!< Group number for XRDC status code.*/
    kStatusGroup_SEMA42 = 16,                 /*!< Group number for SEMA42 status code.*/
    kStatusGroup_SDHC = 17,                   /*!< Group number for SDHC status code */
    kStatusGroup_SDMMC = 18,                  /*!< Group number for SDMMC status code */
    kStatusGroup_SAI = 19,                    /*!< Group number for SAI status code */
    kStatusGroup_MCG = 20,                    /*!< Group number for MCG status codes. */
    kStatusGroup_SCG = 21,                    /*!< Group number for SCG status codes. */
    kStatusGroup_SDSPI = 22,                  /*!< Group number for SDSPI status codes. */
    kStatusGroup_FLEXIO_I2S = 23,             /*!< Group number for FLEXIO I2S status codes */
    kStatusGroup_FLEXIO_MCULCD = 24,          /*!< Group number for FLEXIO LCD status codes */
    kStatusGroup_FLASHIAP = 25,               /*!< Group number for FLASHIAP status codes */
    kStatusGroup_FLEXCOMM_I2C = 26,           /*!< Group number for FLEXCOMM I2C status codes */
    kStatusGroup_I2S = 27,                    /*!< Group number for I2S status codes */
    kStatusGroup_IUART = 28,                  /*!< Group number for IUART status codes */
    kStatusGroup_CSI = 29,                    /*!< Group number for CSI status codes */
    kStatusGroup_SDRAMC = 35,                 /*!< Group number for SDRAMC status codes. */
    kStatusGroup_POWER = 39,                  /*!< Group number for POWER status codes. */
    kStatusGroup_ENET = 40,                   /*!< Group number for ENET status codes. */
    kStatusGroup_PHY = 41,                    /*!< Group number for PHY status codes. */
    kStatusGroup_TRGMUX = 42,                 /*!< Group number for TRGMUX status codes. */
    kStatusGroup_SMARTCARD = 43,              /*!< Group number for SMARTCARD status codes. */
    kStatusGroup_LMEM = 44,                   /*!< Group number for LMEM status codes. */
    kStatusGroup_QSPI = 45,                   /*!< Group number for QSPI status codes. */
    kStatusGroup_DMA = 50,                    /*!< Group number for DMA status codes. */
    kStatusGroup_EDMA = 51,                   /*!< Group number for EDMA status codes. */
    kStatusGroup_DMAMGR = 52,                 /*!< Group number for DMAMGR status codes. */
    kStatusGroup_FLEXCAN = 53,                /*!< Group number for FlexCAN status codes. */
    kStatusGroup_LTC = 54,                    /*!< Group number for LTC status codes. */
    kStatusGroup_FLEXIO_CAMERA = 55,          /*!< Group number for FLEXIO CAMERA status codes. */
    kStatusGroup_LPC_SPI = 56,                /*!< Group number for LPC_SPI status codes. */
    kStatusGroup_LPC_USART = 57,              /*!< Group number for LPC_USART status codes. */
    kStatusGroup_DMIC = 58,                   /*!< Group number for DMIC status codes. */
    kStatusGroup_SDIF = 59,                   /*!< Group number for SDIF status codes.*/
    kStatusGroup_SPIFI = 60,                  /*!< Group number for SPIFI status codes. */
    kStatusGroup_OTP = 61,                    /*!< Group number for OTP status codes. */
    kStatusGroup_MCAN = 62,                   /*!< Group number for MCAN status codes. */
    kStatusGroup_CAAM = 63,                   /*!< Group number for CAAM status codes. */
    kStatusGroup_ECSPI = 64,                  /*!< Group number for ECSPI status codes. */
    kStatusGroup_USDHC = 65,                  /*!< Group number for USDHC status codes.*/
    kStatusGroup_LPC_I2C = 66,                /*!< Group number for LPC_I2C status codes.*/
    kStatusGroup_ESAI = 69,                   /*!< Group number for ESAI status codes. */
    kStatusGroup_FLEXSPI = 70,                /*!< Group number for FLEXSPI status codes. */
    kStatusGroup_MMDC = 71,                   /*!< Group number for MMDC status codes. */
    kStatusGroup_MICFIL = 72,                 /*!< Group number for MIC status codes. */
    kStatusGroup_SDMA = 73,                   /*!< Group number for SDMA status codes. */
    kStatusGroup_NOTIFIER = 98,               /*!< Group number for NOTIFIER status codes. */
    kStatusGroup_DebugConsole = 99,           /*!< Group number for debug console status codes. */
    kStatusGroup_ApplicationRangeStart = 100, /*!< Starting number for application groups. */
};

/*! @brief Generic status return codes. */
enum _generic_status
{
    kStatus_Success = MAKE_STATUS(kStatusGroup_Generic, 0),
    kStatus_Fail = MAKE_STATUS(kStatusGroup_Generic, 1),
    kStatus_ReadOnly = MAKE_STATUS(kStatusGroup_Generic, 2),
    kStatus_OutOfRange = MAKE_STATUS(kStatusGroup_Generic, 3),
    kStatus_InvalidArgument = MAKE_STATUS(kStatusGroup_Generic, 4),
    kStatus_Timeout = MAKE_STATUS(kStatusGroup_Generic, 5),
    kStatus_NoTransferInProgress = MAKE_STATUS(kStatusGroup_Generic, 6),
};

/*! @brief Type used for all status and error return values. */
typedef int32_t status_t;


/*=========================================================================================*/
/*! @name Non-cacheable region definition macros */
/* For initialized non-zero non-cacheable variables, please using "AT_NONCACHEABLE_SECTION_INIT(var) ={xx};" or
 * "AT_NONCACHEABLE_SECTION_ALIGN_INIT(var) ={xx};" in your projects to define them, for zero-inited non-cacheable variables,
 * please using "AT_NONCACHEABLE_SECTION(var);" or "AT_NONCACHEABLE_SECTION_ALIGN(var);" to define them, these zero-inited variables
 * will be initialized to zero in system startup.
 */
/* @{ */
#if (defined(__ICCARM__))
#if defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE)
#define AT_NONCACHEABLE_SECTION(var) var @"NonCacheable"
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var @"NonCacheable"
#define AT_NONCACHEABLE_SECTION_INIT(var) var @"NonCacheable.init"
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var @"NonCacheable.init"
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) SDK_PRAGMA(data_alignment = alignbytes) var
#endif
#elif(defined(__ARMCC_VERSION))
#if defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE)
#define AT_NONCACHEABLE_SECTION(var) __attribute__((section("NonCacheable"), zero_init)) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("NonCacheable"), zero_init)) __align(alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) __attribute__((section("NonCacheable.init"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("NonCacheable.init"))) __align(alignbytes) var
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) __align(alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) __align(alignbytes) var
#endif
#elif(defined(__GNUC__))
/* For GCC, when the non-cacheable section is required, please define "__STARTUP_INITIALIZE_NONCACHEDATA"
 * in your projects to make sure the non-cacheable section variables will be initialized in system startup.
 */
#if defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE)
#define AT_NONCACHEABLE_SECTION_INIT(var) __attribute__((section("NonCacheable.init"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) \
    __attribute__((section("NonCacheable.init"))) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION(var) __attribute__((section("NonCacheable,\"aw\",%nobits @"))) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) \
    __attribute__((section("NonCacheable,\"aw\",%nobits @"))) var __attribute__((aligned(alignbytes)))
#else
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) var __attribute__((aligned(alignbytes)))
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) var __attribute__((aligned(alignbytes)))
#endif
#else
#error Toolchain not supported.
#define AT_NONCACHEABLE_SECTION(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes) var
#define AT_NONCACHEABLE_SECTION_INIT(var) var
#define AT_NONCACHEABLE_SECTION_ALIGN_INIT(var, alignbytes) var
#endif

/*! Macro to change a value to a given size aligned value */
#define SDK_SIZEALIGN(var, alignbytes) \
    ((unsigned int)((var) + ((alignbytes)-1)) & (unsigned int)(~(unsigned int)((alignbytes)-1)))
/* @} */

/*=========================================================================================*/



#ifdef	__cplusplus
}
#endif

#endif	/*__IMX_COMMON_H__*/
