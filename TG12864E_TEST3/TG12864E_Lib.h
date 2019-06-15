#ifndef TG12864E_LIB_H_
#define TG12864E_LIB_H_

#include <avr/io.h>
#include "stdbool.h"

#define TG12864E_DB0			( _BV(PINA0) )
#define TG12864E_DB1			( _BV(PINA1) )
#define TG12864E_DB2			( _BV(PINA2) )
#define TG12864E_DB3			( _BV(PINA3) )
#define TG12864E_DB4			( _BV(PINA4) )
#define TG12864E_DB5			( _BV(PINA5) )
#define TG12864E_DB6			( _BV(PINA6) )
#define TG12864E_DB7			( _BV(PINA7) )

#define TG12864E_CS1			( _BV(PIND0) )
#define TG12864E_CS2			( _BV(PIND1) )
#define TG12864E_RST			( _BV(PIND2) )
#define TG12864E_E  			( _BV(PIND3) )
#define TG12864E_RW  			( _BV(PIND4) )
#define TG12864E_RS  			( _BV(PIND5) )

#define TG12864E_RST_HI()		( PORTD = PORTD | TG12864E_RST )
#define TG12864E_RST_LOW()		( PORTD = PORTD & ~(TG12864E_RST) )

#define TG12864E_CS1_HI()		( PORTD = PORTD | TG12864E_CS1 )
#define TG12864E_CS1_LOW()		( PORTD = PORTD & ~(TG12864E_CS1) )

#define TG12864E_CS2_HI()		( PORTD = PORTD | TG12864E_CS2 )
#define TG12864E_CS2_LOW()		( PORTD = PORTD & ~(TG12864E_CS2) )

#define TG12864E_E_HI()			( PORTD = PORTD | TG12864E_E )
#define TG12864E_E_LOW()		( PORTD = PORTD & ~(TG12864E_E) )

#define TG12864E_RW_HI()		( PORTD = PORTD | TG12864E_RW )
#define TG12864E_RW_LOW()		( PORTD = PORTD & ~(TG12864E_RW) )

#define TG12864E_RS_HI()		( PORTD = PORTD | TG12864E_RS )
#define TG12864E_RS_LOW()		( PORTD = PORTD & ~(TG12864E_RS) )


#define TG12864E_DELAY			( 5 )

#define TG12864E_DISP_WIDTH		( 64 )
#define TG12864E_DISP_HEIGHT	( 64 )

typedef enum
{
	CS1 = 0,
	CS2 = 1
} TG12864_CS_ENUM;

//=============================================================================
// TG12864Eèâä˙âªèàóù
//=============================================================================
void TG12864E_init();

//=============================================================================
// TG12864E ÉäÉZÉbÉgèàóù
//=============================================================================
void TG12864E_Reset();

//=============================================================================
// TG12864E_DisplayOn
//=============================================================================
void TG12864E_DisplayOn( TG12864_CS_ENUM eCS, bool bOn );

//=============================================================================
// TG12864E_SetAddress
//=============================================================================
void TG12864E_SetAddress( TG12864_CS_ENUM eCS, unsigned char Address );

//=============================================================================
// TG12864E_SetPage
//=============================================================================
void TG12864E_SetPage( TG12864_CS_ENUM eCS, unsigned char Page );

//=============================================================================
// TG12864E_DisplayStartLine
//=============================================================================
void TG12864E_DisplayStartLine( TG12864_CS_ENUM eCS, unsigned char Line );

//=============================================================================
// TG12864E_StatusRead
//=============================================================================
void TG12864E_StatusRead( TG12864_CS_ENUM eCS, unsigned char *pStatus );

//=============================================================================
// TG12864E_WriteDisplayData
//=============================================================================
void TG12864E_WriteDisplayData( TG12864_CS_ENUM eCS, unsigned char Data );

//=============================================================================
// TG12864E_ReadDisplayData
//=============================================================================
void TG12864E_ReadDisplayData( TG12864_CS_ENUM eCS, unsigned char *pData );

//=============================================================================
// TG12864E_Fill
//=============================================================================
void TG12864E_Fill(	unsigned char Data );

//=============================================================================
// TG12864E_Pixel
//=============================================================================
void TG12864E_Pixel( uint16_t x, uint16_t y );

//=============================================================================
// TG12864E_Locate
//=============================================================================
void TG12864E_Locate( uint16_t x, uint16_t y, TG12864_CS_ENUM *pCS );

#endif /* TG12864E_LIB_H_ */