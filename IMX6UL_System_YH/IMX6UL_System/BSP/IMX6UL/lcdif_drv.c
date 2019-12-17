
#include "bsp.h"
//#include "logo.h"
#include "lcdif_common.h"
#include "regselcdif.h"



volatile hw_lcdif_t *rLCDIF=REGS_LCDIF_BASE;

static const clock_ip_name_t s_elcdifApbClocks[] = LCDIF_CLOCKS;
static const clock_ip_name_t s_elcdifPixClocks[] = LCDIF_PERIPH_CLOCKS;


static void delay_us(int us)
{
	volatile int i;

	while(us-- > 0)
	{
		for(i=0;i<200;i++);
	}

}

void lcdif_csc_config(int enable)
{
    if (enable) {
        /*CSC control, from RGB to YUV */
		rLCDIF->CSC_COEFF0.U = 0x00410002;
		rLCDIF->CSC_COEFF1.U = 0x00190081;
		rLCDIF->CSC_COEFF2.U = 0x03B603DB;
		rLCDIF->CSC_COEFF3.U = 0x00700070;
		rLCDIF->CSC_COEFF4.U = 0x03EE03A2;
		rLCDIF->CSC_OFFSET.U = 0x00800010;
		rLCDIF->CSC_LIMIT.U = 0x10F010EB;
	}

    rLCDIF->CTRL.B.RGB_TO_YCBCR422_CSC=(enable);
}

/*!
 * @brief Reset the LCD controller by software mode
 */
static int lcdif_sw_reset(void)
{
#if 0
	// software reset the LCDIF controller
	rLCDIF->CTRL.B.CLKGATE=(0);
	rLCDIF->CTRL.B.SFTRST=(1);
    delay_us(100);

    rLCDIF->CTRL.B.SFTRST=(0);
    rLCDIF->CTRL.B.CLKGATE=(0);

    return 0;
#endif

    volatile uint32_t i = 0x1000;

    /* Disable the clock gate. */
    rLCDIF->CTRL_CLR = LCDIF_CTRL_CLKGATE_MASK;
     /* Confirm the clock gate is disabled. */
     while (rLCDIF->CTRL.U & LCDIF_CTRL_CLKGATE_MASK)
     {
     }

     /* Reset the block. */
     rLCDIF->CTRL_SET = LCDIF_CTRL_SFTRST_MASK;
     /* Confirm the reset bit is set. */
     while (!(rLCDIF->CTRL.U & LCDIF_CTRL_SFTRST_MASK))
     {
     }

     /* Delay for the reset. */
     while (i--)
     {
     }

     /* Bring the module out of reset. */
     rLCDIF->CTRL_CLR = LCDIF_CTRL_SFTRST_MASK;
     /* Disable the clock gate. */
     rLCDIF->CTRL_CLR = LCDIF_CTRL_CLKGATE_MASK;
     return 0;
}

/*!
 * @brief Set the LCD controller waveforms to driven the panel
 *
 * @param   sync_waveform	lcd waveform setting, including hsync/vsync/drdy and blanking parameters
 */
static int lcdif_waveform_setting(lcdif_sync_waveform_t *sync_waveform)
{
	/* frame size setting: 800 * 480 */
	rLCDIF->TRANSFER_COUNT.B.H_COUNT = sync_waveform->frameWidth;
	rLCDIF->TRANSFER_COUNT.B.V_COUNT = sync_waveform->frameHeight;

	/* timing setting */
	rLCDIF->TIMING.B.DATA_SETUP = 0x01;
	rLCDIF->TIMING.B.DATA_HOLD = 0x01;
	rLCDIF->TIMING.B.CMD_SETUP = 0x01;
	rLCDIF->TIMING.B.CMD_HOLD = 0x01;

	/* VSYNC and Dotclk Mode control setting */
	rLCDIF->VDCTRL0.B.VSYNC_PULSE_WIDTH = sync_waveform->vSyncPulseWidth;
	rLCDIF->VDCTRL0.B.VSYNC_PULSE_WIDTH_UNIT = sync_waveform->vSyncPulseUnit;
	rLCDIF->VDCTRL0.B.VSYNC_PERIOD_UNIT = sync_waveform->vSyncPeriodUnit;
	rLCDIF->VDCTRL0.B.ENABLE_POL = sync_waveform->enablePol;
	rLCDIF->VDCTRL0.B.DOTCLK_POL = sync_waveform->dotclkPol;
	rLCDIF->VDCTRL0.B.HSYNC_POL = sync_waveform->hsyncPol;
	rLCDIF->VDCTRL0.B.VSYNC_POL = sync_waveform->vSyncPol;
	rLCDIF->VDCTRL0.B.ENABLE_PRESENT = sync_waveform->enPresent;


	/* total vsync period */
	rLCDIF->VDCTRL1.B.VSYNC_PERIOD = sync_waveform->vSyncPeriod;

	/* total hsync setting */
	rLCDIF->VDCTRL2.B.HSYNC_PERIOD = sync_waveform->hSyncPeriod;
	rLCDIF->VDCTRL2.B.HSYNC_PULSE_WIDTH = sync_waveform->hSyncPulseWidth;

	rLCDIF->VDCTRL3.B.VSYNC_ONLY = 0;
	rLCDIF->VDCTRL3.B.VERTICAL_WAIT_CNT = sync_waveform->vWaitCount;
	rLCDIF->VDCTRL3.B.HORIZONTAL_WAIT_CNT = sync_waveform->hWaitCount;

	rLCDIF->VDCTRL4.B.DOTCLK_H_VALID_DATA_CNT = sync_waveform->hValidDataCount;
	rLCDIF->VDCTRL4.B.SYNC_SIGNALS_ON = 1;


#if 0
	   BF_CS1 (LCDIF_CTRL, DOTCLK_MODE, 1);
	    BF_CS1 (LCDIF_CTRL, BYPASS_COUNT, 1); //Always for DOTCLK mode
	    BF_CS1 (LCDIF_VDCTRL0, VSYNC_OEB, 0); //Vsync is always an output in the DOTCLK mode
	    BF_CS4 (LCDIF_VDCTRL0, VSYNC_POL, 0, HSYNC_POL, 0, DOTCLK_POL, 0, ENABLE_POL, 0);
	    BF_CS1 (LCDIF_VDCTRL0, ENABLE_PRESENT, 1);
	    BF_CS2 (LCDIF_VDCTRL0, VSYNC_PERIOD_UNIT, 1, VSYNC_PULSE_WIDTH_UNIT, 1);
	    BF_CS1 (LCDIF_VDCTRL0, VSYNC_PULSE_WIDTH, 2);
	    BF_CS1 (LCDIF_VDCTRL1, VSYNC_PERIOD, 280);
	    BF_CS2 (LCDIF_VDCTRL2, HSYNC_PULSE_WIDTH, 10, HSYNC_PERIOD, 360); //Assuming
	    // LCD_DATABUS_WIDTH is 24bit
	    BF_CS2 (LCDIF_VDCTRL3, VSYNC_ONLY, 0);
	    BF_CS2 (LCDIF_VDCTRL3, HORIZONTAL_WAIT_CNT, 20, VERTICAL_WAIT_CNT, 20);
	    BF_CS1 (LCDIF_VDCTRL4, DOTCLK_H_VALID_DATA_CNT, 320);//Note that DOTCLK_V_VALID_DATA_CNT is
	    //implicitly assumed to be HW_LCDIF_TRANSFER_COUNT_V_COUNT
	    BF_CS1 (LCDIF_VDCTRL4, SYNC_SIGNALS_ON, 1);
	    BF_CS1 (LCDIF_CTRL, RUN, 1);
#endif

    return 0;
}

/*!
 * @brief Initialize lcdif controller
 */
static void lcdif_init(void)
{
	  CLOCK_EnableClock(s_elcdifApbClocks[0]);
	  CLOCK_EnableClock(s_elcdifPixClocks[0]);

	//lcdif_power_on();
   // lcdif_clock_enable();
  //  lcdif_backlight_on();

    /* pin muxing for LCD */
   // lcd_iomux_config();

	/* reset controller */
    lcdif_sw_reset();

	/* setup the bus protocol
	 * 8080, signal from LCD is ignored */
    rLCDIF->CTRL1.B.MODE86=(0);
    rLCDIF->CTRL1.B.BUSY_ENABLE=(0);

	/* Take display out of reset */
    rLCDIF->CTRL1.B.RESET=(1);

	/* VSYNC is an input by default */
    rLCDIF->VDCTRL0.B.VSYNC_OEB=(1);

	/* Reset display */
    rLCDIF->CTRL1.B.RESET=(0);
	delay_us(10);
	rLCDIF->CTRL1.B.RESET=(1);
	delay_us(10);

	/* clear fifo in legency fifo: RX FIFO and TX FIFO */
	rLCDIF->CTRL1.B.FIFO_CLEAR=(1);
	delay_us(1000);
	rLCDIF->CTRL1.B.FIFO_CLEAR=(0);


}

/*!
 * @brief Initialize LCD panel.
 *
 * Make sure high - to - low transition to reset the panel
 */
static void lcdif_panel_init(void)
{
	/* low */
	rLCDIF->CTRL1.B.RESET=(0);
	delay_us(100*1000);

	/* high */
	rLCDIF->CTRL1.B.RESET=(1);
	delay_us(10*1000);

	/* low */
	rLCDIF->CTRL1.B.RESET=(0);
	delay_us(10*1000);

	/* high */
	rLCDIF->CTRL1.B.RESET=(1);
	delay_us(1*1000);
}	

#define	LCD_W	800
#define	LCD_H	1280

#define VBP    10
#define VFP    20
#define VSW    5

#define HBP    10
#define HFP    20
#define HSW    5

/*!
 * @brief Configure waveform parameters according to the timing of LCD panel
 *
 * @param   sync_waveform	lcd waveform setting, including hsync/vsync/drdy and blanking parameters
 */
static void setup_panel_params(lcdif_sync_waveform_t * syncWave)
{
    syncWave->frameWidth = LCD_W;//WVGA_FW;
    syncWave->frameHeight = LCD_H;//WVGA_FH;

    syncWave->enPresent = 1;
    //syncWave->dotclkPol = 0;
    syncWave->dotclkPol = 0;
    syncWave->hsyncPol = 0;
    syncWave->vSyncPol = 0;
    syncWave->enablePol = 0;

    syncWave->vSyncPeriodUnit = 1;
    syncWave->vSyncPulseUnit = 1;
    syncWave->vSyncPulseWidth = 10;

    syncWave->vSyncPeriod = LCD_H+VBP+VFP+VSW;//523;//523;

    syncWave->hSyncPulseWidth = 10;
    syncWave->hSyncPeriod = LCD_W+HBP+HFP+HSW;//1063;

    syncWave->hWaitCount = 20;
    syncWave->vWaitCount = 20;

    syncWave->hValidDataCount = LCD_W;//800;
}

//(*(hw_lcdif_t *) REGS_LCDIF_BASE)


void lcdif_display_setup(u32 fb_addr)
{
	u32 a;
    lcdif_sync_waveform_t syncWave = { 0 };
    char recvCh = 0;

   //?? memset((void *)DDR_LCD_BASE1, 0xFF, FRAME_WIDTH * FRAME_HEIGHT * 2);

    /* initilization for LCD */
    lcdif_init();
	/* make sure externel panel is reset by low-high transition */
    lcdif_panel_init();

#if 0
    BF_CS1 (LCDIF_CTRL, DOTCLK_MODE, 1);

    BF_CS1 (LCDIF_CTRL, BYPASS_COUNT, 1); //Always for DOTCLK mode
    BF_CS1 (LCDIF_VDCTRL0, VSYNC_OEB, 0); //Vsync is always an output in the DOTCLK mode
    BF_CS4 (LCDIF_VDCTRL0, VSYNC_POL, 0, HSYNC_POL, 0, DOTCLK_POL, 0, ENABLE_POL, 0);
    BF_CS1 (LCDIF_VDCTRL0, ENABLE_PRESENT, 1);
    BF_CS2 (LCDIF_VDCTRL0, VSYNC_PERIOD_UNIT, 1, VSYNC_PULSE_WIDTH_UNIT, 1);
    BF_CS1 (LCDIF_VDCTRL0, VSYNC_PULSE_WIDTH, 2);
    BF_CS1 (LCDIF_VDCTRL1, VSYNC_PERIOD, 280);
    BF_CS2 (LCDIF_VDCTRL2, HSYNC_PULSE_WIDTH, 10, HSYNC_PERIOD, 360); //Assuming
                                                           //  LCD_DATABUS_WIDTH is 24bit
    BF_CS1 (LCDIF_VDCTRL3, VSYNC_ONLY, 0);
    BF_CS2 (LCDIF_VDCTRL3, HORIZONTAL_WAIT_CNT, 20, VERTICAL_WAIT_CNT, 20);
    BF_CS1 (LCDIF_VDCTRL4, DOTCLK_H_VALID_DATA_CNT, 320);//Note that DOTCLK_V_VALID_DATA_CNT is
                                     //implicitly assumed to be HW_LCDIF_TRANSFER_COUNT_V_COUNT
    BF_CS1 (LCDIF_VDCTRL4, SYNC_SIGNALS_ON, 1);
    BF_CS1 (LCDIF_CTRL, RUN, 1);
    return;
#endif

    rLCDIF->CTRL.B.BYPASS_COUNT =1;

    /* LCDIF control registers setting */
	//rLCDIF->CTRL1_VSYNC_EDGE_IRQ(1);
	//rLCDIF->CTRL1_BYTE_PACKING_FORMAT(0xF);
	//rLCDIF->CTRL1_RECOVER_ON_UNDERFLOW(1);

   // rLCDIF->CTRL1.B.VSYNC_EDGE_IRQ=(1);
    //rLCDIF->CTRL1.B.RECOVER_ON_UNDERFLOW=(1);

	//rLCDIF->CTRL2.B.OUTSTANDING_REQS = 0x4;

	/* VSYNC is an output by default */
	rLCDIF->VDCTRL0.B.VSYNC_OEB = 0;


	/* frame buffer start address */
	rLCDIF->CUR_BUF.U=(fb_addr);
	rLCDIF->NEXT_BUF.U=(fb_addr);

	rLCDIF->CTRL.B.MASTER =1;
	rLCDIF->CTRL.B.DOTCLK_MODE =1;

    /*DOTCLK or VSYNC mode control */
    setup_panel_params(&syncWave);
    lcdif_waveform_setting(&syncWave);

    rLCDIF->TRANSFER_COUNT.B.H_COUNT =LCD_W;
    rLCDIF->TRANSFER_COUNT.B.V_COUNT =LCD_H;

#if 0
    rLCDIF->VDCTRL1.U =  	LCD_H +VFP + VBP + VSW;
    rLCDIF->VDCTRL2.U = 	(HSW << LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_SHIFT) |
							((LCD_W + HFP + HBP +HSW) << LCDIF_VDCTRL2_HSYNC_PERIOD_SHIFT);

    rLCDIF->VDCTRL3.U = 	((HBP + HSW) << LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_SHIFT) |
							((VBP + VSW) << LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_SHIFT);

#endif


    rLCDIF->VDCTRL1.B.VSYNC_PERIOD = LCD_H + VFP + VBP + VSW;
    rLCDIF->VDCTRL2.B.HSYNC_PERIOD = LCD_W + HFP + HBP + HSW;
    rLCDIF->VDCTRL2.B.HSYNC_PULSE_WIDTH = HSW;

    rLCDIF->VDCTRL3.B.HORIZONTAL_WAIT_CNT =10;
    rLCDIF->VDCTRL3.B.VERTICAL_WAIT_CNT =20;
	rLCDIF->VDCTRL4.B.DOTCLK_H_VALID_DATA_CNT = LCD_W;

    rLCDIF->VDCTRL3.B.MUX_SYNC_SIGNALS =0;
    rLCDIF->VDCTRL3.B.VSYNC_ONLY =0;

//	rLCDIF->VDCTRL4.B.DOTCLK_DLY_SEL =0;




//    printf("Please select the LCD panel type: \n");
//    printf("\t0 - SEIKO 70WVW1TZ3 WVGA Panel (MX28LCD 24-bit)\n");
 //   printf("\t1 - CHUNHWA CLAA 070VC01 WVGA Panel (MX51LCD 16-bit)\n");

#if 0
    do {
        recvCh = getchar();
    } while (recvCh == NONE_CHAR);
#endif;

    recvCh = '1';

    if (recvCh == '0')
    {

    	rLCDIF->CTRL1.B.BYTE_PACKING_FORMAT =0x7;
    	rLCDIF->CTRL.B.DATA_FORMAT_24_BIT =0;

        /*RGB888(24bit) in dotclk mode transfer */

		/* 24 bit data bus */
		rLCDIF->CTRL.B.LCD_DATABUS_WIDTH =3; //3
		rLCDIF->CTRL.B.WORD_LENGTH =3;       //2


		rLCDIF->CTRL1.B.FIFO_CLEAR =1;
		rLCDIF->VDCTRL4.B.SYNC_SIGNALS_ON = 1;

		rLCDIF->CTRL.B.RUN=(1);
    }
    else if (recvCh == '1')
    {
    	rLCDIF->CTRL1.B.BYTE_PACKING_FORMAT =0xF;
        /*RGB565(16bit) in dotclk mode transfer */

		/* 16 bit data bus */
		rLCDIF->CTRL.B.LCD_DATABUS_WIDTH =0x0;
		rLCDIF->CTRL.B.WORD_LENGTH =0;

		rLCDIF->CTRL1.B.FIFO_CLEAR =1;
		rLCDIF->VDCTRL4.B.SYNC_SIGNALS_ON = 1;
		rLCDIF->CTRL.B.RUN=(1);
    }
    else
    {
        printf("Wrong panel type!\n");
    }
}

#if 0
void image_center_copy(void)
{
    int width = 313, height = 117;
    int left_offset = (FRAME_WIDTH - width) / 2;
    int top_offset = (FRAME_HEIGHT - height) / 2;
    int i = 0;

    memset((void *)DDR_LCD_BASE1, 0xFF, FRAME_WIDTH * FRAME_HEIGHT * 2);

    for (i = 0; i < height; i++) {
    //    memcpy((void *)(DDR_LCD_BASE1 + FRAME_WIDTH * (top_offset + i) * 2 + left_offset * 2),
     //          &gImage_fsl[width * i * 2], width * 2);
    }
}
#endif
