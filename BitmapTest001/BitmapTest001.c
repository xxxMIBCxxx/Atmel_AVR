/*
 * BitmapTest001.c
 *
 * Created: 2014/03/24 20:42:22
 *  Author: MIBC
 */ 


#include "avr/io.h"
#include "stdbool.h"
#include "util/delay.h"
#include "ff.h"
#include "SpiLib.h"
#include "SdLib.h"
#include "LM024C9325.h"


FATFS   g_Fatfs;                    // FatFsオブジェクト
FILINFO *g_pFileInfo;               // ファイル情報
UINT    g_iFileInfoNum;             // ファイル情報数
FIL     Fil;                        // Fileオブジェクト
BYTE    Buff[512];					// File read buffer


//-----------------------------------------------------------------------------
// 画像データ読込
//-----------------------------------------------------------------------------
bool ReadImage( char *pFilePath )
{
	FRESULT Ret = FR_OK;
    UINT br;
	bool bErr = false;
	int32_t color = 0;
     
    Ret = f_open(&Fil, pFilePath, FA_READ);
    if ( Ret != FR_OK )
    {
		return false;
	}		
	
	// LCD書込み開始
	LM024C9325_StartGRAM();
	
	// ファイルが終わるまで読み続ける
    while ( !f_eof(&Fil) )
    {
		// 512バイトずつ読む
		Ret = f_read(&Fil, Buff, sizeof(Buff), &br);
		if ( Ret != FR_OK )
		{
			bErr = true;
			break;
		}
		if ( (br % 4) != 0 )
		{
			bErr = true;
			break;
		}
		
		for ( int32_t i = 0 ; i < (br /4) ; i++ )
		{
			// LCD書込み
			color = Buff[i*4+1];
			color = color << 8;
			color = color | Buff[i*4+2];
			color = color << 8;
			color = color | Buff[i*4+3];
			LM024C9325_WriteGRAM( color );
		}		
    }
     
    f_close(&Fil);	
	
	return ((bErr == false) ? true : false);
}



int main(void)
{
	FRESULT Ret = FR_OK;
	int		cnt = 1;
	char	szFileName[ 50 ];
	
	
	DDRA = LM024C9325_D8 | LM024C9325_D9 | LM024C9325_D10 | LM024C9325_D11 | 
           LM024C9325_D12 | LM024C9325_D13 | LM024C9325_D14 | LM024C9325_D15;
    PORTA = ~(LM024C9325_D8 | LM024C9325_D9 | LM024C9325_D10 | LM024C9325_D11 | 
              LM024C9325_D12 | LM024C9325_D13 | LM024C9325_D14 | LM024C9325_D15);
    
	DDRB = LM024C9325_RST | LM024C9325_CS | LM024C9325_RS | LM024C9325_WR | LM024C9325_RD | SPI_SCK | SPI_MOSI;
    PORTB = LM024C9325_RST | LM024C9325_CS | LM024C9325_RS | LM024C9325_WR | LM024C9325_RD;
	
	DDRD = SD_CS;
	PORTD = SD_CS;
	
	SPCR = _BV(SPE) | _BV(MSTR);
	
	// SDカード初期化処理
    SD_Init();
	
	// LCD初期化処理 
    LM024C9325_Init();
	
	// SDカードマウント
    Ret = f_mount(0, &g_Fatfs);
    if ( Ret != FR_OK )
    {
        //USART_SendMessage("f_mount Error!!\r\n");
        while(1);
    }   
	
    while(1)
    {
		// 画像データファイル名生成(x.dat)
		sprintf( szFileName, "%d.dat", cnt );
		
        // 画像データファイルを読込んで、LCDに表示
		if (ReadImage( szFileName ) == false )
		{
			break;
		}
		
		cnt++;
		if ( cnt > 3)
		{
			cnt = 1;
		}
		
		// 5秒待つ
		_delay_ms( 1000 * 5 );
    }
}