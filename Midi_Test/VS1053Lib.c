#include <avr/io.h>
#include "util/delay.h"
#include "SpiLib.h"
#include "VS1053Lib.h"


void VS1053_SCI_Enable()
{
	VS1053_CS_HI();
	VS1053_DCS_HI();
	VS1053_CS_LOW();
}

void VS1053_SCI_Disable()
{
	VS1053_CS_HI();
}

void VS1053_SDI_Enable()
{
	VS1053_DCS_HI();
	VS1053_CS_HI();
	VS1053_DCS_LOW();
}

void VS1053_SDI_Disable()
{
	VS1053_DCS_HI();
}

void VS1053_Reset()
{
	VS1053_RESET_LOW();
	_delay_ms(10);
	VS1053_RESET_HI();
	_delay_ms(10);
	//while ( !VS1053_DREQ() );
	VS1053_RESET_HI();
	
	VS1053_SCI_Write( 0x00, SM_SDINEW | SM_RESET );
	_delay_ms(5);
	
	VS1053_SCI_Write( 0x03, (0xa000 | 0x0800) );
	_delay_ms(100);
	
}



void VS1053_Init()
{
	SPI_MasterInit( SPI_CLOCK_RATE2 );
}


void VS1053_SCI_Init()
{
	VS1053_SCI_Enable();
	
	SPI_SetClockRate( SPI_CLOCK_RATE8 );
	for ( uint16_t i = 0 ; i < 4 ; i++ )
	{
		SPI_MasterTransmit(0xFF);
	}
		
	VS1053_SCI_Disable();
	_delay_us(10);
}
	
void VS1053_SDI_Init()
{
	VS1053_SDI_Enable();
	
	SPI_SetClockRate( SPI_CLOCK_RATE8 );
	
	VS1053_SDI_Disable();
}


void VS1053_SCI_Write( unsigned char Address, uint16_t wData )
{
	VS1053_SCI_Enable();
	
	while ( !VS1053_DREQ() );
	SPI_MasterTransmit(0x02);
	SPI_MasterTransmit(Address);
	SPI_MasterTransmit((wData >> 8) & 0xFF);
	SPI_MasterTransmit((wData & 0xFF));
	
	VS1053_SCI_Disable();
}


uint16_t VS1053_SCI_Read( unsigned char Address )
{
	uint16_t wRecvData = 0x0000;
	
	VS1053_SCI_Enable();
	
	while ( !VS1053_DREQ() );
	SPI_MasterTransmit(0x03);
	SPI_MasterTransmit(Address);
	wRecvData = SPI_MasterTransmit(0x00);
	wRecvData = (wRecvData << 8);
	wRecvData += SPI_MasterTransmit(0x00);

	VS1053_SCI_Disable();
		
	return wRecvData;
}


void VS1053_SDI_Write( unsigned char datum )
{
	VS1053_SDI_Enable();
	
	//while ( !VS1053_DREQ() );
	SPI_MasterTransmit( datum );
	
	VS1053_SDI_Disable();
}



void VS1053_Volume( int16_t iLeft, int16_t iRight )
{
	uint16_t _left = -iLeft;
	uint16_t _right = -iRight;
	_left *= 2;
	_right *= 2;
	uint16_t attenuation = ((256 * _left) + _right);
	
	VS1053_SCI_Write( 0x0B, attenuation );
}

void sine_test_activate(unsigned char wave)
{
	VS1053_SCI_Write( 0x00, 0x0820 );
	
    VS1053_SDI_Enable();									 //enables SDI/disables SCI
	
    while ( !VS1053_DREQ() );						 //wait unitl data request is high
    SPI_MasterTransmit(0x53);                        //SDI write
    SPI_MasterTransmit(0xEF);                        //SDI write
    SPI_MasterTransmit(0x6E);                        //SDI write
    SPI_MasterTransmit(wave);                        //SDI write
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
	
    VS1053_SDI_Disable();										 //enables SCI/disables SDI
}


void sine_test_deactivate(void)
{
	VS1053_SCI_Write( 0x00, 0x0820 );
    
	
    VS1053_SDI_Enable();
	
    while ( !VS1053_DREQ() );
    SPI_MasterTransmit(0x45);                        //SDI write
    SPI_MasterTransmit(0x78);                        //SDI write
    SPI_MasterTransmit(0x69);                        //SDI write
    SPI_MasterTransmit(0x74);                        //SDI write
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
	
	VS1053_SDI_Disable();
}


void memory_test_()
{
	VS1053_SCI_Write( 0x00, 0x0820 );
	
    VS1053_SDI_Enable();
	
	
	SPI_MasterTransmit(0x4D);                        //SDI write
    SPI_MasterTransmit(0xEA);                        //SDI write
    SPI_MasterTransmit(0x6D);                        //SDI write
    SPI_MasterTransmit(0x54);                        //SDI write
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte
    SPI_MasterTransmit(0x00);                        //filler byte	
	
	_delay_ms(10000);
	
	uint16_t HDAT0 = VS1053_SCI_Read( 0x08 );
	uint16_t HDAT1 = VS1053_SCI_Read( 0x09 );
	
	
}
