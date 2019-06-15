//=================================================================================================
// SCCB ���C�u���� �w�b�_�[�t�@�C��
//=================================================================================================
#ifndef __SCCB_LIB_H__
#define __SCCB_LIB_H__

#include <avr/io.h>
#include <stdbool.h>

//-------------------------------------------------------------------------------------------------
// SCCB ������
//-------------------------------------------------------------------------------------------------
void SCCB_Init(void);


//-------------------------------------------------------------------------------------------------
// SCCB ������
//-------------------------------------------------------------------------------------------------
bool SCCB_Write( unsigned char ID, unsigned char Addr, unsigned char Data );


//-------------------------------------------------------------------------------------------------
// SCCB �Ǎ���
//-------------------------------------------------------------------------------------------------
bool SCCB_Read( unsigned char ID, unsigned char Addr, unsigned char *pData );


#endif	// #ifndef __SCCB_LIB_H__