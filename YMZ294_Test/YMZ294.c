//=================================================================================================
// YMZ294���C�u����
//=================================================================================================
#include "YMZ294.h"
#include <util/delay.h>


static void YMZ294_SetRegister( BYTE Address, BYTE Data );


//-------------------------------------------------------------------
// YMZ294������
//-------------------------------------------------------------------
void YMZ294_Init( void )
{
	// YMZ294�Ŏg�p����PIN��ݒ�
	DDRB = D0_PIN | D1_PIN | D2_PIN | D3_PIN | D4_PIN | D5_PIN | D6_PIN | D7_PIN;
	DDRD = RESET_PIN | AD_PIN | CS_PIN | WR_PIN;
	
	// RESET�EAD�ECS�EWR�̃s����HI�ɂ���
	PORTD = RESET_PIN | AD_PIN | CS_PIN | WR_PIN;
	
	// ���肷��܂ő҂�
	_delay_ms( 50 );
	
	// YMZ294�����Z�b�g
	YMZ294_Reset();

	return;
}


//-------------------------------------------------------------------
// YMZ294���Z�b�g����
//-------------------------------------------------------------------
void YMZ294_Reset( void )
{
	PORTD &= ~RESET_PIN;
	_delay_ms( 100 );
	PORTD |= RESET_PIN;
	_delay_ms( 50 );

	return;
}



//-------------------------------------------------------------------
// YMZ294�y�����g���ݒ�($00 - $05)
//-------------------------------------------------------------------
void YMZ294_SetFreq( YMZ294_CH_ENUM eCh, WORD wFreq )
{
	BYTE		Address = 0x00;
	
	
	// �w�肵���`�����l����ʂ���Ή�����A�h���X���擾
	switch ( eCh ) {
	case YMZ294_CH_A:
		Address = 0x00;
		break;
	case YMZ294_CH_B:
		Address = 0x02;
		break;
	case YMZ294_CH_C:
		Address = 0x04;
		break;
	default:
		return;
	}	
	
	// �y�����g���̐ݒ�
	YMZ294_SetRegister( Address  , ((wFreq & 0x00FF) >> 0) );
	YMZ294_SetRegister( Address+1, ((wFreq & 0x0F00) >> 8) );
	
	return;
}	
	
	
//-------------------------------------------------------------------
// YMZ294�m�C�Y���g���ݒ�($06)
//-------------------------------------------------------------------
void YMZ294_SetNoiseFreq( BYTE Freq )
{
	YMZ294_SetRegister( 0x06, (Freq & 0x1F) );
	
	return;
}


//-------------------------------------------------------------------
// YMZ294�~�L�T�[�ݒ�($07)
//-------------------------------------------------------------------
void YMZ294_SetMixer( BYTE Noise, BYTE Tone )
{
	YMZ294_SetRegister( 0x07, (((Noise & 0x07) << 3) | (Tone & 0x07)) );
	
	return;
}


//-------------------------------------------------------------------
// YMZ294���ʐݒ�($08 - $0A)
//-------------------------------------------------------------------
void YMZ294_SetVolume( YMZ294_CH_ENUM eCh, bool bEnv, BYTE Level )
{
	BYTE		Address = 0x00;
	BYTE		temp_level = 0x00;
	
	
	// �w�肵���`�����l����ʂ���Ή�����A�h���X���擾
	switch ( eCh ) {
	case YMZ294_CH_A:
		Address = 0x08;
		break;
	case YMZ294_CH_B:
		Address = 0x09;
		break;
	case YMZ294_CH_C:
		Address = 0x0A;
		break;
	default:
		return;
	}
	
	// �G���x���[�u����
	if ( bEnv == true )
	{
		temp_level = 0x10;
	}
	temp_level = temp_level | (Level & 0x0F);
	
	YMZ294_SetRegister( Address, temp_level );
	
	return;
}


//-------------------------------------------------------------------
// YMZ294�G���x���[�u���g���ݒ�($0B - $0C)
//-------------------------------------------------------------------
void SetEnvFreq( WORD wFreq )
{
	YMZ294_SetRegister( 0x0B, ((wFreq & 0x00FF) >> 0) );
	YMZ294_SetRegister( 0x0C, ((wFreq & 0xFF00) >> 8) );	

	return;	
}


//-------------------------------------------------------------------
// YMZ294�G���x���[�u�`��̐ݒ�($0D)
//-------------------------------------------------------------------
void YMZ294_SetEnvShape( BYTE Cont, BYTE Att, BYTE Alt, BYTE Hold )
{
	YMZ294_SetRegister( 0x0D, ((Cont << 3) | (Att << 2) | (Alt << 1) | Hold) );
	
	return;
}










































//-------------------------------------------------------------------
// ���W�X�^�ݒ菈��
//-------------------------------------------------------------------
static void YMZ294_SetRegister( BYTE Address, BYTE Data )
{
	// �A�h���X�`�F�b�N
	if ( Address > 0x0D )
	{
		return;
	}
	
	// �A�h���X��ݒ�
	PORTD &= ~(AD_PIN | CS_PIN | WR_PIN);
	PORTB = Address;
	PORTD |= CS_PIN | WR_PIN;	
	
	// �f�[�^��ݒ�
	PORTD = (PORTD & ~(CS_PIN | WR_PIN)) | AD_PIN;
	PORTB = Data;
	PORTD |= AD_PIN | CS_PIN | WR_PIN;	
	
	return;
}




