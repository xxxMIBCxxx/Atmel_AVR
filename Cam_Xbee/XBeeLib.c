#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "util/delay.h"
#include "XBeeLib.h"






//=================================================================================================
// デファイン定義
//=================================================================================================
#define XBEE_RECV_DATA			( 100 )
#define XBEE_RESET_TIMEOUT		( 3000 )				// XBEEリセットタイムアウト(ms)
#define SPI_Start()				PORTB = PORTB & ~(SPI_SS)
#define SPI_End()				PORTB = PORTB | SPI_SS


#define XBee_RecvWait()						\
{											\
	if ( (PINB & XBEE_ATTN) )				\
	{										\
		while( (PINB & XBEE_ATTN) );		\
	}										\
}

//=================================================================================================
// 構造体定義
//=================================================================================================
// XBEE 受信データ格納構造体
typedef struct 
{
	uint16_t			wWriteIndex;				// 書込みポジション
	uint16_t			wReadIndex;					// 読込ポジション
	uint16_t			wLength;					// 受信データを格納しているサイズ
	char				szData[ XBEE_RECV_DATA ];	// 受信データ格納領域

} XBEE_RECV_INFO_TABLE, *PXBEE_RECV_INFO_TABLE;



//=================================================================================================
// プロトタイプ宣言
//=================================================================================================
static void SPI_MasterInit(void);
static char SPI_MasterTransmit( char Data );
static bool xbee_GetRecvData(void);
static bool xbee_CompCheckSum( char *pData, uint16_t wDataSize, unsigned char CheckSum );
static void xbee_SetRecvData( char Data );

//=================================================================================================
// グローバル変数
//=================================================================================================
volatile static XBEE_RECV_INFO_TABLE		g_tXBeeRecvInfo;
volatile static char						g_szXBeeRecvData[ XBEE_RECV_DATA ];		// XBee 受信データ取得用


//=================================================================================================
// SPI関連
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// SPI初期化（主装置）
//-------------------------------------------------------------------------------------------------
static void SPI_MasterInit(void)
{
#if 0
	SPCR  = _BV(SPE) | _BV(MSTR) | _BV(SPR0);			// SPI許可,主装置,8分周(2.5MHz)
	SPSR  = _BV(SPI2X);
#else
	SPCR  = _BV(SPE) | _BV(MSTR);						// SPI許可,主装置,8分周(2.0MHz)
#endif

//	DDRB  |= SPI_SCK | SPI_MOSI | SPI_SS;				// SPIで使用するPINを設定(出力：SCK,MOSI,SS / 入力：MISO) 
//	PORTB |= SPI_SS;										
	DDRB  = SPI_SCK | SPI_MOSI | SPI_SS | XBEE_RESET | XBEE_DOUT;
	PORTB = SPI_SS | XBEE_RESET | XBEE_ATTN | SWITCH_PIN;
	
}


//-------------------------------------------------------------------------------------------------
// SPIデータ送信（主装置）
//-------------------------------------------------------------------------------------------------
static char SPI_MasterTransmit( char Data )
{
	SPDR = Data;
	
	// データ送信完了するまで待つ
	while( !(SPSR & _BV(SPIF)) );
	
	return SPDR;
}


//=================================================================================================
// XBee関連
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// XBee初期化
//-------------------------------------------------------------------------------------------------
bool XBee_Init(void)
{
	bool		bRet = true;
	
	// SPI初期化（主装置）
	SPI_MasterInit();
	
	// XBeeで使用するPINを設定
	DDRB  |= XBEE_RESET | XBEE_DOUT;
	PORTB = (PORTB & ~XBEE_DOUT) | XBEE_RESET | XBEE_ATTN;
	
	// XBeeリセット
	bRet = XBee_Reset();
		
	return true;
}


//-------------------------------------------------------------------------------------------------
// XBeeリセット
//-------------------------------------------------------------------------------------------------
bool XBee_Reset(void)
{
	bool					bRet = false;
	int						iRet = 0;
	uint16_t				wSize = 0;
	uint16_t				wTimeOut = 0;
	XBEE_API_FRAME_TABLE	tXBeeResApiFrame;
	

	//// XBeeをリセット
	//PORTB &= ~(XBEE_RESET | XBEE_DOUT);
	//_delay_ms( 50 );
	//PORTB |= XBEE_RESET;
	//_delay_ms( 50 );
	
	memset( (void *)&g_tXBeeRecvInfo, 0x00, sizeof(g_tXBeeRecvInfo) );
	
	XBee_RecvWait();
	
	// Modem Statusが通知されるまで待つ
	while( true )
	{
		wSize = XBEE_RECV_DATA;
		bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
		if ( bRet == true )
		{
			// 受信データ解析
			iRet = XBee_AnalyzeRecvData( (char *)g_szXBeeRecvData, wSize, &tXBeeResApiFrame );
			if ( iRet == 0 )
			{
				// API Frame Type:Modem Status(0x8A)
				if ( tXBeeResApiFrame.pszData[0] == 0x8A )
				{
					// Status:Hardware reset or power up(0)
					if ( tXBeeResApiFrame.pszData[1] == 0 )
					{
						bRet = true;
					}
					else
					{
						bRet = false;
					}
					free( tXBeeResApiFrame.pszData );
					break;								// ループを抜ける
				}
				
				// 違うフレームだった（ありえない？）
				free( tXBeeResApiFrame.pszData );
			}
		}
		
		_delay_ms( 100 );
		wTimeOut = wTimeOut + 100;
		if ( wTimeOut >= XBEE_RESET_TIMEOUT )
		{
			break;
		}
	}
		
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// XBeeデータ受信
//-------------------------------------------------------------------------------------------------
static bool xbee_GetRecvData(void)
{
	bool		bRet = false;
	
	
	// SPI_ATTNがLowの場合、受信データが存在しているのでデータを受信
	if ( !(PINB & XBEE_ATTN) )
	{
		SPI_Start();
		do 
		{	
			// ダミーデータを送信して、受信データを取得
			g_tXBeeRecvInfo.szData[ g_tXBeeRecvInfo.wWriteIndex ] = SPI_MasterTransmit( 0x00 );
			g_tXBeeRecvInfo.wWriteIndex = g_tXBeeRecvInfo.wWriteIndex + 1;
			g_tXBeeRecvInfo.wLength = g_tXBeeRecvInfo.wLength + 1;
			if ( g_tXBeeRecvInfo.wWriteIndex >= XBEE_RECV_DATA )
			{
				g_tXBeeRecvInfo.wWriteIndex = g_tXBeeRecvInfo.wWriteIndex - XBEE_RECV_DATA;
			}
		
		} while ( !(PINB & XBEE_ATTN) );
		SPI_End();
		bRet = true;
	}
	
	return bRet;	
}


//-------------------------------------------------------------------------------------------------
// XBee受信データ取得
//-------------------------------------------------------------------------------------------------
bool XBee_GetRecvData( char *pData, uint16_t *pwSize )
{
	bool			bRet = false;
	uint16_t		wReadSize = 0;
	uint16_t		wIndex = 0;
	
	
	// とりあえずXBee側の溜まっているデータを全て受信する
	bRet = xbee_GetRecvData();
	
	// 受信データが存在する？
	if ( g_tXBeeRecvInfo.wLength != 0 )
	{
		if ( *pwSize >= g_tXBeeRecvInfo.wLength )
		{
			*pwSize = g_tXBeeRecvInfo.wLength;
		}
		
		// 取得データが受信データ格納領域を超える？
		if ( (g_tXBeeRecvInfo.wReadIndex + *pwSize) >= XBEE_RECV_DATA )
		{
			wReadSize = XBEE_RECV_DATA - g_tXBeeRecvInfo.wReadIndex;
			memcpy( &pData[ wIndex ], (const char*)&g_tXBeeRecvInfo.szData[ g_tXBeeRecvInfo.wReadIndex ], wReadSize );
			wIndex = wReadSize;
// 20120821-[BUG]-CHG[S]
//			wReadSize = (g_tXBeeRecvInfo.wReadIndex + *pwSize) - wReadSize;
			wReadSize = (g_tXBeeRecvInfo.wReadIndex + *pwSize) - XBEE_RECV_DATA;
// 20120821-[BUG]-CHG[E]
			memcpy( &pData[ wIndex ], (const char*)&g_tXBeeRecvInfo.szData[ 0 ], wReadSize );
			g_tXBeeRecvInfo.wReadIndex = wReadSize;
		}
		else
		{
			memcpy( &pData[ wIndex ], (const char*)&g_tXBeeRecvInfo.szData[ g_tXBeeRecvInfo.wReadIndex ], *pwSize );
			g_tXBeeRecvInfo.wReadIndex = g_tXBeeRecvInfo.wReadIndex + *pwSize;
		}
		g_tXBeeRecvInfo.wLength = g_tXBeeRecvInfo.wLength - *pwSize;
		bRet = true;
	}
	
	return bRet;	
}


//-------------------------------------------------------------------------------------------------
// XBee 受信データ解析
// ※XBee 受信データ解析の戻り値がtrueの場合、解析データが不要になったらPXBEE_API_FRAME_TABLEの
//   pDataをfree関数を使用して領域を解放してください
// 戻り値： 0 = 正常
//         -1 = API Frameデータでない
//         -2 = サイズチェックエラー
//         -3 = メモリ生成エラー
//         -4 = チェックサムエラー 
//-------------------------------------------------------------------------------------------------
int XBee_AnalyzeRecvData( char *pData, uint16_t wDataSize, PXBEE_API_FRAME_TABLE ptXBeeApiFrame )
{
	int				iRet = -1;
	bool			bRet = false;
	uint16_t		wIndex = 0;
	
	
	while ( wIndex < wDataSize )
	{
		// Start Delimiterが有るか調べる
		if ( pData[ wIndex ] == 0x7E )
		{
			ptXBeeApiFrame->StartDelimiter = pData[ wIndex ];								// Start Delimiter
			ptXBeeApiFrame->wLength = ((uint16_t)pData[ wIndex+1 ]) << 8;					// データサイズ
			ptXBeeApiFrame->wLength |=  (uint16_t)pData[ wIndex+2 ];
			wIndex = wIndex + 3;
			
			// サイズチェック
			if ( wDataSize < (wIndex + ptXBeeApiFrame->wLength + 1) )
			{
				iRet = -2;
				break;
			}
			
			// 取得したデータサイズから格納領域を生成
			ptXBeeApiFrame->pszData = (char *)malloc( ptXBeeApiFrame->wLength );
			if ( ptXBeeApiFrame->pszData == NULL )
			{
				iRet = -3;
				break;
			}
			memcpy( ptXBeeApiFrame->pszData, &pData[ wIndex ], ptXBeeApiFrame->wLength );	// 受信データ
			wIndex = wIndex + ptXBeeApiFrame->wLength;
			ptXBeeApiFrame->CheckSum = pData[ wIndex ];										// チェックサム
			
			// チェックサムをチェック
			bRet = xbee_CompCheckSum( ptXBeeApiFrame->pszData, ptXBeeApiFrame->wLength, ptXBeeApiFrame->CheckSum );
			if ( bRet == false )
			{
				iRet = -4;
				break;
			}
			
			iRet = 0;
			break;
		}
	
		wIndex++;
	}			
	
	// 正常終了以外の場合
	if ( iRet != 0 )
	{
		if ( ptXBeeApiFrame->pszData != NULL )
		{
			free( ptXBeeApiFrame->pszData );
			ptXBeeApiFrame->pszData = NULL;
		}
	}
	
	return iRet;
}


//-------------------------------------------------------------------------------------------------
// XBee チェックサム生成
//-------------------------------------------------------------------------------------------------
unsigned char XBee_CreateCheckSum( char *pData, uint16_t wDataSize )
{
	uint16_t			wIndex = 0;
	unsigned char		Sum = 0x00;
	unsigned char		CheckSum = 0x00;
	
	
	// データをすべて合計する
	for ( wIndex = 0 ; wIndex < wDataSize ; wIndex++ )
	{
		Sum = Sum + (unsigned char)pData[ wIndex ];
	}
	CheckSum = 0xFF - Sum;
	
	return CheckSum;
}


//-------------------------------------------------------------------------------------------------
// XBee チェックサム比較
//-------------------------------------------------------------------------------------------------
static bool xbee_CompCheckSum( char *pData, uint16_t wDataSize, unsigned char CheckSum )
{
	bool				bRet = false;
	unsigned char		TempCheckSum = 0x00;
	
	
	// 受信データからチェックサムを生成
	TempCheckSum = XBee_CreateCheckSum( pData, wDataSize );
	
	// 比較
	bRet = ( TempCheckSum == CheckSum ) ? true : false;
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// XBee 受信データをセット
//-------------------------------------------------------------------------------------------------
static void xbee_SetRecvData( char Data )
{
	g_tXBeeRecvInfo.szData[ g_tXBeeRecvInfo.wWriteIndex ] = Data;
	g_tXBeeRecvInfo.wWriteIndex = g_tXBeeRecvInfo.wWriteIndex + 1;
	g_tXBeeRecvInfo.wLength = g_tXBeeRecvInfo.wLength + 1;
	if ( g_tXBeeRecvInfo.wWriteIndex >= XBEE_RECV_DATA )
	{
		g_tXBeeRecvInfo.wWriteIndex = g_tXBeeRecvInfo.wWriteIndex - XBEE_RECV_DATA;
	}	
}



//-------------------------------------------------------------------------------------------------
// XBee Api Frame送信
//-------------------------------------------------------------------------------------------------
bool XBee_SendApiFrame( const PXBEE_API_FRAME_TABLE ptXBeeApiFrame, PXBEE_API_FRAME_TABLE ptXBeeResApiFrame )
{
	bool			bRet = false;
	int				iRet = 0;
	char			Data = 0x00;
	uint16_t		wIndex = 0;
	uint16_t		wSize = 0;
	uint16_t		wTimeOut = 0;
	bool			bRecvFlag = false;
	
	
	SPI_Start();
	
	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( ptXBeeApiFrame->StartDelimiter );						// Start Delimiter
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );
	
	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( (char)((ptXBeeApiFrame->wLength & 0xFF00) >> 8) );		// データサイズ
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );

	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( (char)(ptXBeeApiFrame->wLength & 0x00FF) );
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );
	
	for ( wIndex = 0 ; wIndex < ptXBeeApiFrame->wLength ; wIndex++ )					// データ
	{
		bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
		Data = SPI_MasterTransmit( ptXBeeApiFrame->pszData[ wIndex ] );
		if ( bRecvFlag == true ) xbee_SetRecvData( Data );
	}
	
	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( ptXBeeApiFrame->CheckSum );								// チェックサム
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );

	SPI_End();

	if ( ptXBeeResApiFrame != NULL )
	{
		XBee_RecvWait();
		
		while ( 1 )
		{
			// データを受信
			wSize = XBEE_RECV_DATA;
			bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
			if ( bRet == true )
			{
				// Api Frame解析
				bRet = XBee_AnalyzeRecvData( (char *)g_szXBeeRecvData, wSize, ptXBeeResApiFrame );
				if ( iRet == 0)
				{
					bRet = true;
					break;
				}
			}
			
			// データ受信タイムアウト？
			_delay_ms(100);
			wTimeOut = wTimeOut + 100;
			if ( wTimeOut >= XBEE_RESET_TIMEOUT )
			{
				bRet = false;
				break;
			}			
		}
	}
	else
	{
		bRet = true;
	}	

	return bRet;
}


//-------------------------------------------------------------------------------------------------
// XBee Wi-Fi接続待ち
//-------------------------------------------------------------------------------------------------
bool XBee_JoinedWifi( void )
{
	bool						bRet = false;
	int							iRet = 0;
	uint16_t					wSize = 0;
	uint16_t					wTimeOut = 0;
	XBEE_API_FRAME_TABLE		tXBeeResApiFrame;
	
	
	XBee_RecvWait();
	
	while ( 1 )
	{
		// データを受信
		wSize = XBEE_RECV_DATA;
		bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
		if ( bRet == true )
		{
			// Api Frame解析
			bRet = XBee_AnalyzeRecvData( (char *)g_szXBeeRecvData, wSize, &tXBeeResApiFrame );
			if ( iRet == 0)
			{
				// Modem Status?
				if ( tXBeeResApiFrame.pszData[0] == 0x8A )
				{
					// Joined?
					if ( tXBeeResApiFrame.pszData[1] == 0x02 )
					{
						bRet = true;
					}
					free( tXBeeResApiFrame.pszData );
					break;
				}
				free( tXBeeResApiFrame.pszData );
			}
		}
			
		// データ受信タイムアウト？
		_delay_ms(100);
		wTimeOut = wTimeOut + 100;
		if ( wTimeOut >= XBEE_RESET_TIMEOUT )
		{
			bRet = false;
			break;
		}			
	}

	return bRet;	
}