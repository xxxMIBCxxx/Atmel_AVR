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
	unsigned char Address;			// レジスタアドレス
	uint32_t BitLength;				// 長さ(Bit)
	uint32_t Default;				// #12026の設定値(0xFFFFFFFF:設定しない)
	bool bReadOnly;					// 読み取り専用(TRUE:読み取り専用)
} L6470_RegisterTable;




