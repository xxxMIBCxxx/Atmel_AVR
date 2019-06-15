#define F_CPU		( 20000000UL )		// 8MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <stdbool.h>



#define IR_LED_BIT			( 0b00000001 )		// 赤外線LED用BIT
#define NEC_TIME			( 562 )				// T=562us
#define NEC_TRANSMIT_TIME	( 108000 )			// us

volatile uint32_t g_uiTimeCount = 0;
void Delay_us( uint32_t us );

//===============================================
// タイマー割り込み処理
//===============================================
ISR( TIMER0_COMPA_vect )
{
	g_uiTimeCount++;
}


//===============================================
// NECフォーマット Leader送信
//===============================================
void NEC_Leader(uint32_t *pTransmitTime)
{
	PORTA |= IR_LED_BIT;
	Delay_us( NEC_TIME * 16 );
	PORTA &= ~(IR_LED_BIT);
	Delay_us( NEC_TIME * 8 );
	
	*pTransmitTime += ( NEC_TIME * 16 ) + ( NEC_TIME * 8 );
	
	return;
}


//===============================================
// NECフォーマット DataBit送信
//===============================================
void NEC_DataBit( bool bOn, uint32_t *pTransmitTime )
{
	
	// Data Bit？
	if ( bOn == true )
	{
		// ON('1')
		PORTA |= IR_LED_BIT;
		Delay_us( NEC_TIME * 1 );
		PORTA &= ~(IR_LED_BIT);
		Delay_us( NEC_TIME * 3 );
		
		*pTransmitTime += ( NEC_TIME * 1 ) + ( NEC_TIME * 3 );
	}
	else
	{
		// OFF('0')
		PORTA |= IR_LED_BIT;
		Delay_us( NEC_TIME * 1 );
		PORTA &= ~(IR_LED_BIT);
		Delay_us( NEC_TIME * 1 );

		*pTransmitTime += ( NEC_TIME * 1 ) + ( NEC_TIME * 1 );
	}

	return;
}


//===============================================
// NECフォーマット DataEnd送信
//===============================================
void NEC_DataEnd( uint32_t *pTransmitTime )
{
		// OFF('0')
		PORTA |= IR_LED_BIT;
		Delay_us( NEC_TIME * 1 );
		PORTA &= ~(IR_LED_BIT);
		Delay_us( NEC_TIME * 1 );

		*pTransmitTime += ( NEC_TIME * 1 ) + ( NEC_TIME * 1 );
}

//===============================================
// NECフォーマット データ送信
//===============================================
void Send_NEC_Data( unsigned char *pData, uint32_t uDataSize )
{
	bool bBit = false;
	unsigned char temp = 0x00;
	uint32_t TransmitTime = 0;
	
	// Leaderを送信
	NEC_Leader( &TransmitTime );
	
	// データを送信
	for ( uint32_t i = 0 ; i < uDataSize ; i++ )
	{
		// 送信データを取得
		temp = pData[ i ];
		for ( uint32_t j = 0 ; j < 8 ; j++ )
		{
			bBit = ((temp<<j)&0x80) ? true : false;
			NEC_DataBit( bBit, &TransmitTime );			
		}
	}

	NEC_DataEnd( &TransmitTime );
	_delay_us( NEC_TRANSMIT_TIME - TransmitTime );

	return;	
}

//===============================================
// ディレイ
//===============================================
void Delay_us( uint32_t count )
{
	uint32_t temp = count / 25;
	
	g_uiTimeCount = 0;

	// 1us毎に割込みを発生させる
	TIMSK0 = 0b00000010;
	while ( g_uiTimeCount < temp )
	{	
	}
	TIMSK0 = 0b00000000;

	return;
}


//===============================================
// メイン処理
//===============================================
int main(void)
{
	DDRA   = 0b00000001;
	DDRD   = 0b00010000;
	DDRB   = 0b00000000;
	PORTB  = 0b00000001;
	
	// 東芝REGZA 送信データ
	//unsigned char szData[] = {0x02,0xFD,0x48,0xB7};			// [電源]ボタン
	//unsigned char szData[] = {0x02,0xFD,0x58,0xA7};			// [音量+]ボタン
	unsigned char szData[] = {0x02,0xFD,0x78,0x87};			// [音量-]ボタン



	// Timer0の設定(25us毎の割込みを発生させる)
	TCNT0  = 0;
	OCR0A  = 62;
	TCCR0A = 0b00000011;
	TCCR0B = 0b00001010;
	TIMSK0 = 0b00000000;
	
	// Timer1の設定(赤外線送信用の矩形波[38KHz 1/3]) ※PD4に出力
	TCNT1  = 0;
	ICR1   = 65;
	OCR1B  = 21;
	TCCR1A = 0b00100010;
	TCCR1B = 0b00011010;	

	//割り込み許可
	sei();

    while(1)
    {
		if ( bit_is_clear(PINB,PB0) )
		{
			// 赤外線送信
			Send_NEC_Data( szData, (sizeof(szData)/sizeof(unsigned char)));
		}        
    }
}

