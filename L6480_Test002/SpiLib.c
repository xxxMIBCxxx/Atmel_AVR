#include "SpiLib.h"


void SPI_SetClockRate( SPI_CLOCK_RATE_ENUM eSpiClockRate )
{
	// ������ݒ�
	switch ( eSpiClockRate ){	
	case SPI_CLOCK_RATE4:
		SPCR = SPCR & (~(_BV(SPR1) | _BV(SPR0)));
		SPSR = SPSR & (~(_BV(SPI2X)));
		break;
		
	case SPI_CLOCK_RATE8:
		SPCR = (SPCR & (~(_BV(SPR1)))) | _BV(SPR0);
		SPSR |= _BV(SPI2X);
		break;
		
	case SPI_CLOCK_RATE16:
		SPCR = (SPCR & (~(_BV(SPR1)))) | _BV(SPR0);
		SPSR = SPSR & (~(_BV(SPI2X)));
		break;

	case SPI_CLOCK_RATE32:
		SPCR = (SPCR & (~(_BV(SPR0)))) | _BV(SPR1);
		SPSR |= _BV(SPI2X);
		break;
		
	case SPI_CLOCK_RATE64:
		SPCR = (SPCR & (~(_BV(SPR0)))) | _BV(SPR1);
		SPSR = SPSR & (~(_BV(SPI2X)));
		break;
		
	case SPI_CLOCK_RATE128:
		SPCR |= (_BV(SPR0) | _BV(SPR1));
		SPSR = SPSR & (~(_BV(SPI2X)));
		break;
		
	case SPI_CLOCK_RATE2:
	default:
		SPCR = SPCR & (~(_BV(SPR1) | _BV(SPR0)));
		SPSR |= _BV(SPI2X);
		break;
	}		
}



//-------------------------------------------------------------------------------------------------
// SPI�������i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterInit(SPI_CLOCK_RATE_ENUM eSpiClockRate)
{

	
	// ���g���ݒ�
	SPI_SetClockRate( eSpiClockRate );

	return;
}
 

//-------------------------------------------------------------------------------------------------
// SPI�f�[�^���M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterSend( unsigned char *pSendData, uint16_t iDataLen ) 
{
	
	for ( uint16_t i = 0 ; i < iDataLen ; i++ )
	{
		// ���M
		SPI_MasterTransmit( pSendData[i] );
	}	
		
	return;
}
 
 
//-------------------------------------------------------------------------------------------------
// SPI�f�[�^��M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
void SPI_MasterRecv( unsigned char *pRecvData, uint16_t iDataLen ) 
{
	
	for ( uint16_t i = 0 ; i < iDataLen ; i++ )
	{
		// �_�~�[�f�[�^�𑗐M���āA�f�[�^����M����
		pRecvData[i] = SPI_MasterTransmit( 0xFF );
	}	
		
	return;
}
 
 
//-------------------------------------------------------------------------------------------------
// SPI�f�[�^���M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
char SPI_MasterTransmit( unsigned char Data )
{
    SPDR = Data;
     
    // �f�[�^���M��������܂ő҂�
    while( !(SPSR & _BV(SPIF)) );
     
    return SPDR;
}
 


