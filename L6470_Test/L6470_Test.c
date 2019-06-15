/*
 * L6470_Test.c
 *
 * Created: 2013/05/29 21:24:35
 *  Author: MIBC
 */ 


#define F_CPU	( 20000000UL )
#include <avr/io.h>
#include "util/delay.h"
#include "SpiLib.h"
#include "L6470Lib.h"

int main(void)
{
	uint32_t temp = 0;
	
	DDRB = (SPI_SCK | SPI_MOSI | L6470_CS | L6470_STBY );
	PORTB = L6470_CS;

	 // SPI‹–‰Â,Žå‘•’u
	_delay_ms(100);
    SPCR = _BV(SPE) | _BV(MSTR);
	
	L6470_Init();
	
	
	int32_t iMaxSpeed = 0;
	bool bRet = L6470_GetParam( L6470_MAX_SPEED, &iMaxSpeed );
	if ( bRet == true )
	{
		return iMaxSpeed;
	}	
		
	
	

	
	//L6470_SetParam( L6470_RUN, 0xFFFFF);
	//_delay_ms( 200000 );
	//L6470_SetParam( L6470_STOP, 0x0000 );
	
	
    while(1)
    {
        //TODO:: Please write your application code 

    }
}