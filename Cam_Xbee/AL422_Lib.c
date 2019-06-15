#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "AL422_Lib.h"


//-------------------------------------------------------------------------------------------------
// AL422 èâä˙èàóù
//-------------------------------------------------------------------------------------------------
void AL422_Init(void)
{
	DDRA  = AL422_RCK | AL422_WEN | AL422_RRST | AL422_OE;
	PORTA = (PORTA &~(AL422_RCK)) | AL422_RRST | VSYNC | HREF | AL422_OE;
}


//-------------------------------------------------------------------------------------------------
// AL422 ReadÉäÉZÉbÉg
//-------------------------------------------------------------------------------------------------
void AL422_ReadReset(void)
{
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
	
	PORTA = PORTA & ~(AL422_RCK);
	Data = PIND;
	PORTA = PORTA | AL422_RCK;
	
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
	PORTA = PORTA | AL422_WEN;	
	PORTA = PORTA & ~(AL422_WEN);
	PORTA = PORTA | AL422_WEN;	
}

