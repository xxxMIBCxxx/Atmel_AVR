/* 共通のdeine定義 */

#ifndef _DEF_H_
#define _DEF_H_

#include <avr/io.h>

/* NOP関数 */
#define asmNOP()	__asm__ __volatile__ ("nop") 

/* ポートをビットフィールドアクセスする構造体 */
typedef struct {
	union {
		struct {
			uint8_t P0: 1;
			uint8_t P1: 1;
			uint8_t P2: 1;
			uint8_t P3: 1;
			uint8_t P4: 1;
			uint8_t P5: 1;
			uint8_t P6: 1;
			uint8_t P7: 1;
		} bit;
		uint8_t byte;
	} uni;
} ST_PORT_BITFLD;


#endif
