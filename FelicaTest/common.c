/* ���ʊ֐� */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <avr/io.h>
#include <util/delay.h>
#include "def.h"


/* ms�P�ʂŃE�F�C�g�Ƃ��� */
void wait_ms(uint16_t time)
{
	while(time--){
		/* 1�Œ�Ƃ��邱�ƂŃR�[�h���������Ȃ� */
		_delay_ms(1);
	}
}


/* us�P�ʂŃE�F�C�g���� */
void wait_us(uint16_t time)
{
	while(time--){
		/* 1�Œ�Ƃ��邱�ƂŃR�[�h���������Ȃ� */
		_delay_us(1);
	}
}

#endif
