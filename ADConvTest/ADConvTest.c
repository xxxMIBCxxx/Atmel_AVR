//================================================================================================
// 温度計
//================================================================================================

#include <avr/io.h>
#include <util/delay.h>


#define F_CPU				( 1000000L )                // MCU周波数(1MHz)
#define BAUD				( 2400L )                   // 目的USARTﾎﾞｰﾚｰﾄ速度
#define MYUBRR				F_CPU / 16 / BAUD - 1       // 目的UBRRn値
#define AVG_COUNT			( 50 )						// 平均回数
#define MEASUREMENT_TIME	( 500 )						// 計測時間間隔(ms)

//---------------------------------------------------------
// USART初期化処理
//---------------------------------------------------------
void USART_Init( void )
{
    DDRD  = 0b11111100;
    PORTD = 0b00000000;
 
 
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)(MYUBRR);
 
    UCSR0A = 0b00000000;
    UCSR0B = 0b00011000;    // 受信許可・送信許可
    UCSR0C = 0b00000110;    // 非同期・パリティなし・停止ビット：1bit・データビット：8bit
     
    return;
}
 
 
//---------------------------------------------------------
// 受信処理
//---------------------------------------------------------
unsigned char USART_Receive(void)
{
    // 受信完了待ち（未読データが無くなるまでループ）
    while ( !(UCSR0A & 0b10000000) );
     
    return UDR0;
}
 
 
//---------------------------------------------------------
// 送信処理
//---------------------------------------------------------
void USART_Transmit( unsigned char Data)
{
    // 送信バッファが空きになるまで待つ
    while ( !(UCSR0A & 0b00100000) );
     
    UDR0 = Data;
}

int main(void)
{
	ADMUX  = 0b10000000;				// 基準電圧：内部1.1V,ADC0
	DIDR0  = 0b11111110;				// デジタル入力禁止:ADC7～ADC1
	uint16_t	result = 0;				// A/D変換結果
	uint16_t	total = 0;				// A/D変換結果の合計
	int16_t		avg = 0;				// A/D変換結果の平均
	int16_t		temp = 0;				// 温度
	uint16_t	cnt = 0;				// カウント
	char		szBuff[ 64 ];			// ログ出力用
	uint16_t	i = 0;
	int16_t		len = 0;
	

	// USART初期化	
	USART_Init();
	
	// 温度を調べる
	while ( 1 )
	{
		// A/D変換開始
		ADCSRA = 0b11001000;			// A/D許可, A/D変換開始, A/D変換完了割り込み許可
		
		// A/D変換完了割り込み要求フラグが１になるまでループ
		while ( 1 )
		{
			if ( ADCSRA & 0b00010000 ) break;
		}
		
		// A/D変換結果（電圧(mV)）を取得
		result = (ADCH & 0b00000011);
		result = (result << 8) + ADCL;
		result = (uint16_t)(((double)1.1 * ((double)result / (double)1024)) * 1000);
		total  = total + result;
		
		cnt = cnt + 1;
		if ( cnt < AVG_COUNT ) continue;
		
		// A/D変換結果の平均を求める
		avg = (int16_t)(total / AVG_COUNT);
		
		// A/D変換結果の平均から温度を算出
		temp = (int16_t)((((double)(avg - 424)) / 6.25) * 10);
		
		// ログ出力
		sprintf( szBuff, "%3d.%d  (%4dmV)\n", (temp / 10), (temp % 10), avg );
		len = strlen( szBuff );
		for ( i = 0 ; i < len ; i++ )
		{
			USART_Transmit( (unsigned char)szBuff[i] );
		}
		
		total = 0;
		cnt = 0;
		
		// 計測時間間隔分、待つ
		_delay_ms( MEASUREMENT_TIME );
	}
	
    return 0;
}