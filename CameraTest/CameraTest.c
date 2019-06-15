/*
 * CameraTest.c
 *
 * Created: 2012/07/22 19:37:23
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "avr/interrupt.h"
#include "util/delay.h"
#include "Sensor.h"
#include "Sensor_config.h"
#include <alloca.h>


#include "string.h"
#include "W5200Lib.h"



//#define ENABLE_LAN


#define UDP_SOCKET				( 0 )
#define UDP_PORT_NO				( 9750 )


uint8		MAC_ADDRESS[8] = { 0xEB, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
uint8		SUBNET_MASK[4] = { 255, 255, 255,   0 };
uint8		IP_ADDRESS[4]  = { 192, 168,  33,  20 };


volatile bool			bWriteFlag = true;



#define AL422_OE			_BV(PINA6)			// Output enable
#define AL422_RCK			_BV(PINA5)			// Read clock
#define AL422_NC			_BV(PINA4)
#define AL422_RRST			_BV(PINA3)			// Read Reset(actve low)
#define HREF				_BV(PINA2)
#define AL422_WEN			_BV(PINA1)			// Write Enable(activ low)
#define VSYNC				_BV(PINA0)



#ifndef ENABLE_LAN
//#define BAUD		( 115200L )				// 目的USARTﾎﾞｰﾚｰﾄ速度
//#define	MYUBRR		F_CPU / 16 / BAUD +1		// 目的UBRRn値
#define BAUD		( 115200UL )				// 目的USARTﾎﾞｰﾚｰﾄ速度
#define	MYUBRR		(10)




//---------------------------------------------------------
// USART初期化処理
//---------------------------------------------------------
void USART_Init( void )
{
	DDRD  = 0b01111100;
	PORTD = 0b10000000;


	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char)(MYUBRR);

	UCSR0A = 0b00000000;
	UCSR0B = 0b00011000;					// 受信許可・送信許可
	UCSR0C = 0b00000110;					// 非同期・パリティなし・停止ビット：1bit・データビット：8bit

	return;
}
//---------------------------------------------------------
// 受信処理
//---------------------------------------------------------
unsigned char USART_Receive(void)
{
	// 受信完了待ち
	while ( !(UCSR0A & 0b10000000) );

	return UDR0;
}

//---------------------------------------------------------
// 送信処理
//---------------------------------------------------------
void USART_Transmit( unsigned char Data)
{
	// 送信バッファが空きになるまで待つ
	while ( !(UCSR0A & 0b00100000) );

	UDR0 = Data;
}
#endif	// #ifndef ENABLE_LAN



//---------------------------------------------------------
// VSYNC割込み
//---------------------------------------------------------
ISR( PCINT0_vect )
{
//	// 書込み位置は一番最初に戻す
//	AL422_WriteReset();

	// 書込み許可？(データ読込中でない場合）
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
	char			szBuff[ 20 ];
	bool			bRet = false;
	unsigned char	Data;
	uint32_t		i = 0;
	uint32_t		cnt = 0;
	uint32_t		j  = 0;
	UDP_DATA_FORMAT_TABLE		tUdpData;

	
	//// 割込みを有効にする設定
	PCICR = 0b00000001;
	PCMSK0= 0b00000001;
	
	
	uint16_t	aec = 0;

#ifndef ENABLE_LAN
	USART_Init();
#endif //#ifndef ENABLE_LAN
	

	AL422_Init();
	bRet = Sensor_Init();


#ifdef ENABLE_LAN
	// W5200初期化
	W5200_Init();
#endif	// #ifdef ENABLE_LAN	

	
	
	
	
#ifdef ENABLE_LAN	
	//PORTB = PORTB | _BV(PINB2);
	
	// MACアドレス設定
	W5200_setSHAR( MAC_ADDRESS );
		
	// ※DHCPから取得する場合は以下の処理を有効にしてください※
#if 0
	// サブネットマスク・IPアドレスをDHCPから取得
	memset( &tUdpData, 0x00, sizeof(UDP_DATA_FORMAT_TABLE));
	memset( &tDhcpInfo, 0x00, sizeof(DHCP_INFO_TABLE));
	bRet = W5200_DHCP_Client( MAC_ADDRESS, &tUdpData, &tDhcpInfo );
	if ( bRet == true )
	{
		memcpy( SUBNET_MASK, tDhcpInfo.Subnet, sizeof(uint8) * 4);
		memcpy( IP_ADDRESS , tDhcpInfo.IP, sizeof(uint8) * 4);
	}
#endif

	// サブネットマスク設定
	W5200_setSUB( SUBNET_MASK );
	
	// IPアドレス設定
	W5200_setSIPR( IP_ADDRESS );

	memset( &tUdpData, 0x00, sizeof(UDP_DATA_FORMAT_TABLE));
	tUdpData.IP[0] = 192; tUdpData.IP[1] = 168; tUdpData.IP[2] = 33; tUdpData.IP[3] = 5;	// 送信先のIPアドレス
	tUdpData.Port = 9750;																	// 送信先のポート番号

	bRet = W5200_UDP_OPEN( UDP_SOCKET, UDP_PORT_NO );
#endif	// #ifdef ENABLE_LAN		


	if ( bRet == true )
	{
		sei();
		while ( 1 )
		{			
			if ( bit_is_clear(PIND,PIND7)==true )
			{
////				cli();
				bWriteFlag = false;					// 書込み禁止にする
				AL422_ReadReset();
				
#ifdef ENABLE_LAN					
				sprintf( tUdpData.DataBuff, "@V7670_IMAGE:" );
					tUdpData.DataBuff[ strlen(tUdpData.DataBuff) ] = 0x01;
					tUdpData.DataSize = strlen(tUdpData.DataBuff) + 1;
					W5200_UDP_Send( UDP_SOCKET, &tUdpData );
				
				cnt = 0;
				for ( i = 0 ; i < (76800 * 2) ; i++ )
				{
					tUdpData.DataBuff[ cnt ] = AL422_ReadEnable();
					cnt = cnt + 1;
					if ( cnt >= 1000 )
					{
						tUdpData.DataSize = cnt;
						W5200_UDP_Send( UDP_SOCKET,  &tUdpData );
						cnt = 0;
					}
				}
				
				if ( cnt != 0 )
				{
					tUdpData.DataSize = cnt;
					W5200_UDP_Send( UDP_SOCKET, &tUdpData );
					cnt = 0;
				}
#else				
				for ( i = 0 ; i < 76800 ; i++ )		// 320 x 240
				{
					Data = AL422_ReadEnable();
					USART_Transmit(Data);
 					
					Data = AL422_ReadEnable();
					USART_Transmit(Data);
				}
#endif

				bWriteFlag = true;					// 書込み許可する 
//				sei();
			}				
		}	
	}			

	return 0;
}

