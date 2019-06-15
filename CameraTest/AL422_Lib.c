#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "AL422_Lib.h"

//#define ENABLE_LAN



#ifdef ENABLE_LAN

#define AL422_DO0			_BV(PIND0)			// Data Output 0
#define AL422_DO1			_BV(PIND1)			// Data Output 1
#define AL422_DO2			_BV(PIND2)			// Data Output 2
#define AL422_DO3			_BV(PIND3)			// Data Output 3
#define AL422_DO4			_BV(PIND4)			// Data Output 4
#define AL422_DO5			_BV(PIND5)			// Data Output 5
#define AL422_DO6			_BV(PIND6)			// Data Output 6
#define AL422_DO7			_BV(PIND7)			// Data Output 7

#else

#define AL422_DO0			_BV(PINB0)			// Data Output 0
#define AL422_DO1			_BV(PINB1)			// Data Output 1
#define AL422_DO2			_BV(PINB2)			// Data Output 2
#define AL422_DO3			_BV(PINB3)			// Data Output 3
#define AL422_DO4			_BV(PINB4)			// Data Output 4
#define AL422_DO5			_BV(PINB5)			// Data Output 5
#define AL422_DO6			_BV(PINB6)			// Data Output 6
#define AL422_DO7			_BV(PINB7)			// Data Output 7

#endif	// #ifdef ENABLE_LAN



#define AL422_OE			_BV(PINA6)			// Output enable
#define AL422_RCK			_BV(PINA5)			// Read clock
#define AL422_NC			_BV(PINA4)
#define AL422_RRST			_BV(PINA3)			// Read Reset(actve low)
#define HREF				_BV(PINA2)
#define AL422_WEN			_BV(PINA1)			// Write Enable(activ low)
#define VSYNC				_BV(PINA0)


//-------------------------------------------------------------------------------------------------
// AL422 èâä˙èàóù
//-------------------------------------------------------------------------------------------------
void AL422_Init(void)
{
	DDRA  = AL422_RCK | AL422_WEN | AL422_RRST | AL422_OE;
	DDRC  = 0b00000000;
	PORTC = 0b11111111;
	
	PORTA = (PORTA &~(AL422_RCK)) | AL422_RRST | VSYNC | HREF | AL422_OE;
}


//-------------------------------------------------------------------------------------------------
// AL422 ReadÉäÉZÉbÉg
//-------------------------------------------------------------------------------------------------
void AL422_ReadReset(void)
{
	//PORTA = PORTA & ~(AL422_RRST | AL422_OE);
	//_delay_us( 10 );
	//PORTA = PORTA & ~(AL422_RCK);
	//_delay_us( 40 );
	//PORTA = PORTA | AL422_RCK;
	//_delay_us( 50 );
	//PORTA = PORTA & ~(AL422_RCK);
	//_delay_us( 50 );
	//PORTA = PORTA | AL422_RCK;
	//_delay_us( 50 );
	//PORTA = PORTA & ~(AL422_RCK);
	//_delay_us( 10 );
	//PORTA = PORTA | AL422_RRST;
	//_delay_us( 40 );
	//PORTA = PORTA | AL422_RCK;
	//_delay_us( 50 );
	
	PORTA = (PORTA & ~(AL422_RRST)) | AL422_OE;
	PORTA = PORTA & ~(AL422_RCK);
	PORTA = PORTA | AL422_RCK;
	PORTA = PORTA & ~(AL422_RCK);
	PORTA = PORTA | AL422_RRST;
	PORTA = (PORTA & ~(AL422_OE)) | AL422_RCK ;
	
	
}



unsigned char AL422_ReadEnable( void )
{
	unsigned char Data = 0x00;
	
//	_delay_us( 40 );
//	PORTA = PORTA & ~(AL422_RCK | AL422_OE);
	PORTA = PORTA & ~(AL422_RCK);

#ifdef ENABLE_LAN
	Data = PIND;
#else
	Data = PINB;
#endif	// #ifdef ENABLE_LAN


	PORTA = PORTA | AL422_RCK;
//	_delay_us( 50 );
	
	return Data;
}


void AL422_OutuptEnable( void )
{
	PORTA = PORTA | AL422_OE;
	PORTA = PORTA | AL422_RCK;
	PORTA = PORTA & ~(AL422_RCK);
	PORTA = PORTA | AL422_RCK;
	PORTA = PORTA & ~(AL422_OE);
	PORTA = PORTA & ~(AL422_RCK);
}


void AL422_WriteReset( void )
{

#if 0
	//PORTA = PORTA & ~(AL422_WEN);
	//while( bit_is_set(PINA,PINA0));
	PORTA = PORTA & ~(AL422_WEN);
	while( bit_is_clear(PINA,PINA0));
	PORTA = PORTA | AL422_WEN;	
	while( bit_is_set(PINA,PINA0));
	PORTA = PORTA & ~(AL422_WEN);
	//while( bit_is_clear(PINA,PINA0));
	//PORTA = PORTA & ~(AL422_WEN);
//
#endif

	PORTA = PORTA | AL422_WEN;	
//	while( bit_is_clear(PINA,PINA0));
	PORTA = PORTA & ~(AL422_WEN);
//	while( bit_is_set(PINA,PINA0));
	PORTA = PORTA | AL422_WEN;	


}

