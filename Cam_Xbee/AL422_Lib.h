#ifndef __AL422_LIB_H__
#define __AL422_LIB_H__


#include <avr/io.h>
#include <stdbool.h>
#include "util/delay.h"


#define AL422_DO0			_BV(PIND0)			// Data Output 0
#define AL422_DO1			_BV(PIND1)			// Data Output 1
#define AL422_DO2			_BV(PIND2)			// Data Output 2
#define AL422_DO3			_BV(PIND3)			// Data Output 3
#define AL422_DO4			_BV(PIND4)			// Data Output 4
#define AL422_DO5			_BV(PIND5)			// Data Output 5
#define AL422_DO6			_BV(PIND6)			// Data Output 6
#define AL422_DO7			_BV(PIND7)			// Data Output 7


#define AL422_RCK			_BV(PINA6)			// Read clock
#define AL422_OE			_BV(PINA5)			// Output enable
#define AL422_RRST			_BV(PINA4)			// Read Reset(actve low)
#define AL422_NC			_BV(PINA3)
#define AL422_WEN			_BV(PINA2)			// Write Enable(activ low)
#define HREF				_BV(PINA1)
#define VSYNC				_BV(PINA0)

//-------------------------------------------------------------------------------------------------
// AL422 èâä˙èàóù
//-------------------------------------------------------------------------------------------------
void AL422_Init(void);

//-------------------------------------------------------------------------------------------------
// AL422 ReadÉäÉZÉbÉg
//-------------------------------------------------------------------------------------------------
void AL422_ReadReset(void);


unsigned char AL422_ReadEnable( void );


void AL422_OutuptEnable( void );


void AL422_WriteReset( void );


#endif	// #ifndef __AL422_LIB_H__