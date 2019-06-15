/* LCD���䃉�C�u���� */
/* �����֐� */

#ifndef _LCD_LIB_PRV_H_
#define _LCD_LIB_PRV_H_

#include "lcdlib.h"
#include "lcdConfig.h"


/***************/
/* �edeine��` */
/***************/

/**************/
/* LCD���    */
/**************/
#if CONF_LCD_TYPE == DEF_LCD_TYPE_1602	/* SC1602�̏ꍇ */

/* LCD�̍ő�s�� */
#define DEF_LCD_MAX_X		16		/* �P�s������ */
#define DEF_LCD_MAX_Y		2		/* ��       */

#elif CONF_LCD_TYPE == DEF_LCD_TYPE_2004	/* SC2004�̏ꍇ */

/* LCD�̍ő�s�� */
#define DEF_LCD_MAX_X		20		/* �P�s������ */
#define DEF_LCD_MAX_Y		4		/* ��       */

#else

#endif

/***************/
/* LCD��� end */
/***************/


/* RS�t���O */
#define DEF_LCD_RS_COMM		0		/* ����   */
#define DEF_LCD_RS_DATA		1		/* �f�[�^ */

/* Read/Write */
#define DEF_LCD_RW_WRITE	0		/* �������� */
#define DEF_LCD_RW_READ		1		/* �ǂݏo�� */

/* Ebit */
#define DEF_LCD_E_LOW		0		/* Low */
#define DEF_LCD_E_HI		1		/* Hi  */

/* �r�W�[��� */
#define DEF_LCD_READY		0		/* ���f�B�[ */
#define DEF_LCD_BUSY		1		/* �r�W�[   */

/* �R�}���h�̑҂����� */
#define DEF_LCD_SHORT_WAIT	45		/* 40us�p �]�T��������45us�Ƃ��� */
#define DEF_LCD_LONG_WAIT	2		/* 2ms�p */

/* Lcd_WaitReady�̈����Ŏg�p����define */
#define DEF_LCD_WAIT_ONCE 1		/* �P��ǂݏo��   */
#define DEF_LCD_WAIT_READY 0	/* �����҂������� */

#endif
