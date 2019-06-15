/*
 * _74HC595_TEST.c
 *
 * Created: 2012/07/16 13:43:40
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"



#define _74HC595_RESET			( _BV(PINA0) )
#define _74HC595_SHIFT_CLOCK	( _BV(PINA1) )
#define _74HC595_LATCH_CLOCK	( _BV(PINA2) )
#define _74HC595_OUTPUT_ENABLE	( _BV(PINA3) )
#define _74HC595_DATA			( _BV(PINA4) )


#define DELAY_TIME				( 100 )

//-------------------------------------------------------------------------------------------------
// 74HC595リセット
//-------------------------------------------------------------------------------------------------
void _74HC595_Reset(void)
{
	PORTA &= ~(_74HC595_RESET | _74HC595_OUTPUT_ENABLE);
	PORTA |= _74HC595_RESET;
}

//-------------------------------------------------------------------------------------------------
// 74HC595初期化
//-------------------------------------------------------------------------------------------------
void _74HC595_Init(void)
{
	PORTA |= _74HC595_RESET | (PORTA &~(_74HC595_SHIFT_CLOCK | _74HC595_LATCH_CLOCK | _74HC595_OUTPUT_ENABLE | _74HC595_DATA));
	_74HC595_Reset();
}

//-------------------------------------------------------------------------------------------------
// 74HC595データ設定
//-------------------------------------------------------------------------------------------------
void _74HC595_SetData( uint16_t Data, bool bTrans )
{
	unsigned char		i = 0;
	uint16_t			temp = 0x00;

	
	PORTA |= _74HC595_RESET | (PORTA &~(_74HC595_SHIFT_CLOCK | _74HC595_LATCH_CLOCK | _74HC595_OUTPUT_ENABLE | _74HC595_DATA));
	
	for ( i = 0 ; i < 16 ; i++ )
	{
		// 0x0000 - 0x00FF
		temp = Data & 0x8000;
		if (temp)
		{
			PORTA |= _74HC595_DATA;
		}
		else
		{
			PORTA &= ~_74HC595_DATA;
		}
		
		PORTA |= _74HC595_SHIFT_CLOCK;
		PORTA &= ~_74HC595_SHIFT_CLOCK;			
		Data = Data << 1;
	}
			
	// 転送する？
	if ( bTrans )
	{
		_74HC595_TransferData();
	}
	

}


//-------------------------------------------------------------------------------------------------
// 74HC595データ転送
//-------------------------------------------------------------------------------------------------
void _74HC595_TransferData( void )
{
	PORTA |= _74HC595_RESET | (PORTA &~(_74HC595_SHIFT_CLOCK | _74HC595_LATCH_CLOCK | _74HC595_OUTPUT_ENABLE | _74HC595_DATA));
	PORTA |= _74HC595_LATCH_CLOCK;
	PORTA &= ~_74HC595_LATCH_CLOCK;
}


//-------------------------------------------------------------------------------------------------
// 74HC595シフト
//-------------------------------------------------------------------------------------------------
void _74HC595_Shift( bool bBit, bool bTrans )
{
	PORTA |= _74HC595_RESET | (PORTA &~(_74HC595_SHIFT_CLOCK | _74HC595_LATCH_CLOCK | _74HC595_OUTPUT_ENABLE | _74HC595_DATA));
	
	if (bBit)
	{
		PORTA |= _74HC595_DATA;
	}
	else
	{
		PORTA &= ~_74HC595_DATA;
	}
	PORTA |= _74HC595_SHIFT_CLOCK;
	PORTA &= ~_74HC595_SHIFT_CLOCK;	
	
	// 転送する？
	if ( bTrans )
	{
		_74HC595_TransferData();
	}
}



int main(void)
{
	uint16_t		i = 0;
	uint16_t		Data = 0x0000;
	
	
	DDRA  = 0b00011111;
	
	// 74HC595初期化
	_74HC595_Init();
	
	// 74HC595リセット
	_74HC595_Reset();
	
	while(1)
    {
		Data = 0x0001;
		_74HC595_SetData( ~Data, true );
		_delay_ms(DELAY_TIME);	
		for ( i = 0 ; i < 15 ; i++ )
		{
			_74HC595_Shift( true, true );
			_delay_ms(DELAY_TIME);
		}
		_delay_ms(DELAY_TIME);
		
		Data = 0x8000;
		_74HC595_SetData( ~Data, true );
		_delay_ms(DELAY_TIME);	
		for ( i = 0 ; i < 16 ; i++ )
		{
			_74HC595_SetData( ~Data, true );
			_delay_ms(DELAY_TIME);
			Data = Data >> 1;
		}
		_delay_ms(DELAY_TIME);
		
		for ( i = 0 ; i < 128 ; i++ )
		{
			if ( ( i%2 ) || (i > (128-16)) )
			{
				_74HC595_Shift( false, true );
			}
			else
			{
				_74HC595_Shift( true, true );
			}
			_delay_ms(DELAY_TIME);
		}
		
		
		for ( i = 0 ; i < 1024 ; i++ )
		{
			_74HC595_SetData( ~i, true );
			_delay_ms(10);
		}
		_74HC595_SetData( ~0, true );
		_delay_ms(10);
    }
	
	
	return 0;
}