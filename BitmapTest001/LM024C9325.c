/*
 * LM024C9325_D8.c
 *
 * Created: 2014/03/23 11:22:28
 *  Author: MIBC
 */ 


#include "avr/io.h"
#include "avr/delay.h"
#include "LM024C9325.h"



// カラーバーの色
const int32_t COLOR_BAR[8] = 
{
	RGB( 63,  0,  0 ),		// 赤
	RGB(  0, 63,  0 ),		// 緑
	RGB(  0,  0, 63 ),		// 青
	RGB(  0, 63, 63 ),		// 水色
	RGB( 63, 63,  0 ),		// 黄色
	RGB( 63,  0, 63 ),		// 紫
	RGB(  0,  0,  0 ),		// 黒
	RGB( 63, 63, 63 ),		// 白
};


//-----------------------------------------------------------------------------
// 書込み
//-----------------------------------------------------------------------------
static void lm024c9325_write( char x_WriteData )
{
	// 書込みデータセット
	PORTA = x_WriteData;
	
	// 書込み
	PORTB = PORTB & ~LM024C9325_WR;
	PORTB = PORTB | LM024C9325_WR;
}


//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
void LM024C9325_Init()
{
	//************* Start Initial Sequence **********//
	LM024C9325_WriteRegister(0x0001, 0x0100); // set SS and SM bit
	LM024C9325_WriteRegister(0x0002, 0x0700); // set 1 line inversion
	LM024C9325_WriteRegister(0x0003, 0xD030); // set GRAM write direction and BGR=1.
	LM024C9325_WriteRegister(0x0004, 0x0000); // Resize register
	LM024C9325_WriteRegister(0x0008, 0x0207); // set the back porch and front porch
	LM024C9325_WriteRegister(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
	LM024C9325_WriteRegister(0x000A, 0x0000); // FMARK function
	LM024C9325_WriteRegister(0x000C, 0x0000); // RGB interface setting
	LM024C9325_WriteRegister(0x000D, 0x0000); // Frame marker Position
	LM024C9325_WriteRegister(0x000F, 0x0000); // RGB interface polarity
	//*************Power On sequence ****************//
	LM024C9325_WriteRegister(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LM024C9325_WriteRegister(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
	LM024C9325_WriteRegister(0x0012, 0x0000); // VREG1OUT voltage
	LM024C9325_WriteRegister(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	_delay_ms(200); // Dis-charge capacitor power voltage
	LM024C9325_WriteRegister(0x0010, 0x1490); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LM024C9325_WriteRegister(0x0011, 0x0227); // Set DC1[2:0], DC0[2:0], VC[2:0]
	_delay_ms(50); // Delay 50ms
	LM024C9325_WriteRegister(0x0012, 0x001A); // External reference voltage= Vci;
	_delay_ms(50); // Delay 50ms
	LM024C9325_WriteRegister(0x0013, 0x1400); // VDV[4:0] for VCOM amplitude
	LM024C9325_WriteRegister(0x0029, 0x0019); // VCM[5:0] for VCOMH
	LM024C9325_WriteRegister(0x002B, 0x000B); // Set Frame Rate
	_delay_ms(50); // Delay 50ms
	LM024C9325_WriteRegister(0x0020, 0x0000); // GRAM horizontal Address
	LM024C9325_WriteRegister(0x0021, 0x0000); // GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	LM024C9325_WriteRegister(0x0030, 0x0000);
	LM024C9325_WriteRegister(0x0031, 0x0607);
	LM024C9325_WriteRegister(0x0032, 0x0305);
	LM024C9325_WriteRegister(0x0035, 0x0000);
	LM024C9325_WriteRegister(0x0036, 0x1604);
	LM024C9325_WriteRegister(0x0037, 0x0204);
	LM024C9325_WriteRegister(0x0038, 0x0001);
	LM024C9325_WriteRegister(0x0039, 0x0707);
	LM024C9325_WriteRegister(0x003C, 0x0000);
	LM024C9325_WriteRegister(0x003D, 0x000F);
	//------------------ Set GRAM area ---------------//
	LM024C9325_WriteRegister(0x0050, 0x0000); // Horizontal GRAM Start Address
	LM024C9325_WriteRegister(0x0051, 0x00EF); // Horizontal GRAM End Address
	LM024C9325_WriteRegister(0x0052, 0x0000); // Vertical GRAM Start Address
	LM024C9325_WriteRegister(0x0053, 0x013F); // Vertical GRAM Start Address
	LM024C9325_WriteRegister(0x0060, 0xA700); // Gate Scan Line
	LM024C9325_WriteRegister(0x0061, 0x0001); // NDL,VLE, REV
	LM024C9325_WriteRegister(0x006A, 0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	LM024C9325_WriteRegister(0x0080, 0x0000);
	LM024C9325_WriteRegister(0x0081, 0x0000);
	LM024C9325_WriteRegister(0x0082, 0x0000);
	LM024C9325_WriteRegister(0x0083, 0x0000);
	LM024C9325_WriteRegister(0x0084, 0x0000);
	LM024C9325_WriteRegister(0x0085, 0x0000);
	//-------------- Panel Control -------------------//
	LM024C9325_WriteRegister(0x0090, 0x0010);
	LM024C9325_WriteRegister(0x0092, 0x0600);
	LM024C9325_WriteRegister(0x0093, 0x0003);
	LM024C9325_WriteRegister(0x0095, 0x0110);
	LM024C9325_WriteRegister(0x0097, 0x0000);
	LM024C9325_WriteRegister(0x0098, 0x0000);
	LM024C9325_WriteRegister(0x0007, 0x0133); // 262K color and display ON
}


//-----------------------------------------------------------------------------
// リセット処理
//-----------------------------------------------------------------------------
void LM024C9325_Reset()
{
	PORTB = PORTB & ~LM024C9325_RST;		// RST Low
	_delay_ms( 5 );
	PORTB = PORTB | LM024C9325_RST;			// RST Hi
	_delay_ms( 5 );
}


//-----------------------------------------------------------------------------
// Register書込み
//-----------------------------------------------------------------------------
void LM024C9325_WriteRegister( int16_t x_iIndex, int16_t x_iData )
{
	PORTB = PORTB | LM024C9325_RD;			// RD Hi
	PORTB = PORTB & ~LM024C9325_CS;			// CS Low

	// Index
	PORTB = PORTB & ~LM024C9325_RS;			// RS Low
	lm024c9325_write( 0x00 );
	lm024c9325_write( (char)x_iIndex );
	PORTB = PORTB | LM024C9325_RS;			// RS Hi
	
	// Data
	lm024c9325_write( (char)(x_iData >> 8) );
	lm024c9325_write( (char)x_iData );
	
	PORTB = PORTB | LM024C9325_CS;			// CS Hi
}


//-----------------------------------------------------------------------------
// GRAM書込み開始
// ※GRAMに書込みを行う前に、本関数をコールすること
//-----------------------------------------------------------------------------
void LM024C9325_StartGRAM( )
{
	PORTB = PORTB | LM024C9325_RD;			// RD Hi
	PORTB = PORTB & ~LM024C9325_CS;			// CS Low

	// Read Data from GRAM
	PORTB = PORTB & ~LM024C9325_RS;			// RS Low
	lm024c9325_write( 0x00 );
	lm024c9325_write( 0x22 );
	PORTB = PORTB | LM024C9325_RS;			// RS Hi
	PORTB = PORTB | LM024C9325_CS;			// CS Hi		
}


//-----------------------------------------------------------------------------
// GRAM書込み
// ※GRAMに書込みを行う前に、LM024C9325_StartGRAMをコールすること
//-----------------------------------------------------------------------------
void LM024C9325_WriteGRAM( int32_t x_iColor )
{
	int32_t	iColor = 0;
	
	PORTB = PORTB & ~LM024C9325_CS;			// CS Low
	iColor = (x_iColor & 0x3F000) >> 10;
	lm024c9325_write( (char)iColor );
	iColor = (x_iColor & 0x00FC0) >> 4;
	lm024c9325_write( (char)iColor );
	iColor = (x_iColor & 0x0003F) << 2;
	lm024c9325_write( (char)iColor );
	PORTB = PORTB | LM024C9325_CS;			// CS Hi	
}


//-----------------------------------------------------------------------------
// カラーバー表示（テスト用）
//----------------------------------------------------------------------------
void LM024C9325_ColorBar()
{
	unsigned int i,n,m;

	// GRAM書込み開始
	LM024C9325_StartGRAM();
	
	// LCDにカラーバーを表示する
	for(n = 0; n < 8; n++)
	{
		for(i = 0; i < 320/8; i++)
		{
			for(m = 0; m < 240; m++)
			{
				LM024C9325_WriteGRAM( COLOR_BAR[n] );
			}
		}
	}							
}


