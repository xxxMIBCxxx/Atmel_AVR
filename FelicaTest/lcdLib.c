/* Lcd�̏��� */

#include "lcdLibPrv.h"
#include "common.h"

/* static�֐��錾 */
static void Lcd_Command(uint8_t ucCmd, uint8_t ucRS);
static void Lcd_Command2(uint8_t ucCmd, uint8_t ucRS);
static void Lcd_RunEbit(void);
static void Lcd_PortInit(void);

/* �r�W�[�ǂݏo�����g�p����ꍇ */
#ifdef CONF_LCD_BUSY_READ
static uint8_t Lcd_WaitReady(uint8_t uOnce);
#endif


/********/
/* ���J */
/********/
// LCD������
void Lcd_Init(void)
{
	/******************/
	/* �|�[�g�����ݒ� */
	/******************/
	Lcd_PortInit();


	/**********/
	/* ������ */
	/**********/

	/* 15ms�ҋ@ */
	wait_ms(15);

	/* 0011���R�񑗐M */
	/* 5ms�̃f�B���C */

	/* �P���0011�𑗐M */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 5ms�ҋ@ */
	wait_ms(5);

	/* �Q���0011�𑗐M */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 100us�ҋ@ */
	wait_us(100);

	/* �R���0011�𑗐M */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);


	/* 4bit���[�h */
	/* �����ȍ~�̓r�W�[�`�F�b�N�\ */
	Lcd_Command(0x02, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 100us�ҋ@ */
	wait_us(100);
#endif

	/**********************/
	/* �g�p�����Ԃɐݒ� */
	/**********************/

	/* 4�r�b�g�ʐM�A�f���[�e�B�[ 1/16�A�L�����N�^�T�C�Y 5x7��ݒ� */
	Lcd_Command2(0b00101000, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif


	/* �\��OFF�A�J�[�\��OFF�A�u�����NOFF��ݒ� */
	Lcd_Command2(0b00000100, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif


	/* �������݌㎩���J�[�\���E�ړ��A�\���V�t�gOFF */
	Lcd_Command2(0b00000110, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif

	/* �S�\���N���A */
	Lcd_Clear();


	/* �\��ON */
	Lcd_On();
}


/* �������ݏ��� */
void Lcd_Write(uint8_t *szStr, uint8_t ucLen)
{
	/* �w�蕶�������������� */
	for(uint8_t i = 0; i < ucLen; i++){
		/* �P�o�C�g�������� */
		Lcd_Command2(szStr[i], DEF_LCD_RS_DATA);

#ifdef CONF_LCD_BUSY_READ
		/* �����҂� */
		Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
		/* 40us�ҋ@ */
		wait_us(DEF_LCD_SHORT_WAIT);
#endif
	}
}


/* �J�[�\���ʒu�ړ� */
/* �w���0�C���f�b�N�X�ōs�� */
void Lcd_MoveCur(uint8_t x, uint8_t y)
{
	uint8_t tmp;

	/* �͈̓`�F�b�N */
	if((x < DEF_LCD_MAX_X) && (y < DEF_LCD_MAX_Y)){
		tmp = 0b10000000 + x;

		switch(y){
			case 0:		/* 1�s�� */
				/* �������� */
				break;

			case 1:		/* 2�s�� */
				tmp += 0b01000000;
				break;

#if CONF_LCD_TYPE == DEF_LCD_TYPE_2004
/* SC2004�̏ꍇ�̂� */
			case 2:		/* 3�s�� */
				tmp += 20;
				break;

			case 3:		/* 4�s�� */
				tmp += 20;
				tmp += 0b01000000;
				break;
#endif
			default:
				/* �������� */
				/* �ړ����Ȃ� */
				return;
				break;
		}
		/* �J�[�\���ړ� */
		Lcd_Command2(tmp, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
		/* �����҂� */
		Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
		wait_ms(DEF_LCD_SHORT_WAIT);
#endif
	}
}


/* LCD���� */
void Lcd_Clear()
{
	/* �\���N���A */
	Lcd_Command2(0b00000001, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 2ms�ҋ@ */
	wait_ms(DEF_LCD_LONG_WAIT);
#endif
}


/* LCD off */
void Lcd_Off()
{
	/* LCD off */
	Lcd_Command2(0b00001000, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif
}


/* LCD on */
void Lcd_On()
{
	/* LCD on */
	Lcd_Command2(0b00001100, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* �����҂� */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us�ҋ@ */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif
}


/**********/
/* ����J */
/**********/

/* �R�}���h���M */
/* ����4bit�̂ݑ��M���� */
void Lcd_Command(uint8_t ucCmd, uint8_t ucRS)
{
	/* �e�r�b�g���ɕ��� */
	ST_PORT_BITFLD st;
	st.uni.byte = ucCmd;

	/* E�|�[�g��0���Z�b�g */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;

	/* RW�|�[�g�ɏ������݂��Z�b�g */
	PIN_LCD_RW_PORT = DEF_LCD_RW_WRITE;

	/* RS�|�[�g���Z�b�g */
	PIN_LCD_RS_PORT = ucRS;

	/* �e�|�[�g�̏o�͂ɐݒ� */
	PIN_LCD_DB4_PORT = st.uni.bit.P0;
	PIN_LCD_DB5_PORT = st.uni.bit.P1;
	PIN_LCD_DB6_PORT = st.uni.bit.P2;
	PIN_LCD_DB7_PORT = st.uni.bit.P3;

	/* �R�}���h���s */
	Lcd_RunEbit();

	/* �e�|�[�g��Low�ɖ߂� */
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;
}


/* �R�}���h���M */
/* 8bit�����M����i�Q�񑗐M�j */
/*�i�E�F�C�g�͎��Ԃ��킩��Ȃ��̂ŃR�R�ł͂��Ȃ��j */
void Lcd_Command2(uint8_t ucCmd, uint8_t ucRS)
{
	/* ��ʃr�b�g���M */
    Lcd_Command((ucCmd >> 4) & 0x0F, ucRS);

	/* ���ʃr�b�g���M */
    Lcd_Command(ucCmd & 0x0F, ucRS);
}


/* E�r�b�g�𑀍� */
void Lcd_RunEbit()
{
	/* E�|�[�g�ɂP���Z�b�g */
	PIN_LCD_E_PORT = DEF_LCD_E_HI;

	/* NOP�i��200ns���x�҂��j */
	/* ���Ԃ�����Ȃ��ꍇ�͂����P��NOP������ */
	asmNOP();

	/* E�|�[�g��0���Z�b�g */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;
}

/* LCD�Ŏg�p����|�[�g�̐ݒ� */
void Lcd_PortInit()
{
	/* �|�[�g��Low(0)�ɐݒ� */
	PIN_LCD_RS_PORT = 0;
	PIN_LCD_RW_PORT = 0;
	PIN_LCD_E_PORT = 0;
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;

	/* �|�[�g���o�͂ɐݒ� */
	PIN_LCD_RS_DDR = 1;
	PIN_LCD_RW_DDR = 1;
	PIN_LCD_E_DDR = 1;
	PIN_LCD_DB4_DDR = 1;
	PIN_LCD_DB5_DDR = 1;
	PIN_LCD_DB6_DDR = 1;
	PIN_LCD_DB7_DDR = 1;
}


/*****************************/
/* ���J�iBusy�ǂݏo�����̂݁j*/
/*****************************/

/* �r�W�[��ԓǂݏo���o�[�W�����̏ꍇ */
#ifdef CONF_LCD_BUSY_READ

/* ���������҂� */
uint8_t Lcd_WaitReady(uint8_t uOnce)
{
	uint8_t ucReady;

	/* E�|�[�g��0���Z�b�g */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;

	/* RW�|�[�g�ɓǂݏo�����Z�b�g */
	PIN_LCD_RW_PORT = DEF_LCD_RW_READ;

	/* RS�|�[�g���Z�b�g */
	PIN_LCD_RS_PORT = DEF_LCD_RS_COMM;

	/* �|�[�g����͂ɐݒ� */
	/* �ŏ��ɏo�͂�Low�ɐݒ� */
	/* �i���͂ɐ؂�ւ������Ƀv���A�b�v�ƂȂ�̂�h���j */
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;

	/* �|�[�g����͂ɐݒ� */
	PIN_LCD_DB4_DDR = 0;
	PIN_LCD_DB5_DDR = 0;
	PIN_LCD_DB6_DDR = 0;
	PIN_LCD_DB7_DDR = 0;

	do {
		/* �P��ڂ̓ǂݏo�� */
		/* E�|�[�g�ɂP���Z�b�g */
		/* E�|�[�g��HI�̊Ԃ̂ݓǂݏo�����\ */
		PIN_LCD_E_PORT = DEF_LCD_E_HI;

		/* �ҋ@ */
		asmNOP();

		/* �ǂݏo��1��� */
		ucReady = PIN_LCD_DB7_PIN;

		/* E�|�[�g��0���Z�b�g */
		PIN_LCD_E_PORT = DEF_LCD_E_LOW;

		/* �ҋ@ */
		asmNOP();

		
		/* �Q��ڂ̓ǂݏo�� */
		/* �Q��ڂ͓ǂݔ�΂� */
		Lcd_RunEbit();

		/* �P��m�F�̏ꍇ�͂�����Ń��[�v�I�� */
		if(uOnce == DEF_LCD_WAIT_ONCE){
			break;
		}

	/* �ʏ�̊m�F�̏ꍇ�̓��f�B�ɂȂ�܂őҋ@���� */
	} while(ucReady == DEF_LCD_BUSY);

	/* �|�[�g���o�͂ɍĐݒ� */
	PIN_LCD_DB4_DDR = 1;
	PIN_LCD_DB5_DDR = 1;
	PIN_LCD_DB6_DDR = 1;
	PIN_LCD_DB7_DDR = 1;

	/* RW�|�[�g�ɏ������݂��Z�b�g */
	PIN_LCD_RW_PORT = DEF_LCD_RW_WRITE;

	return ucReady;
}

#endif
