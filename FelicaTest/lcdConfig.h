/* LCD制御ライブラリ */
/* ポート設定 */

#ifndef _LCD_LIB_CONFIG_H_
#define _LCD_LIB_CONFIG_H_

#include "def.h"

/****************************************/
/* 有効にするとビジーを確認し動作します */
/* #define CONF_LCD_BUSY_READ           */
/****************************************/
//#define CONF_LCD_BUSY_READ


/*******************************************/
/* SC1602  SC2004の種別選択                */
/* SC1602 : 0                              */
/* SC2004 : 1                              */
/* こちらは選択肢                          */
/*******************************************/
#define DEF_LCD_TYPE_1602	0
#define DEF_LCD_TYPE_2004	1

/*******************************************/
/* ココに設定                              */
#define CONF_LCD_TYPE	DEF_LCD_TYPE_2004
/*******************************************/






/******************************************************/
/* 各ポートへのアクセスdefine                         */
/* 各ポートと実ポートを組み合わせてください           */
/******************************************************/

/* サンプル */
/*#define LCD_PORTB	((volatile ST_PORT_BITFLD*)&PORTB)->uni.bit */
/*#define LCD_DDRB	((volatile ST_PORT_BITFLD*)&DDRB)->uni.bit */
/*#define LCD_PINB	((volatile ST_PORT_BITFLD*)&PINB)->uni.bit */

/********************************************************/


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* 使用ポートの設定 */
/* PORTA */
#define LCD_PORTA	((volatile ST_PORT_BITFLD*)&PORTA)->uni.bit
#define LCD_DDRA	((volatile ST_PORT_BITFLD*)&DDRA)->uni.bit
#define LCD_PINA	((volatile ST_PORT_BITFLD*)&PINA)->uni.bit

/* PORTC */
#define LCD_PORTC	((volatile ST_PORT_BITFLD*)&PORTC)->uni.bit
#define LCD_DDRC	((volatile ST_PORT_BITFLD*)&DDRC)->uni.bit
#define LCD_PINC	((volatile ST_PORT_BITFLD*)&PINC)->uni.bit

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



/********************************/
/* 各制御レジスタとポートの設定 */
/********************************/

/* レジスタ選択 */
#define PIN_LCD_RS_PORT		LCD_PORTA.P0
#define PIN_LCD_RS_DDR		LCD_DDRA.P0
#define PIN_LCD_RS_PIN		LCD_PINA.P0

/* Read/Write選択 */
#define PIN_LCD_RW_PORT		LCD_PORTA.P1
#define PIN_LCD_RW_DDR		LCD_DDRA.P1
#define PIN_LCD_RW_PIN		LCD_PINA.P1

/* Enable信号 */
#define PIN_LCD_E_PORT		LCD_PORTA.P2
#define PIN_LCD_E_DDR		LCD_DDRA.P2
#define PIN_LCD_E_PIN		LCD_PINA.P2

/* DB4 */
#define PIN_LCD_DB4_PORT	LCD_PORTA.P3
#define PIN_LCD_DB4_DDR		LCD_DDRA.P3
#define PIN_LCD_DB4_PIN		LCD_PINA.P3

/* DB5 */
#define PIN_LCD_DB5_PORT	LCD_PORTA.P4
#define PIN_LCD_DB5_DDR		LCD_DDRA.P4
#define PIN_LCD_DB5_PIN		LCD_PINA.P4

/* DB6 */
#define PIN_LCD_DB6_PORT	LCD_PORTA.P5
#define PIN_LCD_DB6_DDR		LCD_DDRA.P5
#define PIN_LCD_DB6_PIN		LCD_PINA.P5

/* DB7 */
#define PIN_LCD_DB7_PORT	LCD_PORTA.P6
#define PIN_LCD_DB7_DDR		LCD_DDRA.P6
#define PIN_LCD_DB7_PIN		LCD_PINA.P6

#endif
