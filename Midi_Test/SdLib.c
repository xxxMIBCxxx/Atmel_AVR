#include "SdLib.h"
#include "SpiLib.h"
#include <util/delay.h>




// SD�J�[�h�̃R�}���h
#define SD_CMD00			( 0 )			// �J�[�h�����Z�b�g����
#define SD_CMD01			( 1 )			// ���Z�b�g���I�����������肷��
#define SD_CMD16			( 16 )			// �u���b�N�����w�肷��
#define SD_CMD17			( 17 )			// �u���b�N�����Ǎ���
#define SD_CMD24			( 24 )			// �u���b�N����������

const unsigned char DUMMY_SEND_DATA[] = { 0xFF, 0x00 }; 
const unsigned char DUMMY_CRC_DATA[] = { 0xFF, 0xFF };
const unsigned char WRITE_START_DATA[] = { 0xFE, 0x00 };

// �v���g�^�C�v�錾
static bool SD_SendCommand( bool bDummyData, unsigned char Cmd, uint32_t arg, unsigned char *pResponse );
static void SD_SendDummyData();

// �O���[�o���ϐ�
uint32_t g_uiBlockSize = 0;					// �u���b�N���̕ێ��p


//-------------------------------------------------------------------------------------------------
// SD����������
//-------------------------------------------------------------------------------------------------
bool SD_Init()
{
	bool bFlag = false;
	bool bRet = false;
	unsigned char Response = 0x00;
	
	
	// SPI�������i��SD���������I���܂ŁASPI Clock��400KHz�ȉ��ɂ��Ă����j
	SPI_SetClockRate(SPI_CLOCK_RATE128);
	
	// �_�~�[�f�[�^���M
	SD_SendDummyData();
	
	// SD�J�[�h���Z�b�g
	bRet = SD_SendCommand( false, SD_CMD00, 0, &Response );
	if ( bRet == false )
	{
		return false;
	}
	// �����́u�J�[�h���������v�̏ꍇ
	if ( Response == 0x01 )
	{
		bFlag = false;
		for ( int16_t i = 0 ; i < 1000 ; i++ )
		{
			// SD�J�[�h�����������₢���킹
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
			
			// �����́u�����������v�ȊO�̏ꍇ
			if ( Response != 0x00 )
			{
				return false;
			}
			
			// ���[�v�𔲂���
			bFlag = true;
			break;
		}
		if ( bFlag = false )
		{
			return false;	
		}
	}
	
	// SPI�̒ʐM���x��߂�
	SPI_SetClockRate(SPI_CLOCK_RATE8);

	return true;	
}


//-------------------------------------------------------------------------------------------------
// �u���b�N����ݒ�
//-------------------------------------------------------------------------------------------------
bool SD_SetBlockSize( uint32_t uiBlockSize )
{
	bool bRet = false;
	unsigned char Response = 0x00;
	
	// �u���b�N����ݒ�
	bRet = SD_SendCommand( false, SD_CMD16, uiBlockSize, &Response );
	if ( bRet == false )
	{
		return false;
	}
	
	// �������m�F
	if ( Response != 0x00 )
	{
		return false;
	}
	
	// �����Ńu���b�N����ێ�
	g_uiBlockSize = uiBlockSize;
	
	return true;
}


//-------------------------------------------------------------------------------------------------
// �f�[�^�Ǎ�����
//-------------------------------------------------------------------------------------------------
bool SD_ReadBlockData( uint32_t uiAddress, unsigned char *pReadData )
{
	bool bFlag = false;
	bool bRet = false;
	unsigned char Response = 0x00;
	unsigned char CRC[2];
	unsigned char SendDummyData = 0xFF;
	
	// �܂��u���b�N����ݒ肵�Ă��Ȃ��ꍇ
	if ( g_uiBlockSize == 0 )
	{
		return false;
	}
	
	// �p�����[�^�`�F�b�N
	if ((uiAddress % g_uiBlockSize) != 0 )
	{
		return false;
	}
	
	// �Ǎ��R�}���h���s
	bRet = SD_SendCommand( false, SD_CMD17, uiAddress, &Response );
	if ((bRet == false) && (Response != 0x00))
	{
		return false;
	}
	
	// �Ǎ��J�n(0xFE)�̉����҂�
	SdCS_LOW;
	bFlag = false;
	for ( uint32_t i = 0 ; i < 1000 ; i++ )
	{
		SPI_MasterRecv( &Response, 1);

		if ( Response == 0xFE )
		{
			// �u���b�N�����f�[�^�Ǎ�
			SPI_MasterRecv( pReadData, g_uiBlockSize );
	
			// CRC(2�o�C�g)��Ǎ���
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
		
	// �_�~�[�f�[�^���M(���̃R�}���h����t�ł���悤�ɂ���)
	SPI_MasterSend( DUMMY_SEND_DATA, 1 );
	SdCS_HI;
	
	return bFlag;
}


//-------------------------------------------------------------------------------------------------
// �f�[�^�����ݏ���
//-------------------------------------------------------------------------------------------------
bool SD_WriteBlockData( uint32_t uiAddress, unsigned char *pWriteData )
{
	bool bRet = false;
	bool bFlag = false;
	unsigned char Response = 0x00;
	unsigned char SendDummyData = 0xFF;
	
	
	// �܂��u���b�N����ݒ肵�Ă��Ȃ��ꍇ
	if ( g_uiBlockSize == 0 )
	{
		return false;
	}
	
	// �p�����[�^�`�F�b�N
	if ((uiAddress % g_uiBlockSize) != 0 )
	{
		return false;
	}
	
	// �����݃R�}���h���s
	bRet = SD_SendCommand( false, SD_CMD24, uiAddress, &Response );
	if ((bRet == false) &&  (Response != 0x00))
	{
		return false;
	}

	// �Ǎ��J�n�R�}���h
	SdCS_LOW;
	SPI_MasterSend( WRITE_START_DATA, 1 );					// 0xFE
	
	// �f�[�^������
	SPI_MasterSend( pWriteData, g_uiBlockSize );			// WriteData

	// CRC���M
	SPI_MasterSend( DUMMY_CRC_DATA, 2 );					// CRC(0xFF,0xFF)
	
	// ������M
	bFlag = false;
	SPI_MasterRecv( &Response, 1);
	if ( (Response & 0x0F) == 0x05 )
	{
		// BUSY(0x00)�����҂�
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

	// �_�~�[�f�[�^���M(���̃R�}���h����t�ł���悤�ɂ���)
	SPI_MasterSend( DUMMY_SEND_DATA, 1 );
	SdCS_HI;

	
	return bFlag;
}


//-------------------------------------------------------------------------------------------------
// �R�}���h���s
//-------------------------------------------------------------------------------------------------
static bool SD_SendCommand( bool bDummyData, unsigned char Cmd, uint32_t arg, unsigned char *pResponse )
{
	bool bRet = false;
	unsigned char SendData[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
	unsigned char DummySendData = 0xFF;
		
	// �R�}���h
	SendData[0] = 0x40 | (Cmd & 0x3F);
	
	// �p�����[�^
	SendData[1] = (unsigned char)(arg >> 24);
	SendData[2] = (unsigned char)(arg >> 16);
	SendData[3] = (unsigned char)(arg >>  8);
	SendData[4] = (unsigned char)arg;
	
	// CRC7
	if ( Cmd == SD_CMD00 ) SendData[5] = 0x95;
	if ( Cmd == SD_CMD01 ) SendData[5] = 0xF9;
	
	
	// ���M�O�Ƀ_�~�[�f�[�^�𑗐M����
	if ( bDummyData == true )
	{
		SdCS_HI;
		SPI_MasterSend( &DummySendData, 1 );
	}	
	
	SdCS_LOW;
	
	// ���s
	SPI_MasterSend( SendData, 6 );
	
	// ������҂�
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
// �_�~�[�f�[�^���M����
// ���}�X�^����SD�J�[�h��80�N���b�N�̃_�~�[�N���b�N�𑗂�
//-------------------------------------------------------------------------------------------------
static void SD_SendDummyData()
{
	unsigned char SendData = 0xFF;
	
	// SD SC��LOW�ɂ���
	SdCS_LOW;

	// 80�N���b�N(10�o�C�g)���̃_�~�[�f�[�^�𑗐M
	for ( int16_t i = 0 ; i < 10 ; i++ )
	{
		SPI_MasterSend( &SendData, 1 );
	}
	_delay_us(500);
	
	return;
}
