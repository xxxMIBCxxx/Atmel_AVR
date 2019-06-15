#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <avr/io.h>
#include <stdbool.h>


bool Sensor_Init( void );
bool SetReg_AEC( uint16_t wValue );
bool SetReg_GAIN( uint16_t wValue );
bool SetReg_AWB_BLUE( unsigned char Value );
bool SetReg_AWB_RED( unsigned char Value );
bool EnableAGC( bool bEnable );
bool EnableAWB( bool bEnable );
bool EnableAEC( bool bEnable );
bool EnableMirrorImage( bEnable );
bool EnableVericallyFlipImage( bEnable );
#endif	// #ifndef __SENSOR_H__
