#ifndef SPILIB_H_
#define SPILIB_H_

#include <avr/io.h>

//#define VS053_CS		_BV(PINB3)
#define SD_CS			_BV(PIND0)

//===[ SPI PIN ]===============================================================
#define SPI_MOSI        _BV(PINB5)
#define SPI_MISO        _BV(PINB6)
#define SPI_SCK         _BV(PINB7)
//=============================================================================


//-----------------------------------------------
// SPI�N���b�N���
//-----------------------------------------------
typedef enum
{
	SPI_CLOCK_RATE2 = 0,		// fOSC/2
	SPI_CLOCK_RATE4,			// fOSC/4
	SPI_CLOCK_RATE8,			// fOSC/8
	SPI_CLOCK_RATE16,			// fOSC/16
	SPI_CLOCK_RATE32,			// fOSC/32
	SPI_CLOCK_RATE64,			// fOSC/64
	SPI_CLOCK_RATE128,			// fOSC/128
} SPI_CLOCK_RATE_ENUM;

void SPI_SetClockRate( SPI_CLOCK_RATE_ENUM eSpiClockRate );

//-------------------------------------------------------------------------------------------------
// SPI�������i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterInit(SPI_CLOCK_RATE_ENUM eSpiClockRate);

//-------------------------------------------------------------------------------------------------
// SPI�f�[�^���M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterSend( unsigned char *pSendData, uint16_t iDataLen );
 
//-------------------------------------------------------------------------------------------------
// SPI�f�[�^��M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterRecv( unsigned char *pRecvData, uint16_t iDataLen );

//-------------------------------------------------------------------------------------------------
// SPI�f�[�^���M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
char SPI_MasterTransmit( unsigned char Data );

#endif /* SPILIB_H_ */