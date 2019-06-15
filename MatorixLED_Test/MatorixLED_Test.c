/*
 * MatorixLED_Test.c
 *
 * Created: 2012/07/20 1:37:35
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "avr/interrupt.h"

// MATRIX LED（列）
#define _74HC595_COL_RESET			( _BV(PINA5) )
#define _74HC595_COL_SHIFT_CLOCK	( _BV(PINA4) )
#define _74HC595_COL_LATCH_CLOCK	( _BV(PINA3) )
#define _74HC595_COL_OUTPUT_ENABLE	( _BV(PINA2) )
#define _74HC595_COL_DATA			( _BV(PINA1) )

// MATRIX LED（行）
#define _74HC595_ROW_RESET			( _BV(PINB4) )
#define _74HC595_ROW_SHIFT_CLOCK	( _BV(PINB3) )
#define _74HC595_ROW_LATCH_CLOCK	( _BV(PINB2) )
#define _74HC595_ROW_OUTPUT_ENABLE	( _BV(PINB1) )
#define _74HC595_ROW_DATA			( _BV(PINB0) )


#define DELAY_TIME					( 100 )


#define MATORIX_LED_ROW_MAX			( 16 )
#define MATORIX_LED_COL_MAX			( 32 )

volatile uint32_t	g_MatorixLed[ MATORIX_LED_ROW_MAX ] = 
{ 
	0x0007C000,
	0x000FE000,
	0x001BB000,
	0x00311800,
	0x00783C00,
	0x00793C00,
	0x00FBBE00,
	0x00B39A00,
	0x00838200,
	0x00810200,
	0x009FF200,
	0x00729C00,
	0x00228800,
	0x00200800,
	0x00101000,
	0x000FE000
};


volatile unsigned char	g_MatorixLedCol = 0;
volatile unsigned char	g_MatorixLedRol = 0;
volatile uint16_t		g_Count = 0;
volatile uint16_t		g_MoveCount = 0;
volatile bool			g_bRevers = false;


//-------------------------------------------------------------------------------------------------
// 74HC595リセット
//-------------------------------------------------------------------------------------------------
void _74HC595_COL_Reset(void)
{
	PORTA &= ~(_74HC595_COL_RESET | _74HC595_COL_OUTPUT_ENABLE);
	PORTA |= _74HC595_COL_RESET;
}

//-------------------------------------------------------------------------------------------------
// 74HC595初期化
//-------------------------------------------------------------------------------------------------
void _74HC595_COL_Init(void)
{
	PORTA |= _74HC595_COL_RESET | (PORTA &~(_74HC595_COL_SHIFT_CLOCK | _74HC595_COL_LATCH_CLOCK | _74HC595_COL_OUTPUT_ENABLE | _74HC595_COL_DATA));
	_74HC595_COL_Reset();
}

//-------------------------------------------------------------------------------------------------
// 74HC595データ設定
//-------------------------------------------------------------------------------------------------
void _74HC595_COL_SetData( uint32_t Data, bool bTrans )
{
	unsigned char		i = 0;
	uint32_t			mask = 0x80000000;
	uint32_t			temp = 0x00000000;

	
	PORTA |= _74HC595_COL_RESET | (PORTA &~(_74HC595_COL_SHIFT_CLOCK | _74HC595_COL_LATCH_CLOCK | _74HC595_COL_OUTPUT_ENABLE | _74HC595_COL_DATA));
	
	for ( i = 0 ; i < 32 ; i++ )
	{
		// 0x0000 - 0x00FF
		temp = Data & mask;
		if (temp)
		{
			PORTA |= _74HC595_COL_DATA;
		}
		else
		{
			PORTA &= ~_74HC595_COL_DATA;
		}
		
		PORTA |= _74HC595_COL_SHIFT_CLOCK;
		PORTA &= ~_74HC595_COL_SHIFT_CLOCK;			
		mask = mask >> 1;
	}
			
	// 転送する？
	if ( bTrans )
	{
		_74HC595_COL_TransferData();
	}
	

}


//-------------------------------------------------------------------------------------------------
// 74HC595データ転送
//-------------------------------------------------------------------------------------------------
void _74HC595_COL_TransferData( void )
{
	PORTA |= _74HC595_COL_RESET | (PORTA &~(_74HC595_COL_SHIFT_CLOCK | _74HC595_COL_LATCH_CLOCK | _74HC595_COL_OUTPUT_ENABLE | _74HC595_COL_DATA));
	PORTA |= _74HC595_COL_LATCH_CLOCK;
	PORTA &= ~_74HC595_COL_LATCH_CLOCK;
}


//-------------------------------------------------------------------------------------------------
// 74HC595シフト
//-------------------------------------------------------------------------------------------------
void _74HC595_COL_Shift( bool bBit, bool bTrans )
{
	PORTA |= _74HC595_COL_RESET | (PORTA &~(_74HC595_COL_SHIFT_CLOCK | _74HC595_COL_LATCH_CLOCK | _74HC595_COL_OUTPUT_ENABLE | _74HC595_COL_DATA));
	
	if (bBit)
	{
		PORTA |= _74HC595_COL_DATA;
	}
	else
	{
		PORTA &= ~_74HC595_COL_DATA;
	}
	PORTA |= _74HC595_COL_SHIFT_CLOCK;
	PORTA &= ~_74HC595_COL_SHIFT_CLOCK;	
	
	// 転送する？
	if ( bTrans )
	{
		_74HC595_COL_TransferData();
	}
}







//-------------------------------------------------------------------------------------------------
// 74HC595リセット
//-------------------------------------------------------------------------------------------------
void _74HC595_ROW_Reset(void)
{
	PORTB &= ~(_74HC595_ROW_RESET | _74HC595_ROW_OUTPUT_ENABLE);
	PORTB |= _74HC595_ROW_RESET;
}

//-------------------------------------------------------------------------------------------------
// 74HC595初期化
//-------------------------------------------------------------------------------------------------
void _74HC595_ROW_Init(void)
{
	PORTB |= _74HC595_ROW_RESET | (PORTB &~(_74HC595_ROW_SHIFT_CLOCK | _74HC595_ROW_LATCH_CLOCK | _74HC595_ROW_OUTPUT_ENABLE | _74HC595_ROW_DATA));
	_74HC595_ROW_Reset();
}

//-------------------------------------------------------------------------------------------------
// 74HC595データ設定
//-------------------------------------------------------------------------------------------------
void _74HC595_ROW_SetData( uint32_t Data, bool bTrans )
{
	unsigned char		i = 0;
	uint32_t			mask = 0x80000000;
	uint32_t			temp = 0x00000000;

	
	PORTB |= _74HC595_ROW_RESET | (PORTB &~(_74HC595_ROW_SHIFT_CLOCK | _74HC595_ROW_LATCH_CLOCK | _74HC595_ROW_OUTPUT_ENABLE | _74HC595_ROW_DATA));
	
	for ( i = 0 ; i < 32 ; i++ )
	{
		// 0x0000 - 0x00FF
		temp = Data & mask;
		if (temp)
		{
			PORTB |= _74HC595_ROW_DATA;
		}
		else
		{
			PORTB &= ~_74HC595_ROW_DATA;
		}
		
		PORTB |= _74HC595_ROW_SHIFT_CLOCK;
		PORTB &= ~_74HC595_ROW_SHIFT_CLOCK;			
		mask = mask >> 1;
	}
			
	// 転送する？
	if ( bTrans )
	{
		_74HC595_ROW_TransferData();
	}
	

}


//-------------------------------------------------------------------------------------------------
// 74HC595データ転送
//-------------------------------------------------------------------------------------------------
void _74HC595_ROW_TransferData( void )
{
	PORTB |= _74HC595_ROW_RESET | (PORTB &~(_74HC595_ROW_SHIFT_CLOCK | _74HC595_ROW_LATCH_CLOCK | _74HC595_ROW_OUTPUT_ENABLE | _74HC595_ROW_DATA));
	PORTB |= _74HC595_ROW_LATCH_CLOCK;
	PORTB &= ~_74HC595_ROW_LATCH_CLOCK;
}


//-------------------------------------------------------------------------------------------------
// 74HC595シフト
//-------------------------------------------------------------------------------------------------
void _74HC595_ROW_Shift( bool bBit, bool bTrans )
{
	PORTB |= _74HC595_ROW_RESET | (PORTB &~(_74HC595_ROW_SHIFT_CLOCK | _74HC595_ROW_LATCH_CLOCK | _74HC595_ROW_OUTPUT_ENABLE | _74HC595_ROW_DATA));
	
	if (bBit)
	{
		PORTB |= _74HC595_ROW_DATA;
	}
	else
	{
		PORTB &= ~_74HC595_ROW_DATA;
	}
	PORTB |= _74HC595_ROW_SHIFT_CLOCK;
	PORTB &= ~_74HC595_ROW_SHIFT_CLOCK;	
	
	// 転送する？
	if ( bTrans )
	{
		_74HC595_ROW_TransferData();
	}
}


ISR( TIMER0_COMPA_vect )
{
	uint32_t			temp = 0x00000000;
	uint32_t			mask = 0;
	int					i = 0;
	bool				bRet = false;

	_74HC595_COL_SetData( 0x00000000, true);

	mask = 0x00010000UL << g_MatorixLedCol;
	for ( i = 0 ; i < 16 ; i++ )
	{
		bRet = g_MatorixLed[ 15-i ] & mask;
		if ( g_bRevers == true ) bRet = (bRet == true) ? false : true;
				
		if ( bRet )
		{
			_74HC595_COL_Shift( true , false );
		}
		else
		{
			_74HC595_COL_Shift( false , false );
		}
	}
	mask = 0x00000001UL << g_MatorixLedCol;
	for ( i = 0 ; i < 16 ; i++ )
	{
		bRet = g_MatorixLed[ 15-i ] & mask;
		if ( g_bRevers == true ) bRet = (bRet == true) ? false : true;
		if ( bRet )
		{
			_74HC595_COL_Shift( true , false );
		}
		else
		{
			_74HC595_COL_Shift( false , false );
		}
	}

	if ( g_MatorixLedCol == 0 )
	{
		_74HC595_ROW_SetData( 0x00010001, true );		
	}
	else
	{
		_74HC595_ROW_Shift( false, true );		
	}
	_74HC595_COL_TransferData();
	
	g_MatorixLedCol++;
	if ( g_MatorixLedCol >= 16 )
	{
		g_MatorixLedCol=0;
	}
	
	g_Count++;
	if ( g_MoveCount <= 100 )
	{
		if ( g_MoveCount <= g_Count )
		{
			for ( int i = 0 ; i < 16 ; i++ )
			{
				temp = g_MatorixLed[ i ];
				g_MatorixLed[ i ] = g_MatorixLed[ i ] << 1;
				if ( temp & 0x80000000UL )
				{
					g_MatorixLed[ i ] |= 0x00000001UL;
				}
			}
			g_Count = 0;
		}
	}	

	return 0;
}

ISR( ADC_vect )
{
	uint16_t    result = 0;             // A/D変換結果


	result = ADC;
	g_MoveCount =(uint16_t)( result / 10 );
	ADCSRA = 0b11001111;            // A/D許可, A/D変換開始, A/D変換完了割り込み許可
}


int main(void)
{
	uint32_t		wait = 0;			// チャタリング防止
	
	ADMUX  = 0b00000000;                // 基準電圧：外部基準電圧,ADC0
    DIDR0  = 0b11111110;                // デジタル入力禁止:ADC7～ADC1


	DDRA  = 0b00111110;
	DDRB  = 0b00011111;
	DDRC  = 0b00000000;
	PORTC = 0b00000001;
	
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000101;
	TIMSK0 = 0b00000010;
	OCR0A  = 20;
	
	// 74HC595初期化
	_74HC595_COL_Init();
	_74HC595_ROW_Init();
	
	// 74HC595リセット
	_74HC595_COL_Reset();
	_74HC595_ROW_Reset();
	
    ADCSRA = 0b11001111;            // A/D許可, A/D変換開始, A/D変換完了割り込み許可
	sei();
	
	while(1)
    {
		if ( ( bit_is_clear( PINC, PINC0 ) == true ) && (wait >= 100000) )
		{
			g_bRevers = (g_bRevers == false) ? true : false;
			wait = 0;
		}
		
		if ( wait < 0xFFFFFFFF ) wait++;
		
    }
	
	return 0;
}