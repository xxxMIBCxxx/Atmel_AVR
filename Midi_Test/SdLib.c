#include "SdLib.h"
#include "SpiLib.h"
#include <util/delay.h>




// SDカードのコマンド
#define SD_CMD00			( 0 )			// カードをリセットする
#define SD_CMD01			( 1 )			// リセットが終了したか判定する
#define SD_CMD16			( 16 )			// ブロック長を指定する
#define SD_CMD17			( 17 )			// ブロック長分読込む
#define SD_CMD24			( 24 )			// ブロック長分書込む

const unsigned char DUMMY_SEND_DATA[] = { 0xFF, 0x00 }; 
const unsigned char DUMMY_CRC_DATA[] = { 0xFF, 0xFF };
const unsigned char WRITE_START_DATA[] = { 0xFE, 0x00 };

// プロトタイプ宣言
static bool SD_SendCommand( bool bDummyData, unsigned char Cmd, uint32_t arg, unsigned char *pResponse );
static void SD_SendDummyData();

// グローバル変数
uint32_t g_uiBlockSize = 0;					// ブロック長の保持用


//-------------------------------------------------------------------------------------------------
// SD初期化処理
//-------------------------------------------------------------------------------------------------
bool SD_Init()
{
	bool bFlag = false;
	bool bRet = false;
	unsigned char Response = 0x00;
	
	
	// SPI初期化（※SD初期化が終わるまで、SPI Clockを400KHz以下にしておく）
	SPI_SetClockRate(SPI_CLOCK_RATE128);
	
	// ダミーデータ送信
	SD_SendDummyData();
	
	// SDカードリセット
	bRet = SD_SendCommand( false, SD_CMD00, 0, &Response );
	if ( bRet == false )
	{
		return false;
	}
	// 応答は「カード初期化中」の場合
	if ( Response == 0x01 )
	{
		bFlag = false;
		for ( int16_t i = 0 ; i < 1000 ; i++ )
		{
			// SDカード初期化完了問い合わせ
			bRet = SD_SendCommand( true, SD_CMD01, 0, &Response );
			if ( bRet == false )
			{
				return false;
			}
			
			if ( Response == 0x01 )
			{
				_delay_us(100);
				continue;
			}
			
			// 応答は「初期化完了」以外の場合
			if ( Response != 0x00 )
			{
				return false;
			}
			
			// ループを抜ける
			bFlag = true;
			break;
		}
		if ( bFlag = false )
		{
			return false;	
		}
	}
	
	// SPIの通信速度を戻す
	SPI_SetClockRate(SPI_CLOCK_RATE8);

	return true;	
}


//-------------------------------------------------------------------------------------------------
// ブロック長を設定
//-------------------------------------------------------------------------------------------------
bool SD_SetBlockSize( uint32_t uiBlockSize )
{
	bool bRet = false;
	unsigned char Response = 0x00;
	
	// ブロック長を設定
	bRet = SD_SendCommand( false, SD_CMD16, uiBlockSize, &Response );
	if ( bRet == false )
	{
		return false;
	}
	
	// 応答を確認
	if ( Response != 0x00 )
	{
		return false;
	}
	
	// 内部でブロック長を保持
	g_uiBlockSize = uiBlockSize;
	
	return true;
}


//-------------------------------------------------------------------------------------------------
// データ読込処理
//-------------------------------------------------------------------------------------------------
bool SD_ReadBlockData( uint32_t uiAddress, unsigned char *pReadData )
{
	bool bFlag = false;
	bool bRet = false;
	unsigned char Response = 0x00;
	unsigned char CRC[2];
	unsigned char SendDummyData = 0xFF;
	
	// まだブロック長を設定していない場合
	if ( g_uiBlockSize == 0 )
	{
		return false;
	}
	
	// パラメータチェック
	if ((uiAddress % g_uiBlockSize) != 0 )
	{
		return false;
	}
	
	// 読込コマンド発行
	bRet = SD_SendCommand( false, SD_CMD17, uiAddress, &Response );
	if ((bRet == false) && (Response != 0x00))
	{
		return false;
	}
	
	// 読込開始(0xFE)の応答待ち
	SdCS_LOW;
	bFlag = false;
	for ( uint32_t i = 0 ; i < 1000 ; i++ )
	{
		SPI_MasterRecv( &Response, 1);

		if ( Response == 0xFE )
		{
			// ブロック長分データ読込
			SPI_MasterRecv( pReadData, g_uiBlockSize );
	
			// CRC(2バイト)を読込む
			SPI_MasterRecv( CRC, 2 );
			
			bFlag = true;
			break;
		}
		
		if ( Response != 0xFF )
		{
			bFlag = false;
			break;
		}
		
		_delay_us(200);
	}	
		
	// ダミーデータ送信(次のコマンドを受付できるようにする)
	SPI_MasterSend( DUMMY_SEND_DATA, 1 );
	SdCS_HI;
	
	return bFlag;
}


//-------------------------------------------------------------------------------------------------
// データ書込み処理
//-------------------------------------------------------------------------------------------------
bool SD_WriteBlockData( uint32_t uiAddress, unsigned char *pWriteData )
{
	bool bRet = false;
	bool bFlag = false;
	unsigned char Response = 0x00;
	unsigned char SendDummyData = 0xFF;
	
	
	// まだブロック長を設定していない場合
	if ( g_uiBlockSize == 0 )
	{
		return false;
	}
	
	// パラメータチェック
	if ((uiAddress % g_uiBlockSize) != 0 )
	{
		return false;
	}
	
	// 書込みコマンド発行
	bRet = SD_SendCommand( false, SD_CMD24, uiAddress, &Response );
	if ((bRet == false) &&  (Response != 0x00))
	{
		return false;
	}

	// 読込開始コマンド
	SdCS_LOW;
	SPI_MasterSend( WRITE_START_DATA, 1 );					// 0xFE
	
	// データ書込み
	SPI_MasterSend( pWriteData, g_uiBlockSize );			// WriteData

	// CRC送信
	SPI_MasterSend( DUMMY_CRC_DATA, 2 );					// CRC(0xFF,0xFF)
	
	// 応答受信
	bFlag = false;
	SPI_MasterRecv( &Response, 1);
	if ( (Response & 0x0F) == 0x05 )
	{
		// BUSY(0x00)解除待ち
		for ( uint32_t i = 0 ; i < 5000 ; i++ )
		{
			SPI_MasterRecv( &Response, 1);
			if ( Response != 0x00 )
			{
				bFlag = true;
				break;
			}
			_delay_us(200);
		}
	}	

	// ダミーデータ送信(次のコマンドを受付できるようにする)
	SPI_MasterSend( DUMMY_SEND_DATA, 1 );
	SdCS_HI;

	
	return bFlag;
}


//-------------------------------------------------------------------------------------------------
// コマンド発行
//-------------------------------------------------------------------------------------------------
static bool SD_SendCommand( bool bDummyData, unsigned char Cmd, uint32_t arg, unsigned char *pResponse )
{
	bool bRet = false;
	unsigned char SendData[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
	unsigned char DummySendData = 0xFF;
		
	// コマンド
	SendData[0] = 0x40 | (Cmd & 0x3F);
	
	// パラメータ
	SendData[1] = (unsigned char)(arg >> 24);
	SendData[2] = (unsigned char)(arg >> 16);
	SendData[3] = (unsigned char)(arg >>  8);
	SendData[4] = (unsigned char)arg;
	
	// CRC7
	if ( Cmd == SD_CMD00 ) SendData[5] = 0x95;
	if ( Cmd == SD_CMD01 ) SendData[5] = 0xF9;
	
	
	// 送信前にダミーデータを送信する
	if ( bDummyData == true )
	{
		SdCS_HI;
		SPI_MasterSend( &DummySendData, 1 );
	}	
	
	SdCS_LOW;
	
	// 発行
	SPI_MasterSend( SendData, 6 );
	
	// 応答を待つ
	for ( int16_t i = 0 ; i < 1000 ; i++ )
	{
		SPI_MasterRecv( pResponse, 1 );
		if ( (*pResponse & 0x80) != 0x80 )
		{
			bRet = true;
			break;
		}
		_delay_us(100);
	}
	SdCS_HI;

	return bRet;
}


//-------------------------------------------------------------------------------------------------
// ダミーデータ送信処理
// ※マスタからSDカードへ80クロックのダミークロックを送る
//-------------------------------------------------------------------------------------------------
static void SD_SendDummyData()
{
	unsigned char SendData = 0xFF;
	
	// SD SCをLOWにする
	SdCS_LOW;

	// 80クロック(10バイト)分のダミーデータを送信
	for ( int16_t i = 0 ; i < 10 ; i++ )
	{
		SPI_MasterSend( &SendData, 1 );
	}
	_delay_us(500);
	
	return;
}
