/* 共通関数 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <avr/io.h>
#include <util/delay.h>
#include "def.h"


/* ms単位でウェイトとする */
void wait_ms(uint16_t time)
{
	while(time--){
		/* 1固定とすることでコードが小さくなる */
		_delay_ms(1);
	}
}


/* us単位でウェイトする */
void wait_us(uint16_t time)
{
	while(time--){
		/* 1固定とすることでコードが小さくなる */
		_delay_us(1);
	}
}

#endif
