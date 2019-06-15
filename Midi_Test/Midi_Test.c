#define F_CPU   ( 20000000UL )
#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "ff.h"
#include "SpiLib.h"
#include "SdLib.h"
#include "VS1053Lib.h"


#define BAUD		( 14400UL )					// 目的USARTボーレート
#define MYUBRR		F_CPU / 16 / BAUD - 1		// 目的UBRRn値



FATFS	g_Fatfs;								// FatFsオブジェクト
FILINFO *g_pFileInfo;							// ファイル情報
UINT    g_iFileInfoNum;							// ファイル情報数

FIL		Fil;									// Fileオブジェクト


BYTE	Buff[512];				/* File read buffer */


//---------------------------------------------------------
// USART初期化処理
//---------------------------------------------------------
void USART_Init( void )
{
	DDRD  = 0b11111100;
	PORTD = 0b00000000;


	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char)(MYUBRR);
	UCSR0A = 0b00000000;
	UCSR0B = 0b00011000;	// 受信許可・送信許可
	UCSR0C = 0b00000110;	// 非同期・パリティなし・停止ビット：1bit・データビット：8bit
	
	return;
}


//---------------------------------------------------------
// 受信処理
//---------------------------------------------------------
unsigned char USART_Receive(void)
{
	// 受信完了待ち（未読データが無くなるまでループ）
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


//---------------------------------------------------------
// メッセージ送信処理
//---------------------------------------------------------
void USART_SendMessage( char *pMsg )
{
	for ( uint16_t i = 0 ; i < (strlen(pMsg)) ;  i++ )
	{
		USART_Transmit( pMsg[i] );
	}
}

//---------------------------------------------------------
// ファイルリスト読み出し
//---------------------------------------------------------
bool GetMusicList( char *pPath )
{
	FRESULT Ret = FR_OK;
	DIR dir;				
	FILINFO TempFileInfo;
	uint16_t iIndex = 0;
	

	g_iFileInfoNum = 0;
	free( g_pFileInfo );
	
	// ファイル数をカウントする
	Ret = f_opendir( &dir, pPath );
	if ( Ret != FR_OK) return false;
	
	while ( 1 )
	{
		// ファイル情報取得
		Ret = f_readdir( &dir, &TempFileInfo );
		if ( Ret != FR_OK) return false;
		
		// ファイル情報終了？
		if ( strlen(TempFileInfo.fname) == 0 ) break;
		
		// ファイル？
		if ( (TempFileInfo.fattrib & AM_DIR) != AM_DIR )
		{
			g_iFileInfoNum = g_iFileInfoNum + 1;
		}
	}
	
	if ( g_iFileInfoNum == 0 ) return true;
	
	// リスト情報格納領域確保
	g_pFileInfo = (FILINFO*)malloc( (sizeof(FILINFO) * g_iFileInfoNum) );
	if ( g_pFileInfo == 0 ) return false;
	
	// ファイル情報取得して格納する
	Ret = f_opendir( &dir, pPath );
	if ( Ret != FR_OK) return false;
	
	iIndex = 0;
	while ( 1 )
	{
		// ファイル情報取得
		Ret = f_readdir( &dir, &TempFileInfo );
		if ( Ret != FR_OK) return false;
		
		// ファイル情報終了？
		if ( strlen(TempFileInfo.fname) == 0 ) break;
		
		// ファイル？
		if ( (TempFileInfo.fattrib & AM_DIR) != AM_DIR )
		{
			memcpy( &g_pFileInfo[iIndex], &TempFileInfo, sizeof(FILINFO));
			iIndex++;
		}
	}
	
	return true;
}	


//---------------------------------------------------------
// ファイルリスト表示処理
//---------------------------------------------------------
uint16_t DispMusicList( )
{
	char szBuff[100];
	char szRecv[100];
	char Recv;
	uint16_t iIndex = 0;
	uint16_t iRet = 0;
	
	USART_SendMessage("-----< Select MP3 File >-----\r\n");
		
	for ( uint16_t i = 0 ; i < g_iFileInfoNum ; i++ )
	{
		sprintf( szBuff, "[%d] : %s\r\n", (i+1), g_pFileInfo[i].fname );
		USART_SendMessage(szBuff);
	}
	USART_SendMessage(">");
	
	// リスト番号受付
	iIndex = 0;
	memset( szRecv, 0x00, sizeof(szRecv) );
	while (1)
	{
		Recv = USART_Receive();
	
		// エンターキー?
		if ( Recv == 0x0D )
		{
			// 文字を数値に変換
			iRet = atoi( szRecv );
			if ( iRet == 0 )
			{
				memset( szRecv, 0x00, sizeof(szRecv) );
				iIndex = 0;
			}
			else
			{
				// ファイル番号チェック？
				iRet = iRet - 1;
				if ( iRet > g_iFileInfoNum )
				{
					memset( szRecv, 0x00, sizeof(szRecv) );
					iIndex = 0;					
				}
				else
				{
					// ループを抜ける
					break;
				}
			}
		}
		else
		{
			// 受信データをバッファに格納する4
			szRecv[iIndex] = Recv;
			iIndex++;
			if ( iIndex >= 100 )
			{
				memset( szRecv, 0x00, sizeof(szRecv) );
				iIndex = 0;
			}
		}		
	}	
	
	return iRet;
}


//---------------------------------------------------------
// 音楽再生
//---------------------------------------------------------
void PlayMusic( uint16_t iFileListIndex )
{
	FRESULT Ret = FR_OK;
	UINT bw, br;
	
	Ret = f_open(&Fil, g_pFileInfo[iFileListIndex].fname, FA_READ);
	if ( Ret != FR_OK )
	{
		USART_SendMessage("f_open Error!!\r\n");
		return;
	}	
	
	USART_SendMessage("Music playing...\r\n");
	
	// ファイルが終わるまで読み続ける
	while ( !f_eof(&Fil) )
	{
		Ret = f_read(&Fil, Buff, sizeof Buff, &br);
		if ( Ret != FR_OK )
		{
			USART_SendMessage("f_read Error!!\r\n");
			break;
		}
		
		// ファイルから読込んだデータをVS1053へ送信
		for ( UINT i = 0 ; i < br ; i++ )
		{
			while ( !VS1053_DREQ() );
			VS1053_SDI_Write( Buff[i] );				
		}
	}
	
	f_close(&Fil);
}

void main(void)
{
	FRESULT Ret = FR_OK;
	
	
	
	// PIN設定
	DDRA = VS1053_RESET | VS1053_CS | VS1053_DCS;
	PORTA = VS1053_RESET;
	DDRB = (SPI_SCK | SPI_MOSI | SD_CS);
	PORTB = SD_CS;
	SPCR = _BV(SPE) | _BV(MSTR);

	// シリアル初期化
	USART_Init();

	// SDカード初期化処理
	SD_Init();
	
	// VS1053初期化処理
	VS1053_Reset();
	
	// SDカードマウント
	Ret = f_mount(0, &g_Fatfs);
	if ( Ret != FR_OK )
	{
		USART_SendMessage("f_mount Error!!\r\n");
		while(1);
	}	

	// ファイルリスト取得
	if ( GetMusicList("0:") == false )
	{
		USART_SendMessage("GetMusicList Error!!\r\n");
		while(1);
	}
	
	while ( 1 )
	{
		// 音楽ファイル再生
		PlayMusic( DispMusicList() );		
	}		
}



	
	
	







//
//
//
//
//
//
//
//
//
//
//
//
//
//int main(void)
//{
	//FRESULT rc;				/* Result code */
	//DIR dir;				/* Directory object */
	//FILINFO fno;			/* File information object */
	//UINT bw, br;
	//uint16_t HDAT0 = 0x00;
	//uint16_t HDAT1 = 0x00;
	//uint16_t Address = 0xffff;
	//uint16_t Version = 0;
	//uint16_t endFileByte = 0;
//
//
	//// A
	//DDRA = VS1053_RESET | VS1053_CS | VS1053_DCS;
	//PORTA = VS1053_RESET;
	//DDRB = (SPI_SCK | SPI_MOSI | SD_CS);
	//PORTB = SD_CS;
	//SPCR = _BV(SPE) | _BV(MSTR);
//
	//
	//// SDカード初期化処理
	//SD_Init();
	//
	//// VS1053初期化処理
	//VS1053_Reset();
	//
	//// SDカードマウント
	//rc = f_mount(0, &Fatfs);
	//if ( rc == FR_OK )
	//{
		//
	//}
	//
	//
	//
	//rc = f_open(&Fil, "Washas.mp3", FA_READ);
	////rc = f_open(&Fil, "Nitijyo.mp3", FA_READ);
	////rc = f_open(&Fil, "challen.mp3", FA_READ);
	//if ( rc == FR_OK )
	//{	
		//while ( !f_eof(&Fil) )
		//{
			//rc = f_read(&Fil, Buff, sizeof Buff, &br);
			//if ( rc != FR_OK )
			//{
				//break;
			//}
				//
			//
			//for ( UINT i = 0 ; i < br ; i++ )
			//{
				//while ( !VS1053_DREQ() );
				//VS1053_SDI_Write( Buff[i] );				
			//}
		//}
		//
		//VS1053_DCS_LOW();
		//for ( uint16_t i = 0 ; i < 2048 ; i++ )
		//{
			//while ( !VS1053_DREQ() );
			//VS1053_SDI_Write( 0 );
		//}	
		//while ( !VS1053_DREQ() );
		//VS1053_DCS_HI();
			//
		//f_close(&Fil);
	//}	
////
////
////
////
////
	////HDAT0 = VS1053_SCI_Read( 0x08 );
	////HDAT1 = VS1053_SCI_Read( 0x09 );
////
	////VS1053_SCI_Write( 0x07, 0x1e02 );
	////Version = VS1053_SCI_Read( 0x06 );
////
	////VS1053_SCI_Write( 0x07, 0x1e06 );
	////endFileByte = VS1053_SCI_Read( 0x06 );
//
//
	//while(1)
	//{
		////sine_test_activate( 200 );
		////_delay_ms(1000);
		////sine_test_deactivate();
		////_delay_ms(1000);
		////VS1053_SCI_Write( 0x00, SM_SDINEW | SM_RESET );
	//}
//}
//