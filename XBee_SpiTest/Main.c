/*
 * Main.c
 *
 * Created: 2012/08/19 0:23:37
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "XBeeLib.h"


int main(void)
{
	bool						bRet = false;
	XBEE_API_FRAME_TABLE		tXBeeApiFrame;
	XBEE_API_FRAME_TABLE		tXBeeResApiFrame;
	char						Data[] = { 0x08, 0x01, 'N', 'I' };
	
	
	
	
	// XBee������
	bRet = XBee_Init();
	
	if ( bRet == true )
	{
		
		tXBeeApiFrame.StartDelimiter = 0x7E;
		tXBeeApiFrame.wLength = sizeof(Data) / sizeof(char);
		tXBeeApiFrame.pszData = Data;
		tXBeeApiFrame.CheckSum = XBee_CreateCheckSum( tXBeeApiFrame.pszData, tXBeeApiFrame.wLength );
	
		bRet = XBee_SendApiFrame( &tXBeeApiFrame, &tXBeeResApiFrame );
		if ( bRet == true )
		{
			// �����������ɓ����Ă����Ƃ���ŁAtXBeeResApiFrame�̓��e���m�F
			free( tXBeeResApiFrame.pszData );
		}
	
		while(1);
	}
			
	return 0;
}