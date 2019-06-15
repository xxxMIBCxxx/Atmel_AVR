//=================================================================================================
// I2Cライブラリ
//=================================================================================================
#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "I2C_Lib.h"



//---< 共通の状態符号 >---------------------------------------------------------------------------
#define TWSR_START					( 0x08 )	// 開始条件送信
#define TWSR_RESTART				( 0x10 )	// 再送開始条件送信
#define TWSR_CONFLICT				( 0x38 )	// バス競合
//---< 送信主装置動作の状態符号 >------------------------------------------------------------------
#define TWSR_SEND_SLA_W_ACK			( 0x18 )	// SLA+W送信ACK受信
#define TWSR_SEND_SLA_W_NACK		( 0x20 )	// SLA+W送信NACK受信
#define TWSR_SEND_DATA_ACK			( 0x28 )	// データバイト送信ACK受信
#define TWSR_SEND_DATA_NACK			( 0x30 )	// データバイト送信NACK受信
//---< 受信主装置動作の状態符号 >------------------------------------------------------------------
#define TWSR_SEND_SLA_R_ACK			( 0x40 )	// SLA+R送信ACK受信
#define TWSR_SEND_SLA_R_NACK		( 0x48 )	// SLA+R送信NACK受信
#define TWSR_RECV_DATA_ACK			( 0x50 )	// データバイト受信ACK応答
#define TWSR_RECV_DATA_NACK			( 0x58 )	// データバイト受信NACK応答



//-------------------------------------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------------------------------------
void I2C_Init(void)
{
	// 電力削減レジスタの2線直列インターフェース削減(Power Reduction TWI)を許可
	PRR = PRR & ~(_BV(PRTWI));

#if 0	
	// SCL周波数設定(400KHz)
	TWSR = 0b00000000;											// TWIビット速度前置分周器選択(分周値:1)
	TWBR = 17;													// TWIビット速度レジスタ(17)
#else
	// SCL周波数設定(400KHz)
	TWSR = 0b00000000;											// TWIビット速度前置分周器選択(分周値:1)
	TWBR = 20;													// TWIビット速度レジスタ(17)
#endif	
	
	
	
	return;
}

//-------------------------------------------------------------------------------------------------
// 書込み（主装置動作）
//-------------------------------------------------------------------------------------------------
bool I2C_Master_Write( unsigned char Slave, unsigned char Data )
{
	bool			bRet = false;
	//unsigned char	i = 0;
	
	
	// 開始条件送出
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	
	// SLA+W送信
	TWDR = Slave | 0x00;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_W_ACK )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	
	// データ送信
	TWDR = Data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto I2C_MASTER_WRITE_END_PROC;
	}
	bRet = true;
	
I2C_MASTER_WRITE_END_PROC:	
	// 停止条件送出
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// 読込み（主装置動作）
//-------------------------------------------------------------------------------------------------
bool I2C_Master_Read( unsigned char Slave, unsigned char *pData )
{
	bool		bRet = false;

	
	// 開始条件送出
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto I2C_MASTER_READ_END_PROC;
	}
	
	// SLA+R送信
	TWDR = Slave | 0x01;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_R_ACK )
	{
		goto I2C_MASTER_READ_END_PROC;
	}
	
	// データ受信
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
	// 停止条件送出
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// SCCB 書込み
//-------------------------------------------------------------------------------------------------
bool SCCB_Write( unsigned char Slave, unsigned char addr, unsigned char Data )
{
	bool			bRet = false;

	
	// 開始条件送出
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_START )
	{
		goto SCCB_WRITE_END_PROC;
	}
	
	// SLA+W送信
	TWDR = Slave | 0x00;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_SLA_W_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}
	
	// アドレスセット
	TWDR = addr;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}

	// データ書込み
	TWDR = Data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while( !(TWCR & _BV(TWINT)) );
	if ( (TWSR & 0xF8) != TWSR_SEND_DATA_ACK )
	{
		goto SCCB_WRITE_END_PROC;
	}

	bRet = true;
	
SCCB_WRITE_END_PROC:	
	// 停止条件送出
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	
	return bRet;
}


//-------------------------------------------------------------------------------------------------
// SCCB 読込み
//-------------------------------------------------------------------------------------------------
bool SCCB_Read( unsigned char Slave, unsigned char addr, unsigned char *pData )
{
	bool			bRet = false;
	
	
	// アドレスセット
	bRet = I2C_Master_Write( Slave, addr );
	if ( bRet == true )
	{
		bRet = I2C_Master_Read( Slave, pData );
	}
	
	return bRet;
}