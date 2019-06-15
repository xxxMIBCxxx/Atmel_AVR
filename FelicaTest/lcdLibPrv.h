/* LCD制御ライブラリ */
/* 内部関数 */

#ifndef _LCD_LIB_PRV_H_
#define _LCD_LIB_PRV_H_

#include "lcdlib.h"
#include "lcdConfig.h"


/***************/
/* 各deine定義 */
/***************/

/**************/
/* LCD種別    */
/**************/
#if CONF_LCD_TYPE == DEF_LCD_TYPE_1602	/* SC1602の場合 */

/* LCDの最大行列数 */
#define DEF_LCD_MAX_X		16		/* １行文字数 */
#define DEF_LCD_MAX_Y		2		/* 列数       */

#elif CONF_LCD_TYPE == DEF_LCD_TYPE_2004	/* SC2004の場合 */

/* LCDの最大行列数 */
#define DEF_LCD_MAX_X		20		/* １行文字数 */
#define DEF_LCD_MAX_Y		4		/* 列数       */

#else

#endif

/***************/
/* LCD種別 end */
/***************/


/* RSフラグ */
#define DEF_LCD_RS_COMM		0		/* 制御   */
#define DEF_LCD_RS_DATA		1		/* データ */

/* Read/Write */
#define DEF_LCD_RW_WRITE	0		/* 書き込み */
#define DEF_LCD_RW_READ		1		/* 読み出し */

/* Ebit */
#define DEF_LCD_E_LOW		0		/* Low */
#define DEF_LCD_E_HI		1		/* Hi  */

/* ビジー状態 */
#define DEF_LCD_READY		0		/* レディー */
#define DEF_LCD_BUSY		1		/* ビジー   */

/* コマンドの待ち時間 */
#define DEF_LCD_SHORT_WAIT	45		/* 40us用 余裕を持って45usとする */
#define DEF_LCD_LONG_WAIT	2		/* 2ms用 */

/* Lcd_WaitReadyの引数で使用するdefine */
#define DEF_LCD_WAIT_ONCE 1		/* １回読み出し   */
#define DEF_LCD_WAIT_READY 0	/* 完了待ちをする */

#endif
