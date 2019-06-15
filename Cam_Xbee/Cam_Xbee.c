#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include "avr/interrupt.h"
#include "util/delay.h"
#include "AL422_Lib.h"
#include "Sensor.h"
#include "Sensor_config.h"
#include "XBeeLib.h"


#define CAMERA_DATA_MAX		( 1000 )		
#define	LED_GREEN			_BV(PINC7)
#define	LED_RED				_BV(PINC6)


volatile bool			bWriteFlag = true;


//---------------------------------------------------------
// VSYNC������
//---------------------------------------------------------
ISR( PCINT0_vect )
{
	// �����݋��H(�f�[�^�Ǎ����łȂ��ꍇ�j
	if ( bWriteFlag == true )
	{
		PORTA = PORTA | AL422_WEN;	
	}
	else
	{
		PORTA = PORTA &~(AL422_WEN);	
	}
}




int main(void)
{
	bool					bRet = false;
	uint32_t				i = 0;
	char					szData[ 12 + CAMERA_DATA_MAX ];
	uint32_t				iCnt = 0;	
	XBEE_API_FRAME_TABLE	tXBeeApiFrame;
	
	
	// �����݂�L���ɂ���ݒ�
	PCICR = 0b00000001;
	PCMSK0= 0b00000001;

	DDRC  = 0b11000000;
//	PORTC = 0b11000000;

	memset( &tXBeeApiFrame, 0x00, sizeof(XBEE_API_FRAME_TABLE) );
	tXBeeApiFrame.StartDelimiter = 0x7E;
	
	memset( szData, 0x00, sizeof(szData) );
	szData[ 0 ] = 0x20;
	szData[ 1 ] = 0x00;
	szData[ 2 ] = 192;
	szData[ 3 ] = 168;
	szData[ 4 ] = 33;
	szData[ 5 ] = 5;
	szData[ 6 ] = 0x26;
	szData[ 7 ] = 0x16;
	szData[ 8 ] = 0x26;
	szData[ 9 ] = 0x16;
	szData[ 10 ] = 0x00;
	szData[ 11 ] = 0x00;

	// XBee������
	bRet = XBee_Init();
	if ( bRet == false )
	{
		PORTC |= LED_RED;
		return -1; 	
	}
	
	// XBee Wi-Fi�ڑ��҂�
	bRet = XBee_JoinedWifi();
	if ( bRet == false )
	{
		PORTC |= LED_RED;
		return -1; 	
	}
	PORTC |= LED_GREEN;

	// �J�����Z���T�[������
	AL422_Init();
	bRet = Sensor_Init();
	if ( bRet == true )
	{
		sei();
		while ( 1 )
		{
			if ( bit_is_clear(PINB,PINB0)==true )
			{
				PORTC &= ~(LED_GREEN);
				bWriteFlag = false;					// �����݋֎~�ɂ���
				AL422_ReadReset();

				
				memset( &szData[12], 0x00, CAMERA_DATA_MAX );
				sprintf( &szData[12], "@V7670_IMAGE:" );
				szData[ 12 + strlen("@V7670_IMAGE:") ] = 1;
				tXBeeApiFrame.wLength = 12 + strlen("@V7670_IMAGE:") + 1;
				tXBeeApiFrame.pszData = &szData[0];
				tXBeeApiFrame.CheckSum = XBee_CreateCheckSum( szData, tXBeeApiFrame.wLength );
				XBee_SendApiFrame( &tXBeeApiFrame, NULL );


				iCnt = 0;
				memset( &szData[12], 0x00, CAMERA_DATA_MAX );
				for ( i = 0 ; i < (76800 * 2) ; i++ )		// 320 x 240
//				for ( i = 0 ; (i < 101376 *2) ; i++ )
				{
					szData[12 + iCnt] = AL422_ReadEnable();
					iCnt = iCnt + 1;
					if ( iCnt >= CAMERA_DATA_MAX )
					{
						tXBeeApiFrame.wLength = 12 + iCnt;
						tXBeeApiFrame.pszData = &szData[0];
						tXBeeApiFrame.CheckSum = XBee_CreateCheckSum( szData, tXBeeApiFrame.wLength );
						XBee_SendApiFrame( &tXBeeApiFrame, NULL );
						iCnt = 0;
						memset( &szData[12], 0x00, CAMERA_DATA_MAX );
					}
				}

				if ( iCnt != 0 )
				{
					tXBeeApiFrame.wLength = 12 + iCnt;
					tXBeeApiFrame.pszData = &szData[0];
					tXBeeApiFrame.CheckSum = XBee_CreateCheckSum( szData, tXBeeApiFrame.wLength );
					XBee_SendApiFrame( &tXBeeApiFrame, NULL );
				}
				
				bWriteFlag = true;					// �����݋����� 
				PORTC |= LED_GREEN;
			}						
		}
	}
	else
	{
		PORTC |= LED_RED;
		return -1;
	}
	
	return 0;	
}