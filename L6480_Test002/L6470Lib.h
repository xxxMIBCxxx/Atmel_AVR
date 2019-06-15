/*
 * L6470Lib.h
 *
 * Created: 2014/03/31 11:00:08
 *  Author: MIBC
 */ 


#ifndef L6470LIB_H_
#define L6470LIB_H_

#include "avr/io.h"
#include "stdbool.h"


typedef enum
{
	L6470_CMD_ABS_POS,		
	L6470_CMD_EL_POS,		
	L6470_CMD_MARK, 			
	L6470_CMD_SPEED,			
	L6470_CMD_ACC,			
	L6470_CMD_DEC,			
	L6470_CMD_MAX_SPEED,		
	L6470_CMD_MIN_SPEED,		
	L6470_CMD_KVAL_HOLD,		
	L6470_CMD_KVAL_RUN,		
	L6470_CMD_KVAL_ACC,		
	L6470_CMD_KVAL_DEC,		
	L6470_CMD_INT_SPD,		
	L6470_CMD_ST_SLP,		
	L6470_CMD_FN_SLP_ACC,	
	L6470_CMD_FN_SLP_DEC,	
	L6470_CMD_K_THERM,		
	L6470_CMD_ADC_OUT,		
	L6470_CMD_ODC_TH,		
	L6470_CMD_STALL_TH,		
	L6470_CMD_FS_SPD,		
	L6470_CMD_STEP_MODE,		
	L6470_CMD_ALARM_EN,		
	L6470_CMD_CONFIG,		
	L6470_CMD_STATUS,		
	L6470_CMD_RUN,			
	L6470_CMD_R_RUN,			
	L6470_CMD_STOP,
	L6470_CMD_MAX				// === MAX ===
} L6470_CMD_ENUM;



// L6470 Register Address
#define L6470_ADDR_ABS_POS		( 0x01 )	// Current position
#define L6470_ADDR_EL_POS		( 0x02 )	// Electrical position
#define L6470_ADDR_MARK 		( 0x03 )	// Mark position
#define L6470_ADDR_SPEED		( 0x04 )	// Current speed
#define L6470_ADDR_ACC			( 0x05 )	//
#define L6470_ADDR_DEC			( 0x06 )	//
#define L6470_ADDR_MAX_SPEED	( 0x07 )	//
#define L6470_ADDR_MIN_SPEED	( 0x08 )	//
#define L6470_ADDR_KVAL_HOLD	( 0x09 )	//
#define L6470_ADDR_KVAL_RUN		( 0x0A )	//
#define L6470_ADDR_KVAL_ACC		( 0x0B )	//
#define L6470_ADDR_KVAL_DEC		( 0x0C )	//
#define L6470_ADDR_INT_SPD		( 0x0D )	//
#define L6470_ADDR_ST_SLP		( 0x0E )	//
#define L6470_ADDR_FN_SLP_ACC	( 0x0F )	//
#define L6470_ADDR_FN_SLP_DEC	( 0x10 )	//
#define L6470_ADDR_K_THERM		( 0x11 )	//
#define L6470_ADDR_ADC_OUT		( 0x12 )	//
#define L6470_ADDR_ODC_TH		( 0x13 )	//
#define L6470_ADDR_STALL_TH		( 0x14 )	//
#define L6470_ADDR_FS_SPD		( 0x15 )	//
#define L6470_ADDR_STEP_MODE	( 0x16 )	//
#define L6470_ADDR_ALARM_EN		( 0x17 )	//
#define L6470_ADDR_CONFIG		( 0x18 )	//
#define L6470_ADDR_STATUS		( 0x19 )	//
//#define L6470_RUN			( 0x51 )		// ê≥âÒì]
//#define L6470_R_RUN			( 0x50 )	// ãtâÒì]
//#define L6470_STOP			( 0xB0 )	// í‚é~






#endif /* L6470LIB_H_ */