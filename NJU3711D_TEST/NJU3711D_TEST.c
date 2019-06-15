/*
 * NJU3711D_TEST.c
 *
 * Created: 2012/07/15 22:54:24
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include "util/delay.h"
#include <stdbool.h>

#define NJU3711_DATA		(_BV(PINA0))
#define NJU3711_CLK			(_BV(PINA1))
#define NJU3711_STB			(_BV(PINA2))
#define NJU3711_CLR			(_BV(PINA3))

#define DELAY_TIME			( 100 )


//-------------------------------------------------------------------------------------------------
// NJU3711初期処理
//-------------------------------------------------------------------------------------------------
void NJU3711_Init(void)
{
	PORTA |= NJU3711_CLR | NJU3711_STB & (PORTA & ~(NJU3711_DATA | NJU3711_CLK));
	PORTA &= ~NJU3711_CLR;
	PORTA |= NJU3711_CLR;
}


//-------------------------------------------------------------------------------------------------
// NJU3711データ設定
//-------------------------------------------------------------------------------------------------
void NJU3711_SetData( unsigned char Data )
{
	unsigned char		i = 0;
	unsigned char		temp = 0x00;
	

	// NJU3711のシフトレジスタにデータを設定
	PORTA |= NJU3711_STB;
	
	// 8bit分ループ
	for ( i = 0 ; i < 8 ; i++ )
	{
		temp = Data & 0x80;
		if (temp)
		{
			PORTA |= NJU3711_DATA;
		}
		else
		{		
			PORTA &= ~(NJU3711_DATA);
		}
		PORTA |= NJU3711_CLK;
		PORTA &= ~(NJU3711_DATA | NJU3711_CLK);

		Data = Data << 1;
	}
	
	// シフトレジスタの内容をラッチ回路に転送
	PORTA &= ~(NJU3711_STB | NJU3711_DATA);
}


//-------------------------------------------------------------------------------------------------
// NJU3711シフト処理
//-------------------------------------------------------------------------------------------------
void NJU3711_Shift( bool bBit )
{
	PORTA &= ~NJU3711_STB;
	if ( bBit == false )
	{
		PORTA |= NJU3711_CLK | NJU3711_DATA;
	}
	else
	{
		PORTA |= NJU3711_CLK | (PORTA & ~NJU3711_DATA);
	}
	PORTA &= ~(NJU3711_CLK | NJU3711_DATA);
	PORTA |= NJU3711_STB;	
}


int main(void)
{
	int16_t			i = 0;
	unsigned char	Data = 0x00;
	
	
	DDRA  = 0b00001111;
	PORTA = 0b00000000;

	// NJU3711初期化
	NJU3711_Init();

	
	while ( 1 )
	{
		//-----------------------------------
		NJU3711_SetData( ~(0x01) ); 
		for ( i = 0 ; i < 8 ; i++ )	
		{
			_delay_ms(DELAY_TIME);
			NJU3711_Shift(false);
		}
		_delay_ms(DELAY_TIME);

		//-----------------------------------
		for ( i = 0 ; i < 8 ; i++ )	
		{
			_delay_ms(DELAY_TIME);
			NJU3711_Shift(true);
		}
		_delay_ms(DELAY_TIME);
	
		//-----------------------------------
		Data = 0x80;
		for ( i = 0 ; i <= 8 ; i++ )	
		{
			_delay_ms(DELAY_TIME);
			NJU3711_SetData( ~(Data) ); 
			Data = Data >> 1;
		}
		_delay_ms(DELAY_TIME);
	
		//-----------------------------------
		for ( i = 0 ; i <= 32 ; i++ )
		{
			if ((i % 2) || (i > (32 -8)) )
			{
				NJU3711_Shift(false);
			}
			else
			{
				NJU3711_Shift(true);
			}
			_delay_ms(DELAY_TIME);		
		}
		_delay_ms(DELAY_TIME);
	}		
	
	return 0;
}
