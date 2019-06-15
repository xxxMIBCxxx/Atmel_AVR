#ifndef __SENSOR_CONFIG_H__
#define __SENSOR_CONFIG_H__


// 画像サイズ
#define VGA_WIDTH					( 640 )
#define VGA_HEIGHT					( 480 )
#define QVGA_WIDTH					( 320 )
#define QVGA_HEIGHT					( 240 )
#define CIF_WIDTH					( 352 )
#define CIF_HEIGHT					( 288 )
#define QCIF_WIDTH					( 176 )
#define QCIF_HEIGHT					( 144 )


#define QCIF_SCREEN_SIZE			( QCIF_WIDTH * QCIF_HEIGHT)

// OV7670デバイスID
#define OV7670_I2C_ADDR				( 0x42 )


// OV7670レジスタ
#define REG_GAIN					( 0x00 )		// Gain lower 8 bits (rest in vref)
#define REG_BLUE					( 0x01 )		// blue gain
#define REG_RED						( 0x02 )		// red gain
#define REG_VREF					( 0x03 )		// Pieces of GAIN, VSTART, VSTOP
#define REG_COM1					( 0x04 )		// Control 1
#define   COM1_CCIR656					( 0x40 )	//   CCIR656 enable
#define REG_BAVE					( 0x05 )		// U/B Average level
#define REG_GbAVE					( 0x06 )		// Y/Gb Average level
#define REG_AECHH					( 0x07 )		// AEC MS 5 bits
#define REG_RAVE					( 0x08 )		// V/R Average level
#define REG_COM2					( 0x09 )		// Control 2
#define   COM2_SSLEEP					( 0x10 )	//   Soft sleep mode
#define REG_PID						( 0x0a )		// Product ID MSB
#define REG_VER						( 0x0b )		// Product ID LSB
#define REG_COM3					( 0x0c )		// Control 3
#define   COM3_SWAP						( 0x40 )	//   Byte swap
#define   COM3_SCALEEN					( 0x08 )	//   Enable scaling
#define   COM3_DCWEN					( 0x04 )	//   Enable downsamp/crop/window
#define REG_COM4					( 0x0d )		// Control 4
#define REG_COM5					( 0x0e )		// All "reserved"
#define REG_COM6					( 0x0f )		// Control 6 
#define REG_AECH					( 0x10 )		// More bits of AEC value
#define REG_CLKRC					( 0x11 )		// Clocl control
#define   CLK_EXT						( 0x40 )	//   Use external clock directly
#define   CLK_SCALE						( 0x3f )	//   Mask for internal clock scale
#define REG_COM7					( 0x12 )		// Control 7
#define   COM7_RESET					( 0x80 )	//   Register reset
#define   COM7_FMT_MASK					( 0x38 )	//
#define   COM7_FMT_VGA					( 0x00 )	//   VGA format
#define   COM7_FMT_CIF					( 0x20 )	//   CIF format
#define   COM7_FMT_QVGA					( 0x10 )	//   QVGA format
#define   COM7_FMT_QCIF					( 0x08 )	//   QCIF format
#define   COM7_RGB						( 0x04 )	//   bits 0 and 2 - RGB format
#define   COM7_YUV						( 0x00 )	//   YUV
#define   COM7_BAYER					( 0x01 )	//   Bayer format
#define   COM7_PBAYER					( 0x05 )	//   "Processed bayer"
#define   COM7_COLOR_BAR				( 0x02 )	//   Enable Color Bar
#define REG_COM8					( 0x13 )		// Control 8
#define   COM8_FASTAEC					( 0x80 )	//   Enable fast AGC/AEC
#define   COM8_AECSTEP					( 0x40 )	//   Unlimited AEC step size
#define   COM8_BFILT					( 0x20 )	//   Band filter enable
#define   COM8_AGC						( 0x04 )	//   Auto gain enable
#define   COM8_AWB						( 0x02 )	//   White balance enable
#define   COM8_AEC						( 0x01 )	//   Auto exposure enable
#define REG_COM9					( 0x14 )		// Control 9  - gain ceiling
#define	  COM9_AGC_2X					( 0x00 )
#define	  COM9_AGC_4X					( 0x10 )
#define	  COM9_AGC_8X					( 0x20 )
#define	  COM9_AGC_16X					( 0x30 )
#define	  COM9_AGC_32X					( 0x40 )
#define	  COM9_AGC_64X					( 0x50 )
#define	  COM9_AGC_128X					( 0x60 )
#define   COM9_AGC_MASK					( 0x70 )
#define	  COM9_FREEZE					( 0x01 )
#define REG_COM10					( 0x15 )		// Control 10
#define   COM10_HSYNC					( 0x40 )	//   HSYNC instead of HREF
#define   COM10_PCLK_HB					( 0x20 )	//   Suppress PCLK on horiz blank
#define   COM10_HREF_REV				( 0x08 )	//   Reverse HREF
#define   COM10_VS_LEAD					( 0x04 )	//   VSYNC on clock leading edge
#define   COM10_VS_NEG					( 0x02 )	//   VSYNC negative
#define   COM10_HS_NEG					( 0x01 )	//   HSYNC negative
#define REG_HSTART					( 0x17 )		// Horiz start high bits
#define REG_HSTOP					( 0x18 )		// Horiz stop high bits
#define REG_VSTART					( 0x19 )		// Vert start high bits
#define REG_VSTOP					( 0x1a )		// Vert stop high bits
#define REG_PSHFT					( 0x1b )		// Pixel delay after HREF
#define REG_MIDH					( 0x1c )		// Manuf. ID high
#define REG_MIDL					( 0x1d )		// Manuf. ID low
#define REG_MVFP					( 0x1e )		// Mirror / vflip
#define   MVFP_MIRROR					( 0x20 )	//   Mirror image
#define   MVFP_FLIP						( 0x10 )	//   Vertical flip

#define REG_AEW						( 0x24 )		// AGC upper limit
#define REG_AEB						( 0x25 )		// AGC lower limit
#define REG_VPT						( 0x26 )		// AGC/AEC fast mode op region
#define REG_HSYST					( 0x30 )		// HSYNC rising edge delay
#define REG_HSYEN					( 0x31 )		// HSYNC falling edge delay
#define REG_HREF					( 0x32 )		// HREF pieces
#define REG_TSLB					( 0x3a )		// lots of stuff
#define   TSLB_YLAST					( 0x04 )	//   UYVY or VYUY - see com13
#define REG_COM11					( 0x3b )		// Control 11
#define   COM11_NIGHT					( 0x80 )    //   NIght mode enable
#define   COM11_NMFR					( 0x60 )	//   Two bit NM frame rate
#define   COM11_HZAUTO					( 0x10 )	//   Auto detect 50/60 Hz
#define   COM11_50HZ					( 0x08 )	//   Manual 50Hz select
#define   COM11_EXP						( 0x02 )	//
#define REG_COM12					( 0x3c )		// Control 12
#define   COM12_HREF					( 0x80 )	//   HREF always
#define REG_COM13					( 0x3d )		// Control 13
#define   COM13_GAMMA					( 0x80 )	//   Gamma enable
#define   COM13_UVSAT					( 0x40 )	//   UV saturation auto adjustment
#define   COM13_UVSWAP					( 0x01 )	//   V before U - w/TSLB
#define REG_COM14					( 0x3e )		// Control 14
#define   COM14_DCWEN					( 0x10 )	//   DCW/PCLK-scale enable
#define REG_EDGE					( 0x3f )		// Edge enhancement factor
#define REG_COM15					( 0x40 )		// Control 15
#define   COM15_R10F0					( 0x00 )	//   Data range 10 to F0
#define   COM15_R01FE					( 0x80 )	//              01 to FE
#define   COM15_R00FF					( 0xc0 )	//              00 to FF
#define   COM15_RGB565					( 0x10 )	//   RGB565 output
#define   COM15_RGB555					( 0x30 )	//   RGB555 output
#define REG_COM16					( 0x41 )		// Control 16
#define   COM16_AWBGAIN					( 0x08 )	//   AWB gain enable
#define REG_COM17					( 0x42 )		// Control 17
#define   COM17_AECWIN					( 0xc0 )	//   AEC window - must match COM4
#define   COM17_CBAR					( 0x08 )	//   DSP Color bar

#define REG_DM_LNL					( 0x92 )
#define REG_DM_LNH					( 0x93 )

//--------------------------------------------------------------------------------------
// This matrix defines how the colors are generated, must be
// tweaked to adjust hue and saturation.
//
// Order: v-red, v-green, v-blue, u-red, u-green, u-blue
//
// They are nine-bit signed quantities, with the sign bit
// stored in 0x58.  Sign for v-red is bit 0, and up from there.
//--------------------------------------------------------------------------------------
#define REG_CMATRIX_BASE			( 0x4f )
#define   CMATRIX_LEN					( 6 )
#define REG_CMATRIX_SIGN			( 0x58 )

#define REG_BRIGHT					( 0x55 )		// Brightness
#define REG_CONTRAS					( 0x56 )		// Contrast control

#define REG_GFIX					( 0x69 )		// Fix gain control

#define REG_REG76					( 0x76 )		// OV's name
#define   R76_BLKPCOR					( 0x80 )	//   Black pixel correction enable
#define   R76_WHTPCOR     				( 0x40 )	//   White pixel correction enable

#define REG_RGB444					( 0x8c )		// RGB 444 control
#define   R444_ENABLE					( 0x02 )	//   Turn on RGB444, overrides 5x5
#define   R444_RGBX						( 0x01 )	//   Empty nibble at end

#define REG_HAECC1					( 0x9f )		// Hist AEC/AGC control 1
#define REG_HAECC2					( 0xa0 )		// Hist AEC/AGC control 2

#define REG_BD50MAX					( 0xa5 )		// 50hz banding step limit */
#define REG_HAECC3					( 0xa6 )		// Hist AEC/AGC control 3 */
#define REG_HAECC4					( 0xa7 )		// Hist AEC/AGC control 4 */
#define REG_HAECC5					( 0xa8 )		// Hist AEC/AGC control 5 */
#define REG_HAECC6					( 0xa9 )		// Hist AEC/AGC control 6 */
#define REG_HAECC7					( 0xaa )		// Hist AEC/AGC control 7 */
#define REG_BD60MAX					( 0xab )		// 60hz banding step limit */


//--------------------------------------------------------------------------------------
// The default register settings, as obtained from OmniVision.  There
// is really no making sense of most of these - lots of "reserved" values
// and such.
// 
// These settings give VGA YUYV.
//--------------------------------------------------------------------------------------
struct regval_list 
{
	unsigned char 		reg_num;
	unsigned char		value;
};



static struct regval_list OV7670_reg[] =
{
	{ REG_COM7, COM7_RESET }, 

     /*參和葎OV7670 QVGA RGB565歌方  */
	{ REG_GAIN, 0x00 },
	{ REG_BLUE, 0x80 },
	{ REG_RED , 0x80 },
//  {0x3a, 0x04},//
  	{REG_TSLB, 0x04},//
//	{0x12, 0x14},
//	{ REG_COM7, COM7_FMT_CIF | COM7_RGB }, 
	{ REG_COM7, COM7_FMT_QVGA | COM7_RGB }, 
//	{ REG_COM7, COM7_FMT_VGA | COM7_RGB }, 
	{ REG_RGB444, 0x00 },
	{ REG_COM15, COM15_RGB565},
	{REG_HREF, 0x80},
	{0x17, 0x16},
        
	{0x18, 0x04},//5
	{0x19, 0x02},
	{0x1a, 0x7b},//0x7a,
//	{REG_VREF, 0x06},//0x0a,
	{REG_VREF, 0x06},//0x0a,
	{REG_COM3, 0x0c},
    {REG_COM10, 0x02},
	{0x3e, 0x00},//10
	{0x70, 0x00},
	{0x71, 0x01},
	{0x72, 0x11},
	{0x73, 0x09},//
        
	{0xa2, 0x02},//15
//	{0x11, 0x00},
	{REG_CLKRC, 0x0F },
	{0x7a, 0x20},
	{0x7b, 0x1c},
	{0x7c, 0x28},
        
	{0x7d, 0x3c},//20
	{0x7e, 0x55},
	{0x7f, 0x68},
	{0x80, 0x76},
	{0x81, 0x80},
        
	{0x82, 0x88},
	{0x83, 0x8f},
	{0x84, 0x96},
	{0x85, 0xa3},
	{0x86, 0xaf},
        
	{0x87, 0xc4},//30
	{0x88, 0xd7},
	{0x89, 0xe8},
	{0x13, 0xe0},
	{0x00, 0x00},//AGC
        
	{0x10, 0x00},
	{REG_COM4, 0x00},
//	{0x14, 0x20},//0x38, limit the max gain
	{ REG_COM9, COM9_AGC_4X },
	{0xa5, 0x05},
	{0xab, 0x07},
        
	{0x24, 0x75},//40
	{0x25, 0x63},
	{0x26, 0xA5},
	{0x9f, 0x78},
	{0xa0, 0x68},
        
	{0xa1, 0x03},//0x0b,
	{0xa6, 0xdf},//0xd8,
	{0xa7, 0xdf},//0xd8,
	{0xa8, 0xf0},
	{0xa9, 0x90},
        
	{0xaa, 0x94},//50
//	{0x13, 0xe5},
	{REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AWB | COM8_AEC},
	{0x0e, 0x61},
	{0x0f, 0x4b},
	{0x16, 0x02},
        
//	{0x1e, 0x27},//0x07,
	{REG_MVFP, 0x37 },
	{0x21, 0x02},
	{0x22, 0x91},
	{0x29, 0x07},
	{0x33, 0x0b},
        
	{0x35, 0x0b},//60
	{0x37, 0x1d},
	{0x38, 0x71},
	{0x39, 0x2a},
	{0x3c, 0x78},
        
	{0x4d, 0x40},
	{0x4e, 0x20},
	{0x69, 0x5d},
	{0x6b, 0x40},//PLL
	{0x74, 0x19},
	{0x8d, 0x4f},
        
	{0x8e, 0x00},//70
	{0x8f, 0x00},
	{0x90, 0x00},
	{0x91, 0x00},
//	{0x92, 0x00},//0x19,//0x66
	{ REG_DM_LNL, 0x00},
	{ REG_DM_LNH, 0x00},
        
	{0x96, 0x00},
	{0x9a, 0x80},
	{0xb0, 0x84},
	{0xb1, 0x0c},
	{0xb2, 0x0e},
        
	{0xb3, 0x82},//80
	{0xb8, 0x0a},
	{0x43, 0x14},
	{0x44, 0xf0},
	{0x45, 0x34},
        
	{0x46, 0x58},
	{0x47, 0x28},
	{0x48, 0x3a},
	{0x59, 0x88},
	{0x5a, 0x88},
        
	{0x5b, 0x44},//90
	{0x5c, 0x67},
	{0x5d, 0x49},
	{0x5e, 0x0e},
	{0x64, 0x04},
	{0x65, 0x20},
        
	{0x66, 0x05},
	{0x94, 0x04},
	{0x95, 0x08},
	{0x6c, 0x0a},
	{0x6d, 0x55},
        
        
	{0x4f, 0x80},
	{0x50, 0x80},
	{0x51, 0x00},
	{0x52, 0x22},
	{0x53, 0x5e},
	{0x54, 0x80},
        
	//{0x54, 0x40},//110
        
        
	{0x6e, 0x11},//100
	{0x6f, 0x9f},//0x9e for advance AWB
    {0x55, 0x00},//疏業
    {0x56, 0x40},//斤曳業
    {0x57, 0x80},//0x40,  change according to Jim's request	      
	{ REG_COM17, 0x00 },
//	{ REG_COM17, COM17_CBAR },
	
	{ 0xff, 0xff },
};


#endif	// #ifndef __SENSOR_CONFIG_H__
