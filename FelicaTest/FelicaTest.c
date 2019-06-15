/*
 * FelicaTest.c
 *
 * Created: 2012/07/07 19:53:11
 *  Author: MIBC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>


#define BAUD								( 115200UL )				// ボーレート速度
#define RCS620S_MAX_CARD_RESPONSE_LEN		( 254 )
#define RCS620S_MAX_RW_RESPONSE_LEN			( 265 )
#define DATA_MAX_BUFF						( 400 )


typedef struct 
{
	uint16_t			wIndex;						// 受信バッファインデックス
	uint16_t			wRcveIndex;					// 受信要求のポジション
	uint16_t			wDataLen;					// 
	unsigned char		szData[ DATA_MAX_BUFF ];

} USART_DATA_TABLE, *PUSART_DATA_TABLE;


volatile USART_DATA_TABLE		g_tUsartRecvData;
volatile unsigned char			g_szResponse[ RCS620S_MAX_RW_RESPONSE_LEN ];
volatile uint16_t				g_wResponseLen;
volatile unsigned char			g_szIdm[ 8 ];
volatile unsigned char			g_szPmm[ 8 ];
volatile unsigned char			g_szLcdBuff[ 20 + 1 ];


void init_USART(void);
bool USART_Receive( unsigned char *pData, uint16_t wDataLen );
void USART_Transmit( unsigned char *pData, uint16_t wDataLen );
unsigned char RCS620S_calcDCS( const unsigned char *pData, uint16_t wDataLen );
bool RCS620S_RfOff(void);
void RCS620S_Cancel(void);
bool RCS620S_Command( unsigned char *pCommand, uint16_t wCommandLen, unsigned char *pResponse, uint16_t *pwResponseLen );
bool init_RCS620S(void);
bool RCS620S_Polling( uint16_t wSystemCode );


//=================================================================================================
// USART受信割込み処理
//=================================================================================================
ISR(USART0_RX_vect)
{
	// 受信完了待ち
	while ( !(UCSR0A & _BV(RXC0)) )
	{
	}
	
	// フレームエラー・オーバーラン？
	if ( (UCSR0A & _BV(FE0)) || (UCSR0A & _BV(DOR0)) )
	{
		unsigned char		dummy = 0;
		dummy = UDR0;
		return;
	}

	g_tUsartRecvData.szData[ g_tUsartRecvData.wIndex ] = UDR0;
	g_tUsartRecvData.wIndex++;
	if ( g_tUsartRecvData.wIndex >= DATA_MAX_BUFF )
	{
		g_tUsartRecvData.wIndex = 0;
	}	
	g_tUsartRecvData.wDataLen++;
}



int main(void)
{
	DDRD = _BV(PIND6) | _BV(PIND5) | _BV(PIND4);
	
	// グローバル変数初期化
	memset( (void *)&g_tUsartRecvData, 0x00, sizeof(g_tUsartRecvData));
	
	PORTD |= _BV(PIND6) | _BV(PIND5) | _BV(PIND4);
	
	// LED初期化
	Lcd_Init();
	sprintf( g_szLcdBuff, "--- Felica Test ---");
	Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));
		
	// USART初期化
	init_USART();
	
	// 割込み許可
	sei();
		

	Lcd_MoveCur( 0, 1 );
	sprintf( g_szLcdBuff, "RC-S620S Initalize");
	Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));

	// RCS620S初期化
	init_RCS620S();
	PORTD &= ~(_BV(PIND6) | _BV(PIND4));
	PORTD |= _BV(PIND5);
	
	Lcd_MoveCur( 0, 1 );
	sprintf( g_szLcdBuff, "RC-S620S Polling..");
	Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));

    while(1)
    {
		if ( RCS620S_Polling( 0xFFFF ) == true )
		{
			PORTD &= ~(_BV(PIND6) | _BV(PIND5));
			PORTD |= _BV(PIND4);			

			Lcd_MoveCur( 0, 1 );
			sprintf( g_szLcdBuff, "RC-S620S Card Read");
			Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));

			Lcd_MoveCur( 0, 2 );
			sprintf( g_szLcdBuff, "IDm:%02X%02X%02X%02X%02X%02X%02X%02X", g_szIdm[0], g_szIdm[1], g_szIdm[2], g_szIdm[3], g_szIdm[4], g_szIdm[5], g_szIdm[6], g_szIdm[7]);
			Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));

			Lcd_MoveCur( 0, 3 );
			sprintf( g_szLcdBuff, "Pmm:%02X%02X%02X%02X%02X%02X%02X%02X", g_szPmm[0], g_szPmm[1], g_szPmm[2], g_szPmm[3], g_szPmm[4], g_szPmm[5], g_szPmm[6], g_szPmm[7]);
			Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));
			
			for ( int i = 0 ; i < 10 ; i++ )
			{
				_delay_ms( 500 );
				PIND = _BV( PIND4 );
			}
			Lcd_Clear();
			
			sprintf( g_szLcdBuff, "--- Felica Test ---");
			Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));
			
			PORTD &= ~(_BV(PIND6) | _BV(PIND4));
			PORTD |= _BV(PIND5);
	
			Lcd_MoveCur( 0, 1 );
			sprintf( g_szLcdBuff, "RC-S620S Polling..");
			Lcd_Write( g_szLcdBuff, strlen(g_szLcdBuff));
			
		}
    }
}


//=================================================================================================
// USART初期化処理
//=================================================================================================
void init_USART(void)
{
	uint32_t		MYUBRR = 0;
	
	
	// UBRRn算出
	MYUBRR = F_CPU / (16UL * BAUD);						// 本当は「MYUBRR = F_CPU / (16UL * BAUD) - 1」だけど動作しなかったので…
	UBRR0H = (unsigned char)(MYUBRR>>8);				// ボーレート設定（上位バイト）
	UBRR0L = (unsigned char) MYUBRR;					// ボーレート設定（下位バイト）
	
	// USART制御
//	UCSR0B = _BV(RXEN0) | _BV(TXEN0);					// 受信許可・送信許可
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);		// 受信完了割り込み許可・受信許可・送信許可
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);					// 非同期動作・パリティ：なし・ストップビット：1bit・データビット長：8bit
}


//=================================================================================================
// USART受信処理
//=================================================================================================
bool USART_Receive( unsigned char *pData, uint16_t wDataLen )
{
	bool				bRet = false;
	uint16_t			i = 0;
	uint16_t			RecvSize = 0;
	uint16_t			index = 0;
	
	// 2秒待つ
	for ( i = 0 ; i < 20 ; i++ )
	{
		// 受信要求データ数となっていた場合
		if ( g_tUsartRecvData.wDataLen >= wDataLen )
		{
//			// 割込み禁止
//			cli();
			
			if ( (g_tUsartRecvData.wRcveIndex + wDataLen) >= DATA_MAX_BUFF )
			{
				RecvSize = DATA_MAX_BUFF - g_tUsartRecvData.wRcveIndex;
				memcpy( &pData[ index ], (const void *)&g_tUsartRecvData.szData[ g_tUsartRecvData.wRcveIndex ], RecvSize );
				index = RecvSize;
				RecvSize = wDataLen - RecvSize;
				memcpy( &pData[ index ], (const void *)&g_tUsartRecvData.szData[ 0 ], RecvSize );
				g_tUsartRecvData.wRcveIndex = RecvSize;			
			}
			else
			{
				memcpy( &pData[ index ], (const void *)&g_tUsartRecvData.szData[ g_tUsartRecvData.wRcveIndex ], wDataLen );
				g_tUsartRecvData.wRcveIndex = g_tUsartRecvData.wRcveIndex + wDataLen;
			}
			g_tUsartRecvData.wDataLen = g_tUsartRecvData.wDataLen - wDataLen;
			
//			// 割込み許可
//			sei();
			
			bRet = true;
			break;
		}
		
		_delay_ms( 100 );
	}

	return bRet;
}
 
 
//=================================================================================================
// USART送信処理
//=================================================================================================
void USART_Transmit( unsigned char *pData, uint16_t wDataLen )
{
	uint16_t		i = 0;
	
	
	for ( i = 0 ; i < wDataLen ; i++ )
	{
		// 送信バッファが空きになるまで待つ
		while ( !(UCSR0A & _BV(UDRE0)) )
		{
		}		
		UDR0 = pData[ i ];
		_delay_us( 100 );
	}
}
 


//=================================================================================================
// DCS計算
//=================================================================================================
unsigned char RCS620S_calcDCS( const unsigned char *pData, uint16_t wDataLen )
{
	unsigned char		sum = 0x00;
	uint16_t			i = 0;
	
	
	// データを１バイトずつ足す
	for ( i = 0 ; i < wDataLen ; i++ )
	{
		sum += pData[i];
	}
	
	return (unsigned char)-(sum & 0xff);
}


//=================================================================================================
// RCS620Sのrf送信をoffにする
//=================================================================================================
bool RCS620S_RfOff(void)
{
	bool			bRet = false;
	
	
	bRet = RCS620S_Command( (unsigned char *)"\x00\x00\xff\x00\xff\x00", 6, g_szResponse, &g_wResponseLen );
	if ( (bRet == false) || (g_wResponseLen != 2) || (memcmp( (const void *)g_szResponse, (const void *)"\xd5\x33", 2) != 0) )
	{
		return false;
	}

    return true;		
}


//=================================================================================================
// RCS620Sにキャンセルコマンドを送信する
//=================================================================================================
void RCS620S_Cancel(void)
{
	USART_Transmit( (unsigned char *)"\x00\x00\xff\x00\xff\x00", 6 );
	_delay_ms( 10 );
}


//=================================================================================================
// RCS620Sにコマンドを送信
//=================================================================================================
bool RCS620S_Command( unsigned char *pCommand, uint16_t wCommandLen, unsigned char *pszResponse, uint16_t *pwResponseLen )
{
	bool					bRet = false;
	unsigned char			dcs = 0x00;
	unsigned char			szBuff[ 9 ];
	
	
	// DCS計算
	dcs = RCS620S_calcDCS( pCommand, wCommandLen );
	
	// コマンド送信
	szBuff[ 0 ] = 0x00;
	szBuff[ 1 ] = 0x00;
	szBuff[ 2 ] = 0xff;
	if ( wCommandLen <= 255 )
	{
		// ノーマルフレーム
		szBuff[ 3 ] = (unsigned char)wCommandLen;
		szBuff[ 4 ] = (unsigned char)-szBuff[ 3 ];
		USART_Transmit( szBuff, 5 );
	}
	else
	{
		// 拡張フレーム
		szBuff[ 3 ] = 0xff;
		szBuff[ 4 ] = 0xff;
		szBuff[ 5 ] = (unsigned char)((wCommandLen >> 8) & 0xff);
		szBuff[ 6 ] = (unsigned char)((wCommandLen >> 0) & 0xff);
		szBuff[ 7 ] = (unsigned char)-(szBuff[ 5 ] + szBuff[ 6 ]);
		USART_Transmit( szBuff, 8 );	
	}
	USART_Transmit( pCommand, wCommandLen );
	
	szBuff[ 0 ] = dcs;
	szBuff[ 1 ] = 0x00;
	USART_Transmit( szBuff, 2 );
//	_delay_ms( 100 );					// コマンドを投げてから少し待つ
	
	// ACKを受信
	bRet = USART_Receive( szBuff, 6 );
 	if ( (bRet == false) || (memcmp( szBuff,"\x00\x00\xff\x00\xff\x00", 6) != 0) )
	{
		RCS620S_Cancel();
		return false;
	}
	
	// 応答データサイズ取得
	bRet = USART_Receive( szBuff, 5 );
	if ( bRet == false )
	{
		RCS620S_Cancel();
		return false;	
	}
	else if ( memcmp(szBuff, "\x00\x00\xff", 3) != 0) 
	{
		return false;
	}
    if ( ( szBuff[3] == 0xff) && (szBuff[4] == 0xff) ) 
	{
		bRet = USART_Receive( szBuff + 5, 3 );
        if ( (bRet == false ) || (((szBuff[5] + szBuff[6] + szBuff[7]) & 0xff) != 0) ) 
		{
            return false;
        }
        *pwResponseLen = ( ((uint16_t)szBuff[5] << 8) | ((uint16_t)szBuff[6] << 0) );
    } 
	else
	{
        if ( ((szBuff[3] + szBuff[4]) & 0xff) != 0) 
		{
            return false;
        }
        *pwResponseLen = (uint16_t)szBuff[3];
    }
    if (*pwResponseLen > RCS620S_MAX_RW_RESPONSE_LEN) 
	{
        return false;
    }

	// 応答データ受信
	bRet = USART_Receive( pszResponse, *pwResponseLen );
    if ( bRet == false )
	{
        RCS620S_Cancel();
        return false;
    }

	// 応答データ整合性チェック
    dcs = RCS620S_calcDCS( pszResponse, *pwResponseLen );
	bRet = USART_Receive( szBuff, 2 );
    if ( (bRet == false) || (szBuff[0] != dcs) || (szBuff[1] != 0x00) )
	{
        RCS620S_Cancel();
        return false;
    }

	return true;	
}


//=================================================================================================
// RCS620S初期化
//=================================================================================================
bool init_RCS620S(void)
{
    bool		bRet = false;


    /* RFConfiguration (various timings) */
	bRet = RCS620S_Command( (unsigned char *)"\xd4\x32\x02\x00\x00\x00", 6, g_szResponse, &g_wResponseLen );
	if ( (bRet == false) || (g_wResponseLen != 2) || (memcmp((const void *)g_szResponse, "\xd5\x33", 2) != 0) )
	{
		return false;
	}

    /* RFConfiguration (max retries) */
	bRet = RCS620S_Command( (unsigned char *)"\xd4\x32\x05\x00\x00\x00", 6, g_szResponse, &g_wResponseLen );
	if ( (bRet == false) || (g_wResponseLen != 2) || (memcmp((const void *)g_szResponse, "\xd5\x33", 2) != 0) )
	{
		return false;
	}

    /* RFConfiguration (additional wait time = 24ms) */
	bRet = RCS620S_Command( (unsigned char *)"\xd4\x32\x81\xb7", 4, g_szResponse, &g_wResponseLen );
	if ( (bRet == false) || (g_wResponseLen != 2) || (memcmp((const void *)g_szResponse, "\xd5\x33", 2) != 0) )
	{
		return false;
	}

    return true;
}


//=================================================================================================
// RCS620S ポーリング
//=================================================================================================
bool RCS620S_Polling( uint16_t wSystemCode )
{
	bool			bRet = false;
	unsigned char	szBuff[ 9 ];
	
    /* InListPassiveTarget */
    memcpy( szBuff, (const void *)"\xd4\x4a\x01\x01\x00\xff\xff\x00\x00", 9);
    szBuff[5] = (unsigned char)((wSystemCode >> 8) & 0xff);
    szBuff[6] = (unsigned char)((wSystemCode >> 0) & 0xff);

	bRet = RCS620S_Command( szBuff, 9, g_szResponse, &g_wResponseLen );
	if ( (bRet == false) || (g_wResponseLen != 22) || (memcmp((const void *)g_szResponse, (const void *)"\xd5\x4b\x01\x01\x12\x01", 6) != 0) )
	{
		return false;
	}
	memcpy( (void *)g_szIdm, (const void *)&g_szResponse[ 6 ], 8 );
	memcpy( (void *)g_szPmm, (const void *)&g_szResponse[ 14 ], 8 );

	return true;
}




