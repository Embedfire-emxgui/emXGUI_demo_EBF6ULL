#ifndef		__IOMUX_H__
#define		__IOMUX_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "fsl_common.h"

/*=========================================================================================*/
#define	IOMUXC_REG_PA_BASE	0x020E0000
#define	IOMUXC_REG_VA_BASE	SFR_PA2VA(IOMUXC_REG_PA_BASE)

#define	IOMUX_REG_BASE		IOMUXC_REG_VA_BASE

typedef union
{
	u32 U;
	struct
	{
		u32 MUX:4;		// [3-0]: ALT5 mode.
		u32 SION:1;		// [4]: Software Input On Field.
		u32 _rsv2:27;

	 } B;
} IOMUXC_MUX_t;

typedef union
{
	u32 U;
	struct
	{
		u32 SRE:1;		// [0]: Slew Rate Field. 0:Slow Slew Rate; 1:Fast Slew Rate.
		u32 _rsv0:2;
		u32 DSE:3;		// [5-3]: Drive Strength Field.
		u32 SPEED:2;	// [7-6]: Speed Field. 0:50MHZ; 1:100MHZ; 2:100MHZ; 3:200MHZ.
		u32 _rsv1:3;
		u32 ODE:1;		// [11]: Open Drain Enable Field.
		u32 PKE:1;		// [12]: Pull / Keep Enable Field.
		u32 PUE:1;		// [13]: Pull / Keep Select Field.
		u32 PUS:1;		// [15-14]: Pull Up / Down Config. Field.
		u32 HYS:1;		// [16]: Hyst. Enable Field.
		u32 _rsv2:15;

	 } B;
} IOMUXC_CFG_t;

#define	rIOMUXC_MUX(offset)				(*(volatile IOMUXC_MUX_t*)(IOMUX_REG_BASE + offset))
#define	rIOMUXC_CFG(offset)				(*(volatile IOMUXC_CFG_t*)(IOMUX_REG_BASE + offset))
//#define	rIOMUXC_MUX_CFG(name,mux,cfg)	(*(volatile u32*)(IOMUX_REG_BASE + MUX_##name))=mux; (*(volatile u32*)(IOMUX_REG_BASE + CFG_##name))=cfg;

static __INLINE void IOMUXC_Config(unsigned int mux_offset,unsigned int cfg_offset,unsigned int mux,unsigned int cfg)
{
	(*(volatile IOMUXC_MUX_t*)(IOMUX_REG_BASE + mux_offset)).B.MUX =mux;
	(*(volatile IOMUXC_CFG_t*)(IOMUX_REG_BASE + cfg_offset)).U =cfg;
}

/*=========================================================================================*/

#define	MUX_BOOT_MODE0		(0x014)
#define	CFG_BOOT_MODE0		(0x2A0)

#define	MUX_BOOT_MODE1		(0x018)
#define	CFG_BOOT_MODE1		(0x2A4)

////////IOMUXC_SNVS
#define	IOMUXC_MUX_SNVS_PA_BASE	0x020E001C
#define	IOMUXC_MUX_SNVS_VA_BASE	SFR_PA2VA(IOMUXC_MUX_SNVS_PA_BASE)
#define	IOMUXC_MUX_SNVS			((IOMUXC_MUX_SNVS_t*)IOMUXC_MUX_SNVS_VA_BASE)
typedef	struct
{
	volatile IOMUXC_MUX_t TAMPER0;
	volatile IOMUXC_MUX_t TAMPER1;
	volatile IOMUXC_MUX_t TAMPER2;
	volatile IOMUXC_MUX_t TAMPER3;
	volatile IOMUXC_MUX_t TAMPER4;
	volatile IOMUXC_MUX_t TAMPER5;
	volatile IOMUXC_MUX_t TAMPER6;
	volatile IOMUXC_MUX_t TAMPER7;
	volatile IOMUXC_MUX_t TAMPER8;
	volatile IOMUXC_MUX_t TAMPER9;

}IOMUXC_MUX_SNVS_t;


#define	IOMUXC_CFG_SNVS_PA_BASE	0x020E02A8
#define	IOMUXC_CFG_SNVS_VA_BASE	SFR_PA2VA(IOMUXC_CFG_SNVS_PA_BASE)
#define	IOMUXC_CFG_SNVS			((IOMUXC_CFG_SNVS_t*)IOMUXC_CFG_SNVS_VA_BASE)
typedef	struct
{
	volatile IOMUXC_CFG_t TAMPER0;
	volatile IOMUXC_CFG_t TAMPER1;
	volatile IOMUXC_CFG_t TAMPER2;
	volatile IOMUXC_CFG_t TAMPER3;
	volatile IOMUXC_CFG_t TAMPER4;
	volatile IOMUXC_CFG_t TAMPER5;
	volatile IOMUXC_CFG_t TAMPER6;
	volatile IOMUXC_CFG_t TAMPER7;
	volatile IOMUXC_CFG_t TAMPER8;
	volatile IOMUXC_CFG_t TAMPER9;

}IOMUXC_CFG_SNVS_t;
/*
#define	MUX_SNVS_TAMPER0	(0x01C)
#define	CFG_SNVS_TAMPER0	(0x2A8)

#define	MUX_SNVS_TAMPER1	(0x020)
#define	CFG_SNVS_TAMPER1	(0x2AC)

#define	MUX_SNVS_TAMPER2	(0x024)
#define	CFG_SNVS_TAMPER2	(0x2B0)

#define	MUX_SNVS_TAMPER3	(0x028)
#define	CFG_SNVS_TAMPER3	(0x2B4)

#define	MUX_SNVS_TAMPER4	(0x02C)
#define	CFG_SNVS_TAMPER4	(0x2B8)

#define	MUX_SNVS_TAMPER5	(0x030)
#define	CFG_SNVS_TAMPER5	(0x2BC)

#define	MUX_SNVS_TAMPER6	(0x034)
#define	CFG_SNVS_TAMPER6	(0x2C0)

#define	MUX_SNVS_TAMPER7	(0x038)
#define	CFG_SNVS_TAMPER7	(0x2C4)

#define	MUX_SNVS_TAMPER8	(0x03C)
#define	CFG_SNVS_TAMPER8	(0x2C8)

#define	MUX_SNVS_TAMPER9	(0x040)
#define	CFG_SNVS_TAMPER9	(0x2CC)
*/

////////IOMUXC_GPIO1
#define	IOMUXC_MUX_GPIO1_PA_BASE	0x020E005C
#define	IOMUXC_MUX_GPIO1_VA_BASE	SFR_PA2VA(IOMUXC_MUX_GPIO1_PA_BASE)
#define	IOMUXC_MUX_GPIO1			((IOMUXC_MUX_GPIO1_t*)IOMUXC_MUX_GPIO1_VA_BASE)
typedef	struct
{
	volatile IOMUXC_MUX_t IO00;
	volatile IOMUXC_MUX_t IO01;
	volatile IOMUXC_MUX_t IO02;
	volatile IOMUXC_MUX_t IO03;
	volatile IOMUXC_MUX_t IO04;
	volatile IOMUXC_MUX_t IO05;
	volatile IOMUXC_MUX_t IO06;
	volatile IOMUXC_MUX_t IO07;
	volatile IOMUXC_MUX_t IO08;
	volatile IOMUXC_MUX_t IO09;

}IOMUXC_MUX_GPIO1_t;


#define	IOMUXC_CFG_GPIO1_PA_BASE	0x020E02E8
#define	IOMUXC_CFG_GPIO1_VA_BASE	SFR_PA2VA(IOMUXC_CFG_GPIO1_PA_BASE)
#define	IOMUXC_CFG_GPIO1			((IOMUXC_CFG_GPIO1_t*)IOMUXC_CFG_GPIO1_VA_BASE)
typedef	struct
{
	volatile IOMUXC_CFG_t IO00;
	volatile IOMUXC_CFG_t IO01;
	volatile IOMUXC_CFG_t IO02;
	volatile IOMUXC_CFG_t IO03;
	volatile IOMUXC_CFG_t IO04;
	volatile IOMUXC_CFG_t IO05;
	volatile IOMUXC_CFG_t IO06;
	volatile IOMUXC_CFG_t IO07;
	volatile IOMUXC_CFG_t IO08;
	volatile IOMUXC_CFG_t IO09;

}IOMUXC_CFG_GPIO1_t;

/*
#define	MUX_GPIO1_IO00	(0x05C)
#define	CFG_GPIO1_IO00	(0x2E8)

#define	MUX_GPIO1_IO01	(0x060)
#define	CFG_GPIO1_IO01	(0x2EC)

#define	MUX_GPIO1_IO02	(0x064)
#define	CFG_GPIO1_IO02	(0x2F0)

#define	MUX_GPIO1_IO03	(0x068)
#define	CFG_GPIO1_IO03	(0x2F4)

#define	MUX_GPIO1_IO04	(0x06C)
#define	CFG_GPIO1_IO04	(0x2F8)

#define	MUX_GPIO1_IO05	(0x070)
#define	CFG_GPIO1_IO05	(0x2FC)

#define	MUX_GPIO1_IO06	(0x074)
#define	CFG_GPIO1_IO06	(0x300)

#define	MUX_GPIO1_IO07	(0x078)
#define	CFG_GPIO1_IO07	(0x304)

#define	MUX_GPIO1_IO08	(0x07C)
#define	CFG_GPIO1_IO08	(0x308)
#define	IOMUXC_GPIO1_IO08_PWM1OUT	0x07C,0x308,0
#define	IOMUXC_GPIO1_IO08_GPIO1_8	0x07C,0x308,5

#define	MUX_GPIO1_IO09	(0x080)
#define	CFG_GPIO1_IO09	(0x30C)*/

////////IOMUXC_LCD
#define	IOMUXC_MUX_LCD_PA_BASE	0x020E0104
#define	IOMUXC_MUX_LCD_VA_BASE	SFR_PA2VA(IOMUXC_MUX_LCD_PA_BASE)
#define	IOMUXC_MUX_LCD			((IOMUXC_MUX_LCD_t*)IOMUXC_MUX_LCD_VA_BASE)
typedef	struct
{
	volatile IOMUXC_MUX_t CLK;
	volatile IOMUXC_MUX_t ENABLE;
	volatile IOMUXC_MUX_t HSYNC;
	volatile IOMUXC_MUX_t VSYNC;
	volatile IOMUXC_MUX_t RESET;
	volatile IOMUXC_MUX_t DATA00;
	volatile IOMUXC_MUX_t DATA01;
	volatile IOMUXC_MUX_t DATA02;
	volatile IOMUXC_MUX_t DATA03;
	volatile IOMUXC_MUX_t DATA04;
	volatile IOMUXC_MUX_t DATA05;
	volatile IOMUXC_MUX_t DATA06;
	volatile IOMUXC_MUX_t DATA07;
	volatile IOMUXC_MUX_t DATA08;
	volatile IOMUXC_MUX_t DATA09;
	volatile IOMUXC_MUX_t DATA10;
	volatile IOMUXC_MUX_t DATA11;
	volatile IOMUXC_MUX_t DATA12;
	volatile IOMUXC_MUX_t DATA13;
	volatile IOMUXC_MUX_t DATA14;
	volatile IOMUXC_MUX_t DATA15;
	volatile IOMUXC_MUX_t DATA16;
	volatile IOMUXC_MUX_t DATA17;
	volatile IOMUXC_MUX_t DATA18;
	volatile IOMUXC_MUX_t DATA19;
	volatile IOMUXC_MUX_t DATA20;
	volatile IOMUXC_MUX_t DATA21;
	volatile IOMUXC_MUX_t DATA22;
	volatile IOMUXC_MUX_t DATA23;

}IOMUXC_MUX_LCD_t;


#define	IOMUXC_CFG_LCD_PA_BASE	0x020E0390
#define	IOMUXC_CFG_LCD_VA_BASE	SFR_PA2VA(IOMUXC_CFG_LCD_PA_BASE)
#define	IOMUXC_CFG_LCD			((IOMUXC_CFG_LCD_t*)IOMUXC_CFG_LCD_VA_BASE)
typedef	struct
{
	volatile IOMUXC_CFG_t CLK;
	volatile IOMUXC_CFG_t ENABLE;
	volatile IOMUXC_CFG_t HSYNC;
	volatile IOMUXC_CFG_t VSYNC;
	volatile IOMUXC_CFG_t RESET;
	volatile IOMUXC_CFG_t DATA00;
	volatile IOMUXC_CFG_t DATA01;
	volatile IOMUXC_CFG_t DATA02;
	volatile IOMUXC_CFG_t DATA03;
	volatile IOMUXC_CFG_t DATA04;
	volatile IOMUXC_CFG_t DATA05;
	volatile IOMUXC_CFG_t DATA06;
	volatile IOMUXC_CFG_t DATA07;
	volatile IOMUXC_CFG_t DATA08;
	volatile IOMUXC_CFG_t DATA09;
	volatile IOMUXC_CFG_t DATA10;
	volatile IOMUXC_CFG_t DATA11;
	volatile IOMUXC_CFG_t DATA12;
	volatile IOMUXC_CFG_t DATA13;
	volatile IOMUXC_CFG_t DATA14;
	volatile IOMUXC_CFG_t DATA15;
	volatile IOMUXC_CFG_t DATA16;
	volatile IOMUXC_CFG_t DATA17;
	volatile IOMUXC_CFG_t DATA18;
	volatile IOMUXC_CFG_t DATA19;
	volatile IOMUXC_CFG_t DATA20;
	volatile IOMUXC_CFG_t DATA21;
	volatile IOMUXC_CFG_t DATA22;
	volatile IOMUXC_CFG_t DATA23;

}IOMUXC_CFG_LCD_t;
/*
#define	MUX_LCD_CLK		(0x104)
#define	CFG_LCD_CLK		(0x390)

#define	MUX_LCD_ENABLE	(0x108)
#define	CFG_LCD_ENABLE	(0x394)

#define	MUX_LCD_HSYNC	(0x10C)
#define	CFG_LCD_HSYNC	(0x398)

#define	MUX_LCD_VSYNC	(0x110)
#define	CFG_LCD_VSYNC	(0x39C)

#define	MUX_LCD_RESET	(0x114)
#define	CFG_LCD_RESET	(0x3A0)

#define	MUX_LCD_DATA00	(0x118)
#define	CFG_LCD_DATA00	(0x3A4)
#define	IOMUXC_LCD_DATA00_PWM1OUT	0x118,0x3A4,1
#define	IOMUXC_LCD_DATA00_GPIO3_5	0x118,0x3A4,5


#define	MUX_LCD_DATA01	(0x11C)
#define	CFG_LCD_DATA01	(0x3A8)
#define	IOMUXC_LCD_DATA01_PWM2OUT	0x11C,0x3A8,1
#define	IOMUXC_LCD_DATA01_GPIO3_6	0x11C,0x3A8,5


#define	MUX_LCD_DATA02	(0x120)
#define	CFG_LCD_DATA02	(0x3AC)
#define	IOMUXC_LCD_DATA02_PWM2OUT	0x120,0x3AC,1
#define	IOMUXC_LCD_DATA02_GPIO3_7	0x120,0x3AC,5

#define	MUX_LCD_DATA03	(0x124)
#define	CFG_LCD_DATA03	(0x3B0)

#define	MUX_LCD_DATA04	(0x128)
#define	CFG_LCD_DATA04	(0x3B4)

#define	MUX_LCD_DATA05	(0x12C)
#define	CFG_LCD_DATA05	(0x3B8)

#define	MUX_LCD_DATA06	(0x130)
#define	CFG_LCD_DATA06	(0x3BC)

#define	MUX_LCD_DATA07	(0x134)
#define	CFG_LCD_DATA07	(0x3C0)

#define	MUX_LCD_DATA08	(0x138)
#define	CFG_LCD_DATA08	(0x3C4)

#define	MUX_LCD_DATA09	(0x13C)
#define	CFG_LCD_DATA09	(0x3C8)

#define	MUX_LCD_DATA10	(0x140)
#define	CFG_LCD_DATA10	(0x3CC)

#define	MUX_LCD_DATA11	(0x144)
#define	CFG_LCD_DATA11	(0x3D0)

#define	MUX_LCD_DATA12	(0x148)
#define	CFG_LCD_DATA12	(0x3D4)

#define	MUX_LCD_DATA13	(0x14C)
#define	CFG_LCD_DATA13	(0x3D8)

#define	MUX_LCD_DATA14	(0x150)
#define	CFG_LCD_DATA14	(0x3DC)

#define	MUX_LCD_DATA15	(0x154)
#define	CFG_LCD_DATA15	(0x3E0)

#define	MUX_LCD_DATA16	(0x158)
#define	CFG_LCD_DATA16	(0x3E4)

#define	MUX_LCD_DATA17	(0x15C)
#define	CFG_LCD_DATA17	(0x3E8)

#define	MUX_LCD_DATA18	(0x160)
#define	CFG_LCD_DATA18	(0x3EC)

#define	MUX_LCD_DATA19	(0x164)
#define	CFG_LCD_DATA19	(0x3F0)

#define	MUX_LCD_DATA20	(0x168)
#define	CFG_LCD_DATA20	(0x3F4)

#define	MUX_LCD_DATA21	(0x16C)
#define	CFG_LCD_DATA21	(0x3F8)

#define	MUX_LCD_DATA22	(0x170)
#define	CFG_LCD_DATA22	(0x3FC)

#define	MUX_LCD_DATA23	(0x174)
#define	CFG_LCD_DATA23	(0x400)
*/
////////IOMUXC_UART
#define	IOMUXC_MUX_UART_PA_BASE	0x020E0084
#define	IOMUXC_MUX_UART_VA_BASE	SFR_PA2VA(IOMUXC_MUX_UART_PA_BASE)
#define	IOMUXC_MUX_UART			((IOMUXC_MUX_UART_t*)IOMUXC_MUX_UART_VA_BASE)

typedef	struct
{
	volatile IOMUXC_MUX_t TXD1;
	volatile IOMUXC_MUX_t RXD1;
	volatile IOMUXC_MUX_t CTS1;
	volatile IOMUXC_MUX_t RTS1;

	volatile IOMUXC_MUX_t TXD2;
	volatile IOMUXC_MUX_t RXD2;
	volatile IOMUXC_MUX_t CTS2;
	volatile IOMUXC_MUX_t RTS2;

	volatile IOMUXC_MUX_t TXD3;
	volatile IOMUXC_MUX_t RXD3;
	volatile IOMUXC_MUX_t CTS3;
	volatile IOMUXC_MUX_t RTS3;

	volatile IOMUXC_MUX_t TXD4;
	volatile IOMUXC_MUX_t RXD4;

	volatile IOMUXC_MUX_t TXD5;
	volatile IOMUXC_MUX_t RXD5;

}IOMUXC_MUX_UART_t;


#define	IOMUXC_CFG_UART_PA_BASE	0x020E0310
#define	IOMUXC_CFG_UART_VA_BASE	SFR_PA2VA(IOMUXC_CFG_UART_PA_BASE)
#define	IOMUXC_CFG_UART			((IOMUXC_UART_CFG_t*)IOMUXC_CFG_UART_VA_BASE)
typedef	struct
{
	volatile IOMUXC_CFG_t TXD1;
	volatile IOMUXC_CFG_t RXD1;
	volatile IOMUXC_CFG_t CTS1;
	volatile IOMUXC_CFG_t RTS1;

	volatile IOMUXC_CFG_t TXD2;
	volatile IOMUXC_CFG_t RXD2;
	volatile IOMUXC_CFG_t CTS2;
	volatile IOMUXC_CFG_t RTS2;

	volatile IOMUXC_CFG_t TXD3;
	volatile IOMUXC_CFG_t RXD3;
	volatile IOMUXC_CFG_t CTS3;
	volatile IOMUXC_CFG_t RTS3;

	volatile IOMUXC_CFG_t TXD4;
	volatile IOMUXC_CFG_t RXD4;

	volatile IOMUXC_CFG_t TXD5;
	volatile IOMUXC_CFG_t RXD5;

}IOMUXC_UART_CFG_t;
/*
#define	MUX_UART1_TX	(0x084)
#define	CFG_UART1_TX	(0x310)

#define	MUX_UART1_RX	(0x088)
#define	CFG_UART1_RX	(0x314)

#define	MUX_UART1_CTS	(0x08C)
#define	CFG_UART1_CTS	(0x318)

#define	MUX_UART1_RTS	(0x090)
#define	CFG_UART1_RTS	(0x31C)

#define	MUX_UART2_TX	(0x094)
#define	CFG_UART2_TX	(0x320)

#define	MUX_UART2_RX	(0x098)
#define	CFG_UART2_RX	(0x324)

#define	MUX_UART2_CTS	(0x09C)
#define	CFG_UART2_CTS	(0x328)

#define	MUX_UART2_RTS	(0x0A0)
#define	CFG_UART2_RTS	(0x32C)

#define	MUX_UART3_TX	(0x0A4)
#define	CFG_UART3_TX	(0x330)

#define	MUX_UART3_RX	(0x0A8)
#define	CFG_UART3_RX	(0x334)

#define	MUX_UART3_CTS	(0x0AC)
#define	CFG_UART3_CTS	(0x338)

#define	MUX_UART3_RTS	(0x0B0)
#define	CFG_UART3_RTS	(0x33C)

#define	MUX_UART4_TX	(0x0B4)
#define	CFG_UART4_TX	(0x340)

#define	MUX_UART4_RX	(0x0B8)
#define	CFG_UART4_RX	(0x344)

#define	MUX_UART5_TX	(0x0BC)
#define	CFG_UART5_TX	(0x348)

#define	MUX_UART5_RX	(0x0C0)
#define	CFG_UART5_RX	(0x34C)
*/

#define	MUX_NAND_RE		(0x178)
#define	CFG_NAND_RE		(0x404)

#define	MUX_NAND_WE		(0x17C)
#define	CFG_NAND_WE		(0x408)

#define	MUX_NAND_D0		(0x180)
#define	CFG_NAND_D0		(0x40C)

#define	MUX_NAND_D1		(0x184)
#define	CFG_NAND_D1		(0x410)

#define	MUX_NAND_D2		(0x188)
#define	CFG_NAND_D2		(0x414)

#define	MUX_NAND_D3		(0x18C)
#define	CFG_NAND_D3		(0x418)

#define	MUX_NAND_D4		(0x190)
#define	CFG_NAND_D4		(0x41C)

#define	MUX_NAND_D5		(0x194)
#define	CFG_NAND_D5		(0x420)

#define	MUX_NAND_D6		(0x198)
#define	CFG_NAND_D6		(0x424)

#define	MUX_NAND_D7		(0x19C)
#define	CFG_NAND_D7		(0x428)

#define	MUX_NAND_ALE	(0x1A0)
#define	CFG_NAND_ALE	(0x42C)

#define	MUX_NAND_WP		(0x1A4)
#define	CFG_NAND_WP		(0x430)

#define	MUX_NAND_RDY	(0x1A8)
#define	CFG_NAND_RDY	(0x434)

#define	MUX_NAND_CE0	(0x1AC)
#define	CFG_NAND_CE0	(0x438)

#define	MUX_NAND_CE1	(0x1B0)
#define	CFG_NAND_CE1	(0x43C)

#define	MUX_NAND_CLE	(0x1B4)
#define	CFG_NAND_CLE	(0x440)

#define	MUX_NAND_DQS	(0x1B8)
#define	CFG_NAND_DQS	(0x444)

////////IOMUXC_SD1
#define	MUX_SD1_CMD		(0x1BC)
#define	CFG_SD1_CMD		(0x448)

#define	MUX_SD1_CLK		(0x1C0)
#define	CFG_SD1_CLK		(0x44C)

#define	MUX_SD1_DAT0	(0x1C4)
#define	CFG_SD1_DAT0	(0x450)

#define	MUX_SD1_DAT1	(0x1C8)
#define	CFG_SD1_DAT1	(0x454)

#define	MUX_SD1_DAT2	(0x1CC)
#define	CFG_SD1_DAT2	(0x458)

#define	MUX_SD1_DAT3	(0x1D0)
#define	CFG_SD1_DAT3	(0x45C)

////////IOMUXC_CSI
#define	IOMUXC_MUX_CSI_PA_BASE	0x020E01D4
#define	IOMUXC_MUX_CSI_VA_BASE	SFR_PA2VA(IOMUXC_MUX_CSI_PA_BASE)
#define	IOMUXC_MUX_CSI			((IOMUXC_MUX_CSI_t*)IOMUXC_MUX_CSI_VA_BASE)
typedef	struct
{
	volatile IOMUXC_MUX_t MCLK;
	volatile IOMUXC_MUX_t PCLK;
	volatile IOMUXC_MUX_t VSYNC;
	volatile IOMUXC_MUX_t HSYNC;
	volatile IOMUXC_MUX_t DATA00;
	volatile IOMUXC_MUX_t DATA01;
	volatile IOMUXC_MUX_t DATA02;
	volatile IOMUXC_MUX_t DATA03;
	volatile IOMUXC_MUX_t DATA04;
	volatile IOMUXC_MUX_t DATA05;
	volatile IOMUXC_MUX_t DATA06;
	volatile IOMUXC_MUX_t DATA07;

}IOMUXC_MUX_CSI_t;


#define	IOMUXC_CFG_CSI_PA_BASE	0x020E0460
#define	IOMUXC_CFG_CSI_VA_BASE	SFR_PA2VA(IOMUXC_CFG_CSI_PA_BASE)
#define	IOMUXC_CFG_CSI			((IOMUXC_CFG_CSI_t*)IOMUXC_CFG_CSI_VA_BASE)
typedef	struct
{
	volatile IOMUXC_CFG_t MCLK;
	volatile IOMUXC_CFG_t PCLK;
	volatile IOMUXC_CFG_t VSYNC;
	volatile IOMUXC_CFG_t HSYNC;
	volatile IOMUXC_CFG_t DATA00;
	volatile IOMUXC_CFG_t DATA01;
	volatile IOMUXC_CFG_t DATA02;
	volatile IOMUXC_CFG_t DATA03;
	volatile IOMUXC_CFG_t DATA04;
	volatile IOMUXC_CFG_t DATA05;
	volatile IOMUXC_CFG_t DATA06;
	volatile IOMUXC_CFG_t DATA07;

}IOMUXC_CFG_CSI_t;


#define	MUX_CSI_MCLK	(0x1D4)
#define	MUX_CSI_PCLK	(0x1D8)
#define	MUX_CSI_VSYNC	(0x1DC)
#define	MUX_CSI_HSYNC	(0x1E0)
#define	MUX_CSI_DATA00	(0x1E4)
#define	MUX_CSI_DATA01	(0x1E8)
#define	MUX_CSI_DATA02	(0x1EC)
#define	MUX_CSI_DATA03	(0x1F0)
#define	MUX_CSI_DATA04	(0x1F4)
#define	MUX_CSI_DATA05	(0x1F8)
#define	MUX_CSI_DATA06	(0x1FC)
#define	MUX_CSI_DATA07	(0x200)
////

#ifdef	__cplusplus
}
#endif

#endif	/*__IOMUX_H__*/
