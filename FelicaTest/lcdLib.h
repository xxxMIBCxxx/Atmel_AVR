/* LCD制御ライブラリ */
/* 公開関数 */

/* 基本的設定4bit */
/* インクリメント */
/* 表示シフトあり */
/* カーソルオフ   */
/* ブリンクオフ   */
/* 5x7サイズ      */

#ifndef _LCD_LIB_H_
#define _LCD_LIB_H_
#include "def.h"

/* ライブラリバージョン */
#define DEF_LCD_LIB_VER		0.1


/*-------------------------------------------------------*/
/*-------------------------------------------------------*/
/* 本ライブラリを使用する場合は                          */
/* common、lcdLibフォルダにincludeパスを設定してください */
/*-------------------------------------------------------*/
/*-------------------------------------------------------*/


/*-----------------------------------------------*/
/*-----------------------------------------------*/
/* 本ライブラリ使用時には                        */
/* lcdConfig.hを設定してください                 */
/*-----------------------------------------------*/
/*-----------------------------------------------*/


/*******************************************************/
/* 関数	：	LCD初期化                                  */
/* 説明	：	本ライブラリで使用するポートの初期化および */
/*			LCDを初期状態にリセットします              */
/*			LCDを使用する最初に呼び出してください      */
/*******************************************************/
void Lcd_Init(void);

 
/********************************************************/
/* 関数	：	LCDに文字を表示                             */
/* 引数1：	表示する文字列                              */
/* 引数2：	表示する文字数（引数１の文字長を指定）      */
/* 説明	：	LCDに文字列を表示します                     */
/*			表示開始位置はLcd_MoveCurで指定してください */
/********************************************************/
void Lcd_Write(uint8_t *szStr, uint8_t ucLen);


/********************************************************/
/* 関数	：	カーソル位置移動                            */
/* 引数1：	カーソル移動先のX座標（0オリジン）          */
/* 引数2：	カーソル移動先のY座標（0オリジン）          */
/* 説明	：	表示開始位置のX,Y座標を指定します           */
/*			SC16002の場合 Xが15、Yが1が最大です         */
/********************************************************/
void Lcd_MoveCur(uint8_t x, uint8_t y);


/********************************************************/
/* 関数	：	LCD消去                                     */
/* 説明	：	LCDの表示をすべて消去します                 */
/*			カーソル位置は(0,0)に移動します             */
/********************************************************/
void Lcd_Clear(void);


/*******************************************************************/
/* 関数	：	LCD off                                                */
/* 説明	：	LCDの表示をOFFします                                   */
/*			表示データはクリアしません（ONすれば再度表示されます） */
/*******************************************************************/
void Lcd_Off(void);


/*******************************************/
/* 関数	：	LCD on                         */
/* 説明	：	LCDの表示をONします            */
/*			表示データはクリアしません     */
/*******************************************/
void Lcd_On(void);


#endif
