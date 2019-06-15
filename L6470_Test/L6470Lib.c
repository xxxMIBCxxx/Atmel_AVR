
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
	unsigned char Address;				// ���W�X�^�A�h���X
	uint32_t BitLength;					// ����(Bit)
	uint32_t Default;					// #12026�̐ݒ�l(0xFFFFFFFF:�ݒ肵�Ȃ�)
	bool bReadOnly;						// �ǂݎ���p(TRUE:�ǂݎ���p)
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
	{ L6470_RUN			, 20, 0xFFFFFFFF, true	},			// ����]
	{ L6470_R_RUN		, 20, 0xFFFFFFFF, true	},			// �t��]
	{ L6470_STOP		,  0, 0xFFFFFFFF, true	},			// ��~
	{ L6470_END_ADDRESS	,  0, 0xFFFFFFFF, false },			// �I�[
};




//-----------------------------------------------------------------------------
// L6470 ����������
//-----------------------------------------------------------------------------
void L6470_Init()
{
	int16_t		iIndex = 0;
	int16_t		iMaxDataNum = sizeof(g_tRegister) / sizeof(L6470_RegisterTable);
	

	// SPI������(5MHz)
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
// L6470 �p�����[�^�ݒ�
//-----------------------------------------------------------------------------
bool L6470_SetParam( unsigned char Address, uint32_t uiValue )
{
	uint16_t TransmitByte = 0;
	uint16_t index = 0;
	uint32_t Mask = 0x00000000;
	L6470_ParamData	Data;
	
	// �w�肳�ꂽ�A�h���X�����݂��邩���ׂ�
	index = 0;
	while ( 1 )
	{
		// �I�[?
		if ( g_tRegister[index].Address == L6470_END_ADDRESS )
		{
			return false;
		}
		
		// �A�h���X��v�H
		if ( g_tRegister[index].Address == Address )
		{
			break;
		}
		
		index++;
	}
	
	// �ǂݎ���p�H
	if ( g_tRegister[index].bReadOnly == true )
	{
		return false;
	}	

	// �]���o�C�g���Z�o
	TransmitByte = g_tRegister[index].BitLength / 8;
	TransmitByte += ((g_tRegister[index].BitLength % 8)!=0) ? 1 : 0;
	
	// �}�X�N�f�[�^����
	for ( uint16_t i = 0 ; i < g_tRegister[index].BitLength ; i++ )
	{
		Mask = Mask << 1;
		Mask = Mask | 0x00000001;
	}
	
	Data.uiData = uiValue & Mask;
	
	// �A�h���X���M	
	L6470_CS_LOW();
	SPI_MasterTransmit( Address );
	L6470_CS_HI();	
	
	// �p�����[�^���M
	for ( uint16_t i = TransmitByte ; i > 0 ; i-- )
	{
		L6470_CS_LOW();
		SPI_MasterTransmit( Data.Data[i-1] );
		L6470_CS_HI();			
	}
	
	return true;
}


//-----------------------------------------------------------------------------
// L6470 �p�����[�^�擾
//-----------------------------------------------------------------------------
bool L6470_GetParam( unsigned char Address, uint32_t *puiValue )
{
	uint16_t TransmitByte = 0;
	uint16_t index = 0;
	uint32_t Mask = 0x00000000;
	L6470_ParamData	Data;
	
	
	Data.uiData = 0;
	
	// �w�肳�ꂽ�A�h���X�����݂��邩���ׂ�
	index = 0;
	while ( 1 )
	{
		// �I�[?
		if ( g_tRegister[index].Address == L6470_END_ADDRESS )
		{
			return false;
		}
		
		// �A�h���X��v�H
		if ( g_tRegister[index].Address == Address )
		{
			break;
		}
		
		index++;
	}
		
	// �}�X�N�f�[�^����
	for ( uint16_t i = 0 ; i < g_tRegister[index].BitLength ; i++ )
	{
		Mask = Mask << 1;
		Mask = Mask | 0x00000001;
	}

	// �]���o�C�g���Z�o
	TransmitByte = g_tRegister[index].BitLength / 8;
	TransmitByte += ((g_tRegister[index].BitLength % 8)!=0) ? 1 : 0;

	// �A�h���X���M	
	L6470_CS_LOW();
	SPI_MasterTransmit( (L6470_READ_ADDRESS | Address) );
	L6470_CS_HI();	
	
	// �p�����[�^�擾
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
// L6470 �f�o�b�O�p
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
