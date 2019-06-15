
#include "L6470Lib.h"
#include "SpiLib.h"
#include "util/delay.h"

#define L6470_READ_ADDRESS			( 0x20 )
#define L6470_END_ADDRESS			( 0xFF )

typedef union
{
	uint32_t uiData;
	unsigned char Data[4];
} L6470_ParamData;



typedef struct
{
	unsigned char Address;				// レジスタアドレス
	uint32_t BitLength;					// 長さ(Bit)
	uint32_t Default;					// #12026の設定値(0xFFFFFFFF:設定しない)
	bool bReadOnly;						// 読み取り専用(TRUE:読み取り専用)
} L6470_RegisterTable;



L6470_RegisterTable g_tRegister[] = {
	{ L6470_ABS_POS		, 22, 0			, false },
	{ L6470_EL_POS		,  9, 0			, false },
	{ L6470_MARK		, 22, 0			, false },
	{ L6470_SPEED		, 20, 0			, false },
	{ L6470_ACC			, 12, 0x8A		, false },
	{ L6470_DEC			, 12, 0x8A		, false },
	{ L6470_MAX_SPEED	, 10, 0x20      , false },
	{ L6470_MIN_SPEED	, 13, 0			, false },
	{ L6470_KVAL_HOLD	,  8, 0xFF      , false },
	{ L6470_KVAL_RUN	,  8, 0xFF      , false },
	{ L6470_KVAL_ACC	,  8, 0xFF      , false },
	{ L6470_KVAL_DEC	,  8, 0xFF      , false },
	{ L6470_INT_SPD		, 14, 0x408		, false },
	{ L6470_ST_SLP		,  8, 0x19		, false },
	{ L6470_FN_SLP_ACC	,  8, 0x29		, false },
	{ L6470_FN_SLP_DEC	,  8, 0x29		, false },
	{ L6470_K_THERM 	,  4, 0			, false },
	{ L6470_ADC_OUT 	,  5, 0xFFFFFFFF, true	},
	{ L6470_ODC_TH 		,  4, 0x0F		, false },
	{ L6470_STALL_TH	,  7, 0x7F		, false },
	{ L6470_FS_SPD		, 10, 0x27		, false },
	{ L6470_STEP_MODE	,  8, 0x07		, false },
	{ L6470_ALARM_EN	,  8, 0xFF		, false },
	{ L6470_CONFIG		, 16, 0x2E88	, false },
	{ L6470_STATUS		, 16, 0xFFFFFFFF, true	},
	{ L6470_RUN			, 20, 0xFFFFFFFF, true	},			// 正回転
	{ L6470_R_RUN		, 20, 0xFFFFFFFF, true	},			// 逆回転
	{ L6470_STOP		,  0, 0xFFFFFFFF, true	},			// 停止
	{ L6470_END_ADDRESS	,  0, 0xFFFFFFFF, false },			// 終端
};




//-----------------------------------------------------------------------------
// L6470 初期化処理
//-----------------------------------------------------------------------------
void L6470_Init()
{
	int16_t		iIndex = 0;
	int16_t		iMaxDataNum = sizeof(g_tRegister) / sizeof(L6470_RegisterTable);
	

	// SPI初期化(5MHz)
	SPI_MasterInit( SPI_CLOCK_RATE4 );
	
	PORTB = PORTB | L6470_STBY;
	_delay_ms(100);
	PORTB = PORTB & ~L6470_STBY;
	
	//for ( iIndex = 0 ; iIndex < iMaxDataNum ; iIndex++ )
	//{
		//if ( g_tRegister[iIndex].Default != 0xFFFFFFFF )
		//{
			//L6470_SetParam( g_tRegister[iIndex].Address, g_tRegister[iIndex].Default );
		//}
	//}
	
	return;
}

//-----------------------------------------------------------------------------
// L6470 パラメータ設定
//-----------------------------------------------------------------------------
bool L6470_SetParam( unsigned char Address, uint32_t uiValue )
{
	uint16_t TransmitByte = 0;
	uint16_t index = 0;
	uint32_t Mask = 0x00000000;
	L6470_ParamData	Data;
	
	// 指定されたアドレスが存在するか調べる
	index = 0;
	while ( 1 )
	{
		// 終端?
		if ( g_tRegister[index].Address == L6470_END_ADDRESS )
		{
			return false;
		}
		
		// アドレス一致？
		if ( g_tRegister[index].Address == Address )
		{
			break;
		}
		
		index++;
	}
	
	// 読み取り専用？
	if ( g_tRegister[index].bReadOnly == true )
	{
		return false;
	}	

	// 転送バイトを算出
	TransmitByte = g_tRegister[index].BitLength / 8;
	TransmitByte += ((g_tRegister[index].BitLength % 8)!=0) ? 1 : 0;
	
	// マスクデータ生成
	for ( uint16_t i = 0 ; i < g_tRegister[index].BitLength ; i++ )
	{
		Mask = Mask << 1;
		Mask = Mask | 0x00000001;
	}
	
	Data.uiData = uiValue & Mask;
	
	// アドレス送信	
	L6470_CS_LOW();
	SPI_MasterTransmit( Address );
	L6470_CS_HI();	
	
	// パラメータ送信
	for ( uint16_t i = TransmitByte ; i > 0 ; i-- )
	{
		L6470_CS_LOW();
		SPI_MasterTransmit( Data.Data[i-1] );
		L6470_CS_HI();			
	}
	
	return true;
}


//-----------------------------------------------------------------------------
// L6470 パラメータ取得
//-----------------------------------------------------------------------------
bool L6470_GetParam( unsigned char Address, uint32_t *puiValue )
{
	uint16_t TransmitByte = 0;
	uint16_t index = 0;
	uint32_t Mask = 0x00000000;
	L6470_ParamData	Data;
	
	
	Data.uiData = 0;
	
	// 指定されたアドレスが存在するか調べる
	index = 0;
	while ( 1 )
	{
		// 終端?
		if ( g_tRegister[index].Address == L6470_END_ADDRESS )
		{
			return false;
		}
		
		// アドレス一致？
		if ( g_tRegister[index].Address == Address )
		{
			break;
		}
		
		index++;
	}
		
	// マスクデータ生成
	for ( uint16_t i = 0 ; i < g_tRegister[index].BitLength ; i++ )
	{
		Mask = Mask << 1;
		Mask = Mask | 0x00000001;
	}

	// 転送バイトを算出
	TransmitByte = g_tRegister[index].BitLength / 8;
	TransmitByte += ((g_tRegister[index].BitLength % 8)!=0) ? 1 : 0;

	// アドレス送信	
	L6470_CS_LOW();
	SPI_MasterTransmit( (L6470_READ_ADDRESS | Address) );
	L6470_CS_HI();	
	
	// パラメータ取得
	for ( uint16_t i = TransmitByte ; i > 0 ; i-- )
	{
		L6470_CS_LOW();
		Data.Data[ i - 1 ] = SPI_MasterTransmit( 0x00 );
		L6470_CS_HI();
	}
	
	*puiValue = Data.uiData & Mask;
	
	return true;
}



//-----------------------------------------------------------------------------
// L6470 デバッグ用
//-----------------------------------------------------------------------------
void L6470_Debug( void )
{
	int16_t		iIndex = 0;
	int16_t		iMaxDataNum = sizeof(g_tRegister) / sizeof(L6470_RegisterTable);
	
	
	//memset( g_L6470Debug, 0x00, sizeof(g_L6470Debug) );
	for ( iIndex = 0 ; iIndex < iMaxDataNum ; iIndex++ )
	{
		if ( g_tRegister[iIndex].Address == L6470_END_ADDRESS )
		{
			break;
		}
		
		g_L6470Debug[iIndex].Address = g_tRegister[iIndex].Address;
		L6470_GetParam( g_L6470Debug[iIndex].Address, &g_L6470Debug[iIndex].uiDate );
	}
	
	return;
}
