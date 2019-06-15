#ifndef L6470LIB_H_
#define L6470LIB_H_

#include "avr/io.h"
#include "stdbool.h"

//#define L6470_BUSY			( _BV(PINA0) )					// BUSY出力・SYNC端子
//#define L6470_FLAG			( _BV(PINA1) )					// アラーム出力
//#define L6470_CS			( _BV(PINA2) )					// CS
//#define L6470_STCK			( _BV(PINA3) )					// ステップクロック入力
//#define L6470_STBY			( _BV(PINA4) )					// スタンバイ・リセット

#define L6470_CS			( _BV(PINB4) )					// CS
#define L6470_STBY			( _BV(PINA0) )					// スタンバイ・リセット

#define L6470_CS_LOW()		( PORTB &= ~L6470_CS )
#define L6470_CS_HI()		( PORTB |= L6470_CS )



#define L6470_ABS_POS		( 0x01 )						//
#define L6470_EL_POS		( 0x02 )						//
#define L6470_MARK 			( 0x03 )						//
#define L6470_SPEED			( 0x04 )						//
#define L6470_ACC			( 0x05 )						//
#define L6470_DEC			( 0x06 )						//
#define L6470_MAX_SPEED		( 0x07 )						//
#define L6470_MIN_SPEED		( 0x08 )						//
#define L6470_KVAL_HOLD		( 0x09 )						//
#define L6470_KVAL_RUN		( 0x0A )						//
#define L6470_KVAL_ACC		( 0x0B )						//
#define L6470_KVAL_DEC		( 0x0C )						//
#define L6470_INT_SPD		( 0x0D )						//
#define L6470_ST_SLP		( 0x0E )						//
#define L6470_FN_SLP_ACC	( 0x0F )						//
#define L6470_FN_SLP_DEC	( 0x10 )						//
#define L6470_K_THERM		( 0x11 )						//
#define L6470_ADC_OUT		( 0x12 )						//
#define L6470_ODC_TH		( 0x13 )						//
#define L6470_STALL_TH		( 0x14 )						//
#define L6470_FS_SPD		( 0x15 )						//
#define L6470_STEP_MODE		( 0x16 )						//
#define L6470_ALARM_EN		( 0x17 )						//
#define L6470_CONFIG		( 0x18 )						//
#define L6470_STATUS		( 0x19 )						//
#define L6470_RUN			( 0x51 )						// 正回転
#define L6470_R_RUN			( 0x50 )						// 逆回転
#define L6470_STOP			( 0xB0 )						// 停止



typedef struct 
{
	unsigned char Address;				// レジスタアドレス
	uint32_t uiDate;					// データ
} L6470_DebugTable;
L6470_DebugTable g_L6470Debug[100];

//-----------------------------------------------------------------------------
// L6470 初期化処理
//-----------------------------------------------------------------------------
void L6470_Init();

//-----------------------------------------------------------------------------
// L6470 パラメータ設定
//-----------------------------------------------------------------------------
bool L6470_SetParam( unsigned char Address, uint32_t uiValue );

//-----------------------------------------------------------------------------
// L6470 パラメータ取得
//-----------------------------------------------------------------------------
bool L6470_GetParam( unsigned char Address, uint32_t *puiValue );

//-----------------------------------------------------------------------------
// L6470 デバッグ用
//-----------------------------------------------------------------------------
void L6470_Debug( void );


#endif /* L6470LIB_H_ */