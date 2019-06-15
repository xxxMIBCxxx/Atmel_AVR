//=================================================================================================
// I2C���C�u����
//=================================================================================================
#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "I2C_Lib.h"



//---< ���ʂ̏�ԕ��� >---------------------------------------------------------------------------
#define TWSR_START					( 0x08 )	// �J�n�������M
#define TWSR_RESTART				( 0x10 )	// �đ��J�n�������M
#define TWSR_CONFLICT				( 0x38 )	// �o�X����
//---< ���M�呕�u����̏�ԕ��� >------------------------------------------------------------------
#define TWSR_SEND_SLA_W_ACK			( 0x18 )	// SLA+W���MACK��M
#define TWSR_SEND_SLA_W_NACK		( 0x20 )	// SLA+W���MNACK��M
#define TWSR_SEND_DATA_ACK			( 0x28 )	// �f�[�^�o�C�g���MACK��M
#define TWSR_SEND_DATA_NACK			( 0x30 )	// �f�[�^�o�C�g���MNACK��M
//---< ��M�呕�u����̏�ԕ��� >------------------------------------------------------------------
#define TWSR_SEND_SLA_R_ACK			( 0x40 )	// SLA+R���MACK��M
#define TWSR_SEND_SLA_R_NACK		( 0x48 )	// SLA+R���MNACK��M
#define TWSR_RECV_DATA_ACK			( 0x50 )	// �f�[�^�o�C�g��MACK����
#define TWSR_RECV_DATA_NACK			( 0x58 )	// �f�[�^�o�C�g��MNACK����



//-------------------------------------------------------------------------------------------------
// ����������
//-------------------------------------------------------------------------------------------------
void I2C_Init(void)
{
	// �d�͍팸���W�X�^��2������C���^�[�t�F�[�X�팸(Power Reduction TWI)������
	PRR = PRR & ~(_BV(PRTWI));

#if 0	
	// SCL���g���ݒ�(400KHz)
	TWSR = 0b00000000;											// TWI�r�b�g���x�O�u������I��(�����l:1)
	TWBR = 17;													// TWI�r�b�g���x���W�X�^(17)
#else
	// SCL���g���ݒ�(400KHz)
	TWSR = 0b00000000;											// TWI�r�b�g���x�O�u������I��(�����l:1)
	TWBR = 20;													// TWI�r�b�g���x���W�X�^(17)
#endif	
	
	
	
	return;
}

//-------------------------------------------------------------------------------------------------
// �����݁i�呕�u����j
//-------------------------------------------------------------------------------------------------
bool I2C_Master_Write( unsigned char Slave, unsigned char Data )
{
	bool			bRet = false;
	//unsigned char	i = 0;
	
	
	// �J�n�������o
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	
	// SLA+W���M
	TWDR = Slave | 0x00;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_W_ACK )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	
	// �f�[�^���M
	TWDR = Data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	bRet = true;
	
I2C_MASTER_WRITE_END_PROC:	
	// ��~�������o
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// �Ǎ��݁i�呕�u����j
//-------------------------------------------------------------------------------------------------
bool I2C_Master_Read( unsigned char Slave, unsigned char *pData )
{
	bool		bRet = false;

	
	// �J�n�������o
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto I2C_MASTER_READ_END_PROC;
	}
	
	// SLA+R���M
	TWDR = Slave | 0x01;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_R_ACK )
	{
		goto I2C_MASTER_READ_END_PROC;
	}
	
	// �f�[�^��M
//	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_RECV_DATA_NACK )
	{
		goto I2C_MASTER_READ_END_PROC;
	}
	*pData = TWDR;
	bRet = true;
	
I2C_MASTER_READ_END_PROC:	
	// ��~�������o
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// SCCB ������
//-------------------------------------------------------------------------------------------------
bool SCCB_Write( unsigned char Slave, unsigned char addr, unsigned char Data )
{
	bool			bRet = false;

	
	// �J�n�������o
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto SCCB_WRITE_END_PROC;
	}
	
	// SLA+W���M
	TWDR = Slave | 0x00;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_W_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}
	
	// �A�h���X�Z�b�g
	TWDR = addr;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}

	// �f�[�^������
	TWDR = Data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}

	bRet = true;
	
SCCB_WRITE_END_PROC:	
	// ��~�������o
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// SCCB �Ǎ���
//-------------------------------------------------------------------------------------------------
bool SCCB_Read( unsigned char Slave, unsigned char addr, unsigned char *pData )
{
	bool			bRet = false;
	
	
	// �A�h���X�Z�b�g
	bRet = I2C_Master_Write( Slave, addr );
	if ( bRet == true )
	{
		bRet = I2C_Master_Read( Slave, pData );
	}
	
	return bRet;
}