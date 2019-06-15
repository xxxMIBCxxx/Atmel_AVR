/*
 * TG12864E_TEST3.c
 *
 * Created: 2014/06/28 10:02:51
 *  Author: MIBC
 */ 

#include "avr/io.h"
#include "stdbool.h"
#include "util/delay.h"
#include "ff.h"
#include "SpiLib.h"
#include "SdLib.h"
#include "TG12864E_Lib.h"


#define TG12864E_WIDTH  ( 128 )
#define TG12864E_HEIGHT ( 64 )
#define DATA_SIZE		((TG12864E_WIDTH/8) * TG12864E_HEIGHT)


FATFS   g_Fatfs;			// FatFsオブジェクト


//=============================================================================
// データ読込処理
//=============================================================================
FRESULT ReadData( char *pFilePath, UINT uiSize, BYTE *pData )
{
	FIL		Fil;
	FRESULT Ret = FR_OK;
	UINT	br = 0;
	UINT    iCnt = 0;
	
	
	// ファイルオープン
	Ret = f_open(&Fil, pFilePath, FA_READ);
    if ( Ret != FR_OK )
    {
        return Ret;
    }	
	
	_delay_ms( 5 );
	
	// ファイルが終わるまで読み続ける
    while ( !f_eof(&Fil) )
    {
        // 512バイトずつ読む
		Ret = f_read( &Fil, &pData[iCnt * 512] , 512, &br);
		if ( Ret != FR_OK )
		{
			f_close(&Fil);
			return Ret;
		}
		iCnt++;
		_delay_ms( 5 );
	}	
	f_close(&Fil);
	
	return Ret;
}


//=============================================================================
// TG1286E描画処理
//=============================================================================
void DrawTG1286E( BYTE *pDrawData )
{
	TG12864_CS_ENUM eCS = CS1;
	int	x = 0;
	int y = 0;


	for ( y = 0 ; y < 8 ; y++ )
	{	
		eCS = CS1;
		TG12864E_SetPage( eCS, y );
		TG12864E_SetAddress( eCS, 0 );
			
		for ( x = 0 ; x < 64 ; x++ )
		{	
			TG12864E_WriteDisplayData( eCS, pDrawData[ (y * 128 + x) ] );
		}			
	
		eCS = CS2;
		TG12864E_SetPage( eCS, y );
		TG12864E_SetAddress( eCS, 0 );
		for ( x = 0 ; x < 64 ; x++ )
		{	
			TG12864E_WriteDisplayData( eCS, pDrawData[ (y * 128 + x + 64 ) ] );
		}
	}	
}


//=============================================================================
// メイン
//=============================================================================
int main(void)
{
	FRESULT		Ret = FR_OK;
	BYTE		Buff[ DATA_SIZE ];
	char		szFileName[ 60 ];
	uint32_t	iIndex = 500;
	
	// 使用するPINを設定
	DDRA = TG12864E_DB0 | TG12864E_DB1 | TG12864E_DB2 | TG12864E_DB3 | TG12864E_DB4 | TG12864E_DB5 | TG12864E_DB6 | TG12864E_DB7;
	DDRD = TG12864E_CS1 | TG12864E_CS2 | TG12864E_RST | TG12864E_E   | TG12864E_RW  | TG12864E_RS;
	
	DDRB = SD_CS | SPI_SCK | SPI_MOSI;
	PORTB = SD_CS;
	
	SPCR = _BV(SPE) | _BV(MSTR);
     
	// TG12864E初期化
    TG12864E_init();	
	
    // SDカード初期化処理
    SD_Init();	
	
	// SDカードマウント
    Ret = f_mount(0, &g_Fatfs);
    if ( Ret != FR_OK )
    {
        while(1);
    }   	
	
	_delay_ms( 10 );
	
    while(1)
    {
		sprintf( szFileName, "%04d.dat", iIndex );
		
		// データ読込
		Ret = ReadData( szFileName, DATA_SIZE, Buff );
		if ( Ret != FR_OK )
		{
			while(1);
		}

		// TG1286E描画処理
		DrawTG1286E( Buff );
		
		iIndex++;
    }
}