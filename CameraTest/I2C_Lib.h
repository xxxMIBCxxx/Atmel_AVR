//=================================================================================================
// I2Cライブラリ ヘッダーファイル
//=================================================================================================
#ifndef __I2C_LIB_H__
#define __I2C_LIB_H__

#include <avr/io.h>
#include <stdbool.h>

//-------------------------------------------------------------------------------------------------
// I2C初期化処理
//-------------------------------------------------------------------------------------------------
void I2C_Init(void);


//-------------------------------------------------------------------------------------------------
// 書込み（主装置動作）
//-------------------------------------------------------------------------------------------------
//bool I2C_Master_Write( unsigned char Slave, unsigned char *pData, unsigned char DataSize );
bool I2C_Master_Write( unsigned char Slave, unsigned char Data );


//-------------------------------------------------------------------------------------------------
// 読込み（主装置動作）
//-------------------------------------------------------------------------------------------------
bool I2C_Master_Read( unsigned char Slave, unsigned char *pData );


//-------------------------------------------------------------------------------------------------
// SCCB 書込み
//-------------------------------------------------------------------------------------------------
bool SCCB_Write( unsigned char Slave, unsigned char addr, unsigned char Data );


//-------------------------------------------------------------------------------------------------
// SCCB 読込み
//-------------------------------------------------------------------------------------------------
bool SCCB_Read( unsigned char Slave, unsigned char addr, unsigned char *pData );


#endif	// #ifndef __I2C_LIB_H__