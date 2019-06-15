//================================================================================================
// ���x�v
//================================================================================================

#include <avr/io.h>
#include <util/delay.h>


#define F_CPU				( 1000000L )                // MCU���g��(1MHz)
#define BAUD				( 2400L )                   // �ړIUSART�ްڰđ��x
#define MYUBRR				F_CPU / 16 / BAUD - 1       // �ړIUBRRn�l
#define AVG_COUNT			( 50 )						// ���ω�
#define MEASUREMENT_TIME	( 500 )						// �v�����ԊԊu(ms)

//---------------------------------------------------------
// USART����������
//---------------------------------------------------------
void USART_Init( void )
{
    DDRD  = 0b11111100;
    PORTD = 0b00000000;
 
 
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)(MYUBRR);
 
    UCSR0A = 0b00000000;
    UCSR0B = 0b00011000;    // ��M���E���M����
    UCSR0C = 0b00000110;    // �񓯊��E�p���e�B�Ȃ��E��~�r�b�g�F1bit�E�f�[�^�r�b�g�F8bit
     
    return;
}
 
 
//---------------------------------------------------------
// ��M����
//---------------------------------------------------------
unsigned char USART_Receive(void)
{
    // ��M�����҂��i���ǃf�[�^�������Ȃ�܂Ń��[�v�j
    while ( !(UCSR0A & 0b10000000) );
     
    return UDR0;
}
 
 
//---------------------------------------------------------
// ���M����
//---------------------------------------------------------
void USART_Transmit( unsigned char Data)
{
    // ���M�o�b�t�@���󂫂ɂȂ�܂ő҂�
    while ( !(UCSR0A & 0b00100000) );
     
    UDR0 = Data;
}

int main(void)
{
	ADMUX  = 0b10000000;				// ��d���F����1.1V,ADC0
	DIDR0  = 0b11111110;				// �f�W�^�����͋֎~:ADC7�`ADC1
	uint16_t	result = 0;				// A/D�ϊ�����
	uint16_t	total = 0;				// A/D�ϊ����ʂ̍��v
	int16_t		avg = 0;				// A/D�ϊ����ʂ̕���
	int16_t		temp = 0;				// ���x
	uint16_t	cnt = 0;				// �J�E���g
	char		szBuff[ 64 ];			// ���O�o�͗p
	uint16_t	i = 0;
	int16_t		len = 0;
	

	// USART������	
	USART_Init();
	
	// ���x�𒲂ׂ�
	while ( 1 )
	{
		// A/D�ϊ��J�n
		ADCSRA = 0b11001000;			// A/D����, A/D�ϊ��J�n, A/D�ϊ��������荞�݋���
		
		// A/D�ϊ��������荞�ݗv���t���O���P�ɂȂ�܂Ń��[�v
		while ( 1 )
		{
			if ( ADCSRA & 0b00010000 ) break;
		}
		
		// A/D�ϊ����ʁi�d��(mV)�j���擾
		result = (ADCH & 0b00000011);
		result = (result << 8) + ADCL;
		result = (uint16_t)(((double)1.1 * ((double)result / (double)1024)) * 1000);
		total  = total + result;
		
		cnt = cnt + 1;
		if ( cnt < AVG_COUNT ) continue;
		
		// A/D�ϊ����ʂ̕��ς����߂�
		avg = (int16_t)(total / AVG_COUNT);
		
		// A/D�ϊ����ʂ̕��ς��牷�x���Z�o
		temp = (int16_t)((((double)(avg - 424)) / 6.25) * 10);
		
		// ���O�o��
		sprintf( szBuff, "%3d.%d  (%4dmV)\n", (temp / 10), (temp % 10), avg );
		len = strlen( szBuff );
		for ( i = 0 ; i < len ; i++ )
		{
			USART_Transmit( (unsigned char)szBuff[i] );
		}
		
		total = 0;
		cnt = 0;
		
		// �v�����ԊԊu���A�҂�
		_delay_ms( MEASUREMENT_TIME );
	}
	
    return 0;
}