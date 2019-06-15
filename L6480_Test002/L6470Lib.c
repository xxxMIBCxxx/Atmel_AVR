/*
 * L6470Lib.c
 *
 * Created: 2014/03/31 10:59:57
 *  Author: MIBC
 */ 
#include "L6470Lib.h"
#include "SpiLib.h"
#include "util/delay.h"


#define L6470_SET_PARAM_COMAND			( 0x00 )
#define L6470_GET_PARAM_COMAND			( 0x20 )


typedef struct
{
	unsigned char Address;			// ���W�X�^�A�h���X
	uint32_t BitLength;				// ����(Bit)
	uint32_t Default;				// #12026�̐ݒ�l(0xFFFFFFFF:�ݒ肵�Ȃ�)
	bool bReadOnly;					// �ǂݎ���p(TRUE:�ǂݎ���p)
} L6470_RegisterTable;




