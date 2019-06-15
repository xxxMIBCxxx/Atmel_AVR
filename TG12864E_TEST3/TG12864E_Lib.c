#include <avr/io.h>
#include <util/delay.h>
#include "stdbool.h"
#include "TG12864E_Lib.h"


void TG12864E_Write( TG12864_CS_ENUM eCS, bool bRS, bool bRW, unsigned char Data );
void TG12864E_Read( TG12864_CS_ENUM eCS, bool bRS, bool bRW, unsigned char *pData );
void TG12864E_Wait( TG12864_CS_ENUM eCS );

//=============================================================================
// TG12864E����������
//=============================================================================
void TG12864E_init()
{
	TG12864E_RS_LOW();
	TG12864E_RW_LOW();
	TG12864E_E_LOW();

	// ���Z�b�g
	TG12864E_Reset();

	// �d��ON����30ms�ȏ�҂�
	_delay_ms(30);

	// CS1
	TG12864E_DisplayStartLine( CS1, 0 );				// Set Display Start Line
	TG12864E_DisplayOn( CS1, true );					// Set Display ON

	// CS2
	TG12864E_DisplayStartLine( CS2, 0 );				// Set Display Start Line
	TG12864E_DisplayOn( CS2, true );					// Set Display ON

	// ��ʃN���A
	TG12864E_Fill( 0x00 );

	return;
}


//=============================================================================
// TG12864E ���Z�b�g����
//=============================================================================
void TG12864E_Reset()
{

	// ���Z�b�gPIN��LOW
	TG12864E_RST_LOW();
	_delay_us(10);

	// ���Z�b�gPIN��HI
	TG12864E_RST_HI();
	_delay_us(200);

	return;
}


//=============================================================================
// TG12864E_DisplayOn
//=============================================================================
void TG12864E_DisplayOn( TG12864_CS_ENUM eCS, bool bOn )
{
	unsigned Data = 0b00111110;
	
	// Display ON ?
	if ( bOn == true )
	{
		Data = Data | 0b00000001;
	}
	
	// Busy�����҂�
	//TG12864E_Wait( eCS );
	
	// Display ON/OFF(RS:0 RW:0, Data)
	TG12864E_Write( eCS, false, false, Data );
	
	return;	
}


//=============================================================================
// TG12864E_SetAddress
//=============================================================================
void TG12864E_SetAddress( TG12864_CS_ENUM eCS, unsigned char Address )
{
	unsigned Data = 0b01000000;
	
	// �p�����[�^�`�F�b�N
	if ( Address > 63 ) return;
	Data = Data | Address;

	// Busy�����҂�
	//TG12864E_Wait( eCS );
	
	// Set Display Address(RS:0 RW:0, Data)
	TG12864E_Write( eCS, false, false, Data );
	
	return;	
}


//=============================================================================
// TG12864E_SetPage
//=============================================================================
void TG12864E_SetPage( TG12864_CS_ENUM eCS, unsigned char Page )
{
	unsigned Data = 0b10111000;
	
	// �p�����[�^�`�F�b�N
	if ( Page > 7 ) return;
	Data = Data | Page;

	// Busy�����҂�
	//TG12864E_Wait( eCS );
	
	// Set Page(RS:0 RW:0, Data)
	TG12864E_Write( eCS, false, false, Data );
	
	return;	
}


//=============================================================================
// TG12864E_DisplayStartLine
//=============================================================================
void TG12864E_DisplayStartLine( TG12864_CS_ENUM eCS, unsigned char Line )
{
	unsigned Data = 0b11000000;
	
	// �p�����[�^�`�F�b�N
	if ( Line > 63 ) return;
	Data = Data | ( 0b00111111 & Line );

	// Busy�����҂�
	//TG12864E_Wait( eCS );

	// Display Start Line(RS:0 RW:0, Data)
	TG12864E_Write( eCS, false, false, Data );
	
	return;	
}


//=============================================================================
// TG12864E_StatusRead
//=============================================================================
void TG12864E_StatusRead( TG12864_CS_ENUM eCS, unsigned char *pStatus )
{
	// �p�����[�^�`�F�b�N
	if ( pStatus == 0 ) return;

	// Status Read(RS:0 RW:1, pData)
	TG12864E_Read( eCS, false, true, pStatus );
	
	return;
}


//=============================================================================
// TG12864E_WriteDisplayData
//=============================================================================
void TG12864E_WriteDisplayData( TG12864_CS_ENUM eCS, unsigned char Data )
{
	// Busy�����҂�
	//TG12864E_Wait( eCS );	
	
	// Write Display Data(RS:1 RW:0, Data)
	TG12864E_Write( eCS, true, false, Data );

	return;
}


//=============================================================================
// TG12864E_ReadDisplayData
//=============================================================================
void TG12864E_ReadDisplayData( TG12864_CS_ENUM eCS, unsigned char *pData )
{
	// �p�����[�^�`�F�b�N
	if ( pData == 0 ) return;

	// Busy�����҂�
	//TG12864E_Wait( eCS );
	
	// Write Display Data(RS:1 RW:1, pData)
	TG12864E_Read( eCS, true, true, pData );
	
	return;
}


//=============================================================================
// TG12864E Write
//=============================================================================
void TG12864E_Write( TG12864_CS_ENUM eCS, bool bRS, bool bRW, unsigned char Data )
{
	uint16_t	i = 0;
	
	//����
	TG12864E_E_LOW();
	_delay_us(TG12864E_DELAY);
	
	// RS�ERW�ݒ�
	if ( eCS == CS1 )
	{
		TG12864E_CS1_HI();
	}
	else
	{
		TG12864E_CS2_HI();
	}

	if ( bRS ) TG12864E_RS_HI();
	if ( bRW ) TG12864E_RW_HI();
	_delay_us(TG12864E_DELAY);

	// �L��
	TG12864E_E_HI();
	_delay_us(TG12864E_DELAY);

	// �f�[�^�Z�b�g
	PORTA = Data;
	_delay_us(TG12864E_DELAY);

	//����
	TG12864E_E_LOW();
	_delay_us(TG12864E_DELAY);
	
	// RS�ERW����
	TG12864E_RS_LOW();
	TG12864E_RW_LOW();
	TG12864E_CS1_LOW();
	TG12864E_CS2_LOW();
	PORTA = 0b00000000;
	//_delay_us(TG12864E_DELAY);
	//
	//// �L��
	//TG12864E_E_HI();
	//_delay_us(TG12864E_DELAY);
	//TG12864E_E_LOW();
	////_delay_us(TG12864E_DELAY);



	return;
}


//=============================================================================
// TG12864E Read
//=============================================================================
void TG12864E_Read( TG12864_CS_ENUM eCS, bool bRS, bool bRW, unsigned char *pData )
{
	uint16_t	i = 0;
	
	// PIN����͂ɂ���
	DDRA = 0b00000000;
	PORTA = 0b00000000;
	*pData = 0b00000000;

	//����
	TG12864E_E_LOW();
	_delay_us(TG12864E_DELAY);
		
	// RS�ERW�ݒ�
	if ( eCS == CS1 )
	{
		TG12864E_CS1_HI();
	}
	else
	{
		TG12864E_CS2_HI();
	}

	if ( bRS ) TG12864E_RS_HI();
	if ( bRW ) TG12864E_RW_HI();
	_delay_us(TG12864E_DELAY);
	
	// �L��
	TG12864E_E_HI();
	//_delay_us(TG12864E_DELAY);
	
	// �f�[�^�Ǎ�
	*pData = PINA;
	_delay_us(TG12864E_DELAY);

	//����
	TG12864E_E_LOW();
	_delay_us(TG12864E_DELAY);

	// RS�ERW����
	TG12864E_RS_LOW();
	TG12864E_RW_LOW();
	TG12864E_CS1_LOW();
	TG12864E_CS2_LOW();

	// PIN���o�͂ɖ߂�
	DDRA = TG12864E_DB0 | TG12864E_DB1 | TG12864E_DB2 | TG12864E_DB3 | TG12864E_DB4 | TG12864E_DB5 | TG12864E_DB6 | TG12864E_DB7;
	PORTA = 0b00000000;
	_delay_us(TG12864E_DELAY);
		
	return;
}


//=============================================================================
// TG12864E Wait
//=============================================================================
void TG12864E_Wait( TG12864_CS_ENUM eCS )
{
	unsigned char Status = 0x00;
	bool bFlag = false;
	
	// Busy�����҂�
	while ( 1 )
	{
		// ��ԓǍ�
		TG12864E_StatusRead( eCS, &Status );
		
		if ( !(Status & 0b10000000) )
		{
			bFlag = true;
			break;
		}		
	}			

	return;	
}

//=============================================================================
// TG12864E_Fill
//=============================================================================
void TG12864E_Fill(	unsigned char Data )
{
	uint16_t i = 0;
	uint16_t j = 0;
	
	
	// �h��Ԃ�����
	for ( j = 0 ; j < 8 ; j++ )
	{
		TG12864E_SetPage( CS1, j );
		TG12864E_SetAddress( CS1, 0 );

		for ( i = 0 ; i < TG12864E_DISP_WIDTH ; i ++ )
		{
			TG12864E_WriteDisplayData( CS1, Data );
		}	
		
		TG12864E_SetPage( CS2, j );
		TG12864E_SetAddress( CS2, 0 );

		for ( i = 0 ; i < TG12864E_DISP_WIDTH ; i ++ )
		{
			TG12864E_WriteDisplayData( CS2, Data );
		}	
	}
}


//=============================================================================
// TG12864E_Pixel
//=============================================================================
void TG12864E_Pixel( uint16_t x, uint16_t y )
{
	TG12864_CS_ENUM	eCS = CS1;
	unsigned char Data = 0x00;
	unsigned char Line = 0;
	
	// �p�����[�^�`�F�b�N
	if ( (x >= (TG12864E_DISP_WIDTH * 2)) || (y >= TG12864E_DISP_HEIGHT) ) return;
	
	// �����݈ʒu��ݒ�
	TG12864E_Locate( x, y, &eCS );
	
	// �����݈ʒu�̕\�����e���擾
	TG12864E_ReadDisplayData( eCS, &Data );
	
	// ������
	Line = 0b00000001 << (y % 8);
	Data = Data ^ Line;
	
	TG12864E_Locate( x, y, &eCS );
	TG12864E_WriteDisplayData( eCS, Data );
	
	return;
}

//=============================================================================
// TG12864E_Locate
//=============================================================================
void TG12864E_Locate( uint16_t x, uint16_t y, TG12864_CS_ENUM *pCS )
{
	TG12864_CS_ENUM	eCS = CS1;
	unsigned char	Page = 0;
	
	// �p�����[�^�`�F�b�N
	if (x >= (TG12864E_DISP_WIDTH * 2))
	{
		x = TG12864E_DISP_WIDTH - 1;
	}
	
	if (y >= TG12864E_DISP_HEIGHT) 
	{
		y = TG12864E_DISP_HEIGHT - 1;
	}
	
	// CS1 or CS2
	if ( x >= TG12864E_DISP_WIDTH )
	{
		eCS = CS2;
		x = x - TG12864E_DISP_WIDTH;
	}	
	
	// �y�[�W�����߂�
	Page = y / 8;
	
	// �ʒu��ݒ�
	TG12864E_SetPage( eCS, Page );
	TG12864E_SetAddress( eCS, x );
	
	*pCS = eCS;
	
	return;
}