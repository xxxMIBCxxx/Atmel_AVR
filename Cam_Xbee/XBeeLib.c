#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "util/delay.h"
#include "XBeeLib.h"






//=================================================================================================
// �f�t�@�C����`
//=================================================================================================
#define XBEE_RECV_DATA			( 100 )
#define XBEE_RESET_TIMEOUT		( 3000 )				// XBEE���Z�b�g�^�C���A�E�g(ms)
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
// �\���̒�`
//=================================================================================================
// XBEE ��M�f�[�^�i�[�\����
typedef struct 
{
	uint16_t			wWriteIndex;				// �����݃|�W�V����
	uint16_t			wReadIndex;					// �Ǎ��|�W�V����
	uint16_t			wLength;					// ��M�f�[�^���i�[���Ă���T�C�Y
	char				szData[ XBEE_RECV_DATA ];	// ��M�f�[�^�i�[�̈�

} XBEE_RECV_INFO_TABLE, *PXBEE_RECV_INFO_TABLE;



//=================================================================================================
// �v���g�^�C�v�錾
//=================================================================================================
static void SPI_MasterInit(void);
static char SPI_MasterTransmit( char Data );
static bool xbee_GetRecvData(void);
static bool xbee_CompCheckSum( char *pData, uint16_t wDataSize, unsigned char CheckSum );
static void xbee_SetRecvData( char Data );

//=================================================================================================
// �O���[�o���ϐ�
//=================================================================================================
volatile static XBEE_RECV_INFO_TABLE		g_tXBeeRecvInfo;
volatile static char						g_szXBeeRecvData[ XBEE_RECV_DATA ];		// XBee ��M�f�[�^�擾�p


//=================================================================================================
// SPI�֘A
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// SPI�������i�呕�u�j
//-------------------------------------------------------------------------------------------------
static void SPI_MasterInit(void)
{
#if 0
	SPCR  = _BV(SPE) | _BV(MSTR) | _BV(SPR0);			// SPI����,�呕�u,8����(2.5MHz)
	SPSR  = _BV(SPI2X);
#else
	SPCR  = _BV(SPE) | _BV(MSTR);						// SPI����,�呕�u,8����(2.0MHz)
#endif

//	DDRB  |= SPI_SCK | SPI_MOSI | SPI_SS;				// SPI�Ŏg�p����PIN��ݒ�(�o�́FSCK,MOSI,SS / ���́FMISO) 
//	PORTB |= SPI_SS;										
	DDRB  = SPI_SCK | SPI_MOSI | SPI_SS | XBEE_RESET | XBEE_DOUT;
	PORTB = SPI_SS | XBEE_RESET | XBEE_ATTN | SWITCH_PIN;
	
}


//-------------------------------------------------------------------------------------------------
// SPI�f�[�^���M�i�呕�u�j
//-------------------------------------------------------------------------------------------------
static char SPI_MasterTransmit( char Data )
{
	SPDR = Data;
	
	// �f�[�^���M��������܂ő҂�
	while( !(SPSR & _BV(SPIF)) );
	
	return SPDR;
}


//=================================================================================================
// XBee�֘A
//=================================================================================================
//-------------------------------------------------------------------------------------------------
// XBee������
//-------------------------------------------------------------------------------------------------
bool XBee_Init(void)
{
	bool		bRet = true;
	
	// SPI�������i�呕�u�j
	SPI_MasterInit();
	
	// XBee�Ŏg�p����PIN��ݒ�
	DDRB  |= XBEE_RESET | XBEE_DOUT;
	PORTB = (PORTB & ~XBEE_DOUT) | XBEE_RESET | XBEE_ATTN;
	
	// XBee���Z�b�g
	bRet = XBee_Reset();
		
	return true;
}


//-------------------------------------------------------------------------------------------------
// XBee���Z�b�g
//-------------------------------------------------------------------------------------------------
bool XBee_Reset(void)
{
	bool					bRet = false;
	int						iRet = 0;
	uint16_t				wSize = 0;
	uint16_t				wTimeOut = 0;
	XBEE_API_FRAME_TABLE	tXBeeResApiFrame;
	

	//// XBee�����Z�b�g
	//PORTB &= ~(XBEE_RESET | XBEE_DOUT);
	//_delay_ms( 50 );
	//PORTB |= XBEE_RESET;
	//_delay_ms( 50 );
	
	memset( (void *)&g_tXBeeRecvInfo, 0x00, sizeof(g_tXBeeRecvInfo) );
	
	XBee_RecvWait();
	
	// Modem Status���ʒm�����܂ő҂�
	while( true )
	{
		wSize = XBEE_RECV_DATA;
		bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
		if ( bRet == true )
		{
			// ��M�f�[�^���
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
					break;								// ���[�v�𔲂���
				}
				
				// �Ⴄ�t���[���������i���肦�Ȃ��H�j
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
// XBee�f�[�^��M
//-------------------------------------------------------------------------------------------------
static bool xbee_GetRecvData(void)
{
	bool		bRet = false;
	
	
	// SPI_ATTN��Low�̏ꍇ�A��M�f�[�^�����݂��Ă���̂Ńf�[�^����M
	if ( !(PINB & XBEE_ATTN) )
	{
		SPI_Start();
		do 
		{	
			// �_�~�[�f�[�^�𑗐M���āA��M�f�[�^���擾
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
// XBee��M�f�[�^�擾
//-------------------------------------------------------------------------------------------------
bool XBee_GetRecvData( char *pData, uint16_t *pwSize )
{
	bool			bRet = false;
	uint16_t		wReadSize = 0;
	uint16_t		wIndex = 0;
	
	
	// �Ƃ肠����XBee���̗��܂��Ă���f�[�^��S�Ď�M����
	bRet = xbee_GetRecvData();
	
	// ��M�f�[�^�����݂���H
	if ( g_tXBeeRecvInfo.wLength != 0 )
	{
		if ( *pwSize >= g_tXBeeRecvInfo.wLength )
		{
			*pwSize = g_tXBeeRecvInfo.wLength;
		}
		
		// �擾�f�[�^����M�f�[�^�i�[�̈�𒴂���H
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
// XBee ��M�f�[�^���
// ��XBee ��M�f�[�^��̖͂߂�l��true�̏ꍇ�A��̓f�[�^���s�v�ɂȂ�����PXBEE_API_FRAME_TABLE��
//   pData��free�֐����g�p���ė̈��������Ă�������
// �߂�l�F 0 = ����
//         -1 = API Frame�f�[�^�łȂ�
//         -2 = �T�C�Y�`�F�b�N�G���[
//         -3 = �����������G���[
//         -4 = �`�F�b�N�T���G���[ 
//-------------------------------------------------------------------------------------------------
int XBee_AnalyzeRecvData( char *pData, uint16_t wDataSize, PXBEE_API_FRAME_TABLE ptXBeeApiFrame )
{
	int				iRet = -1;
	bool			bRet = false;
	uint16_t		wIndex = 0;
	
	
	while ( wIndex < wDataSize )
	{
		// Start Delimiter���L�邩���ׂ�
		if ( pData[ wIndex ] == 0x7E )
		{
			ptXBeeApiFrame->StartDelimiter = pData[ wIndex ];								// Start Delimiter
			ptXBeeApiFrame->wLength = ((uint16_t)pData[ wIndex+1 ]) << 8;					// �f�[�^�T�C�Y
			ptXBeeApiFrame->wLength |=  (uint16_t)pData[ wIndex+2 ];
			wIndex = wIndex + 3;
			
			// �T�C�Y�`�F�b�N
			if ( wDataSize < (wIndex + ptXBeeApiFrame->wLength + 1) )
			{
				iRet = -2;
				break;
			}
			
			// �擾�����f�[�^�T�C�Y����i�[�̈�𐶐�
			ptXBeeApiFrame->pszData = (char *)malloc( ptXBeeApiFrame->wLength );
			if ( ptXBeeApiFrame->pszData == NULL )
			{
				iRet = -3;
				break;
			}
			memcpy( ptXBeeApiFrame->pszData, &pData[ wIndex ], ptXBeeApiFrame->wLength );	// ��M�f�[�^
			wIndex = wIndex + ptXBeeApiFrame->wLength;
			ptXBeeApiFrame->CheckSum = pData[ wIndex ];										// �`�F�b�N�T��
			
			// �`�F�b�N�T�����`�F�b�N
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
	
	// ����I���ȊO�̏ꍇ
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
// XBee �`�F�b�N�T������
//-------------------------------------------------------------------------------------------------
unsigned char XBee_CreateCheckSum( char *pData, uint16_t wDataSize )
{
	uint16_t			wIndex = 0;
	unsigned char		Sum = 0x00;
	unsigned char		CheckSum = 0x00;
	
	
	// �f�[�^�����ׂč��v����
	for ( wIndex = 0 ; wIndex < wDataSize ; wIndex++ )
	{
		Sum = Sum + (unsigned char)pData[ wIndex ];
	}
	CheckSum = 0xFF - Sum;
	
	return CheckSum;
}


//-------------------------------------------------------------------------------------------------
// XBee �`�F�b�N�T����r
//-------------------------------------------------------------------------------------------------
static bool xbee_CompCheckSum( char *pData, uint16_t wDataSize, unsigned char CheckSum )
{
	bool				bRet = false;
	unsigned char		TempCheckSum = 0x00;
	
	
	// ��M�f�[�^����`�F�b�N�T���𐶐�
	TempCheckSum = XBee_CreateCheckSum( pData, wDataSize );
	
	// ��r
	bRet = ( TempCheckSum == CheckSum ) ? true : false;
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// XBee ��M�f�[�^���Z�b�g
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
// XBee Api Frame���M
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
	Data = SPI_MasterTransmit( (char)((ptXBeeApiFrame->wLength & 0xFF00) >> 8) );		// �f�[�^�T�C�Y
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );

	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( (char)(ptXBeeApiFrame->wLength & 0x00FF) );
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );
	
	for ( wIndex = 0 ; wIndex < ptXBeeApiFrame->wLength ; wIndex++ )					// �f�[�^
	{
		bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
		Data = SPI_MasterTransmit( ptXBeeApiFrame->pszData[ wIndex ] );
		if ( bRecvFlag == true ) xbee_SetRecvData( Data );
	}
	
	bRecvFlag = ( !(PINB & (XBEE_ATTN)) ) ? true : false;
	Data = SPI_MasterTransmit( ptXBeeApiFrame->CheckSum );								// �`�F�b�N�T��
	if ( bRecvFlag == true ) xbee_SetRecvData( Data );

	SPI_End();

	if ( ptXBeeResApiFrame != NULL )
	{
		XBee_RecvWait();
		
		while ( 1 )
		{
			// �f�[�^����M
			wSize = XBEE_RECV_DATA;
			bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
			if ( bRet == true )
			{
				// Api Frame���
				bRet = XBee_AnalyzeRecvData( (char *)g_szXBeeRecvData, wSize, ptXBeeResApiFrame );
				if ( iRet == 0)
				{
					bRet = true;
					break;
				}
			}
			
			// �f�[�^��M�^�C���A�E�g�H
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
// XBee Wi-Fi�ڑ��҂�
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
		// �f�[�^����M
		wSize = XBEE_RECV_DATA;
		bRet = XBee_GetRecvData( (char *)g_szXBeeRecvData, &wSize );
		if ( bRet == true )
		{
			// Api Frame���
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
			
		// �f�[�^��M�^�C���A�E�g�H
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