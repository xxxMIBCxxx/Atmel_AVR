#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"
#include "Sensor.h"
#include "Sensor_config.h"
#include "I2C_Lib.h"




bool Sensor_Init( void )
{
	bool			bRet = false;
	unsigned char	i = 0;
	
	// SCCB初期化
	I2C_Init();
	
	// OV7670設定
	i = 0;
	while( 1 )
	{
		// END MARKER?
		if ( (OV7670_reg[i].reg_num == 0xFF) && (OV7670_reg[i].value == 0xFF) )
		{
			break;
		}
		
		bRet = SCCB_Write( OV7670_I2C_ADDR, OV7670_reg[i].reg_num, OV7670_reg[i].value );
		if ( bRet == false )
		{
			return false;
		}
		
		// デフォルト設定待ち
		if ( i == 0 ) _delay_ms(50);
		i++;
	}
	
	return true;	
}


bool SetReg_AEC( uint16_t wValue )
{
	bool			bRet = false;
	unsigned char	Temp = 0x00;
	unsigned char	Aechh = 0x00;
	unsigned char	Aech  = 0x00;
	unsigned char	Com1  = 0x00;


	// 一度OFFにする
	bRet = EnableAEC( false );
	if ( bRet == false ) return false;
	//_delay_ms(10);

	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_COM1, &Com1 );
	if ( bRet == false ) return false;
	
	
	// AEC[15:10](AECHH[5:0])
	Aechh = (unsigned char)((wValue & 0xFC00) >> 10);
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_AECHH, Aechh );
	if ( bRet == false ) return false;
	
	// AEC[9:2](AECH[7:0])
	Aech = (unsigned char)((wValue & 0x03FC) >> 2);
	SCCB_Write( OV7670_I2C_ADDR, REG_AECH, Aech );
	if ( bRet == false ) return false;

	// AEC[1:0](COM1[1:0])
	Temp = (unsigned char)(wValue & 0x0003);
	Com1 = (Com1 & 0xFC) | Temp;
	SCCB_Write( OV7670_I2C_ADDR, REG_COM1, Com1 );
	if ( bRet == false ) return false;
	//_delay_ms(10);

	// ONにする
	bRet = EnableAEC( true );
	if ( bRet == false ) return false;
	//_delay_ms(10);


	return true;
}


bool SetReg_GAIN( uint16_t wValue )
{
	bool			bRet = false;
	unsigned char	Temp = 0x00;
	unsigned char	Vref = 0x00;
	unsigned char	Gain = 0x00;
	
	
	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_VREF, &Vref );
	if ( bRet == false ) return false;
	
	// AGC[9:8](VREF[7:6])
	Temp = (unsigned char)((wValue & 0x0300) >> 2);
	Vref = (Vref & 0x3F) | Temp;
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_VREF, Vref );
	if ( bRet == false ) return false;
	
	// AGC[7:0](GAIN[7:0])
	Gain = (unsigned char)(wValue & 0x00FF);
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_GAIN, Gain );
	if ( bRet == false ) return false;
	
	return true;
}

bool SetReg_AWB_BLUE( unsigned char Value )
{
	return SCCB_Write( OV7670_I2C_ADDR, REG_BLUE, Value );
}
	
bool SetReg_AWB_RED( unsigned char Value )
{
	return SCCB_Write( OV7670_I2C_ADDR, REG_RED, Value );
}


bool EnableAGC( bool bEnable )
{
	bool				bRet = false;
	unsigned char		Com8 = 0x00;
	unsigned char		Temp = 0x00;
	
	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_COM8, &Com8 );
	if ( bRet == false ) return false;
	
	Temp = (bEnable == true) ? 0x04 : 0x00;
	
	Com8 = (Com8 & 0xFB) | Temp;
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_COM8, Com8 );
	if ( bRet == false ) return false;
	
	return true;
}


bool EnableAWB( bool bEnable )
{
	bool				bRet = false;
	unsigned char		Com8 = 0x00;
	unsigned char		Temp = 0x00;
	
	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_COM8, &Com8 );
	if ( bRet == false ) return false;
	
	Temp = (bEnable == true) ? 0x02 : 0x00;
	
	Com8 = (Com8 & 0xFD) | Temp;
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_COM8, Com8 );
	if ( bRet == false ) return false;
	
	return true;
}


bool EnableAEC( bool bEnable )
{
	bool				bRet = false;
	unsigned char		Com8 = 0x00;
	unsigned char		Temp = 0x00;
	
	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_COM8, &Com8 );
	if ( bRet == false ) return false;
	
	Temp = (bEnable == true) ? 0x01 : 0x00;
	
	Com8 = (Com8 & 0xFE) | Temp;
	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_COM8, Com8 );
	if ( bRet == false ) return false;
	
	return true;
}


bool EnableMirrorImage( bool bEnable )
{
	bool				bRet = false;
	unsigned char		Mvfp = 0x00;
	unsigned char		Temp = 0x00;

	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_MVFP, &Mvfp );
	if ( bRet == false ) return false;
	
	Temp = (bEnable == true) ? 0x20 : 0x00;
	Mvfp |= (Mvfp & 0xDF) | Temp;

	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_MVFP, Mvfp );
	if ( bRet == false ) return false;
	
	return true;
}


bool EnableVericallyFlipImage( bool bEnable )
{
	bool				bRet = false;
	unsigned char		Mvfp = 0x00;
	unsigned char		Temp = 0x00;

	bRet = SCCB_Read( OV7670_I2C_ADDR, REG_MVFP, &Mvfp );
	if ( bRet == false ) return false;
	
	Temp = (bEnable == true) ? 0x10 : 0x00;
	Mvfp |= (Mvfp & 0xDE) | Temp;

	bRet = SCCB_Write( OV7670_I2C_ADDR, REG_MVFP, Mvfp );
	if ( bRet == false ) return false;
	
	return true;
}
