#include "SpiLib.h"


void SPI_SetClockRate( SPI_CLOCK_RATE_ENUM eSpiClockRate )
{
	// 分周を設定
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
// SPI初期化（主装置）
//-------------------------------------------------------------------------------------------------
void SPI_MasterInit(SPI_CLOCK_RATE_ENUM eSpiClockRate)
{

	
	// 周波数設定
	SPI_SetClockRate( eSpiClockRate );

	return;
}
 

//-------------------------------------------------------------------------------------------------
// SPIデータ送信（主装置）
//-------------------------------------------------------------------------------------------------
void SPI_MasterSend( unsigned char *pSendData, uint16_t iDataLen ) 
{
	
	for ( uint16_t i = 0 ; i < iDataLen ; i++ )
	{
		// 送信
		SPI_MasterTransmit( pSendData[i] );
	}	
		
	return;
}
 
 
//-------------------------------------------------------------------------------------------------
// SPIデータ受信（主装置）
//-------------------------------------------------------------------------------------------------
void SPI_MasterRecv( unsigned char *pRecvData, uint16_t iDataLen ) 
{
	
	for ( uint16_t i = 0 ; i < iDataLen ; i++ )
	{
		// ダミーデータを送信して、データを受信する
		pRecvData[i] = SPI_MasterTransmit( 0xFF );
	}	
		
	return;
}
 
 
//-------------------------------------------------------------------------------------------------
// SPIデータ送信（主装置）
//-------------------------------------------------------------------------------------------------
char SPI_MasterTransmit( unsigned char Data )
{
    SPDR = Data;
     
    // データ送信完了するまで待つ
    while( !(SPSR & _BV(SPIF)) );
     
    return SPDR;
}
 


