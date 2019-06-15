/*
 * LM024C9325.h
 *
 * Created: 2014/03/23 11:11:28
 *  Author: MIBC
 */ 


#ifndef LM024C9325_H_
#define LM024C9325_H_


#define LM024C9325_RST		( _BV(PINB0) )				// RST
#define LM024C9325_CS		( _BV(PINB1) )				// Chip Select signal
#define LM024C9325_RS		( _BV(PINB2) )				// Register Select signal
#define LM024C9325_WR		( _BV(PINB3) )				// Write Strobe signal(and enables an operation to write data)
#define LM024C9325_RD		( _BV(PINB4) )				// Read Strobe signal(and enables an operation to read out data)

#define LM024C9325_D8		( _BV(PINA0) )				// data bus(DB8)
#define LM024C9325_D9		( _BV(PINA1) )				// data bus(DB9)
#define LM024C9325_D10		( _BV(PINA2) )				// data bus(DB10)
#define LM024C9325_D11		( _BV(PINA3) )				// data bus(DB11)
#define LM024C9325_D12		( _BV(PINA4) )				// data bus(DB12)
#define LM024C9325_D13		( _BV(PINA5) )				// data bus(DB13)
#define LM024C9325_D14		( _BV(PINA6) )				// data bus(DB14)
#define LM024C9325_D15		( _BV(PINA7) )				// data bus(DB15)



// 4線抵抗膜型タッチスクリーン用
#define LM024C9325_XL		( 0b00000000 )
#define LM024C9325_YR		( 0b00000000 )
#define LM024C9325_XR		( 0b00000000 )
#define LM024C9325_YT		( 0b00000000 )


// 色設定
#define RGB( red, green, blue )			( ((red & 0x3F) << 12) | ((green & 0x3F) << 6) | (blue & 0x3F) )


//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
void LM024C9325_Init();

//-----------------------------------------------------------------------------
// リセット処理
//-----------------------------------------------------------------------------
void LM024C9325_Reset();

//-----------------------------------------------------------------------------
// Register書込み
//-----------------------------------------------------------------------------
void LM024C9325_WriteRegister( int16_t x_iIndex, int16_t x_iData );

//-----------------------------------------------------------------------------
// GRAM書込み開始
// ※GRAMに書込みを行う前に、本関数をコールすること
//-----------------------------------------------------------------------------
void LM024C9325_StartGRAM();

//-----------------------------------------------------------------------------
// GRAM書込み
// ※GRAMに書込みを行う前に、LM024C9325_StartGRAMをコールすること
//-----------------------------------------------------------------------------
void LM024C9325_WriteGRAM( int32_t x_iColor );

//-----------------------------------------------------------------------------
// カラーバー表示（テスト用）
//----------------------------------------------------------------------------
void LM024C9325_ColorBar();

#endif /* LM024C9325_H_ */