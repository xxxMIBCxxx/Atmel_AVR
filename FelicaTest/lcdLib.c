/* Lcdの処理 */

#include "lcdLibPrv.h"
#include "common.h"

/* static関数宣言 */
static void Lcd_Command(uint8_t ucCmd, uint8_t ucRS);
static void Lcd_Command2(uint8_t ucCmd, uint8_t ucRS);
static void Lcd_RunEbit(void);
static void Lcd_PortInit(void);

/* ビジー読み出しを使用する場合 */
#ifdef CONF_LCD_BUSY_READ
static uint8_t Lcd_WaitReady(uint8_t uOnce);
#endif


/********/
/* 公開 */
/********/
// LCD初期化
void Lcd_Init(void)
{
	/******************/
	/* ポート初期設定 */
	/******************/
	Lcd_PortInit();


	/**********/
	/* 初期化 */
	/**********/

	/* 15ms待機 */
	wait_ms(15);

	/* 0011を３回送信 */
	/* 5msのディレイ */

	/* １回目0011を送信 */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 5ms待機 */
	wait_ms(5);

	/* ２回目0011を送信 */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 100us待機 */
	wait_us(100);

	/* ３回目0011を送信 */
	Lcd_Command(0x03, DEF_LCD_RS_COMM);
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);


	/* 4bitモード */
	/* ここ以降はビジーチェック可能 */
	Lcd_Command(0x02, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 100us待機 */
	wait_us(100);
#endif

	/**********************/
	/* 使用する状態に設定 */
	/**********************/

	/* 4ビット通信、デューティー 1/16、キャラクタサイズ 5x7を設定 */
	Lcd_Command2(0b00101000, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif


	/* 表示OFF、カーソルOFF、ブリンクOFFを設定 */
	Lcd_Command2(0b00000100, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif


	/* 書き込み後自動カーソル右移動、表示シフトOFF */
	Lcd_Command2(0b00000110, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif

	/* 全表示クリア */
	Lcd_Clear();


	/* 表示ON */
	Lcd_On();
}


/* 書き込み処理 */
void Lcd_Write(uint8_t *szStr, uint8_t ucLen)
{
	/* 指定文字長分書き込む */
	for(uint8_t i = 0; i < ucLen; i++){
		/* １バイト書き込む */
		Lcd_Command2(szStr[i], DEF_LCD_RS_DATA);

#ifdef CONF_LCD_BUSY_READ
		/* 完了待ち */
		Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
		/* 40us待機 */
		wait_us(DEF_LCD_SHORT_WAIT);
#endif
	}
}


/* カーソル位置移動 */
/* 指定は0インデックスで行う */
void Lcd_MoveCur(uint8_t x, uint8_t y)
{
	uint8_t tmp;

	/* 範囲チェック */
	if((x < DEF_LCD_MAX_X) && (y < DEF_LCD_MAX_Y)){
		tmp = 0b10000000 + x;

		switch(y){
			case 0:		/* 1行目 */
				/* 処理無し */
				break;

			case 1:		/* 2行目 */
				tmp += 0b01000000;
				break;

#if CONF_LCD_TYPE == DEF_LCD_TYPE_2004
/* SC2004の場合のみ */
			case 2:		/* 3行目 */
				tmp += 20;
				break;

			case 3:		/* 4行目 */
				tmp += 20;
				tmp += 0b01000000;
				break;
#endif
			default:
				/* 処理無し */
				/* 移動しない */
				return;
				break;
		}
		/* カーソル移動 */
		Lcd_Command2(tmp, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
		/* 完了待ち */
		Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
		wait_ms(DEF_LCD_SHORT_WAIT);
#endif
	}
}


/* LCD消去 */
void Lcd_Clear()
{
	/* 表示クリア */
	Lcd_Command2(0b00000001, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 2ms待機 */
	wait_ms(DEF_LCD_LONG_WAIT);
#endif
}


/* LCD off */
void Lcd_Off()
{
	/* LCD off */
	Lcd_Command2(0b00001000, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif
}


/* LCD on */
void Lcd_On()
{
	/* LCD on */
	Lcd_Command2(0b00001100, DEF_LCD_RS_COMM);

#ifdef CONF_LCD_BUSY_READ
	/* 完了待ち */
	Lcd_WaitReady(DEF_LCD_WAIT_READY);
#else
	/* 40us待機 */
	wait_us(DEF_LCD_SHORT_WAIT);
#endif
}


/**********/
/* 非公開 */
/**********/

/* コマンド送信 */
/* 下位4bitのみ送信する */
void Lcd_Command(uint8_t ucCmd, uint8_t ucRS)
{
	/* 各ビット毎に分割 */
	ST_PORT_BITFLD st;
	st.uni.byte = ucCmd;

	/* Eポートに0をセット */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;

	/* RWポートに書き込みをセット */
	PIN_LCD_RW_PORT = DEF_LCD_RW_WRITE;

	/* RSポートをセット */
	PIN_LCD_RS_PORT = ucRS;

	/* 各ポートの出力に設定 */
	PIN_LCD_DB4_PORT = st.uni.bit.P0;
	PIN_LCD_DB5_PORT = st.uni.bit.P1;
	PIN_LCD_DB6_PORT = st.uni.bit.P2;
	PIN_LCD_DB7_PORT = st.uni.bit.P3;

	/* コマンド実行 */
	Lcd_RunEbit();

	/* 各ポートをLowに戻す */
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;
}


/* コマンド送信 */
/* 8bit分送信する（２回送信） */
/*（ウェイトは時間がわからないのでココではやらない） */
void Lcd_Command2(uint8_t ucCmd, uint8_t ucRS)
{
	/* 上位ビット送信 */
    Lcd_Command((ucCmd >> 4) & 0x0F, ucRS);

	/* 下位ビット送信 */
    Lcd_Command(ucCmd & 0x0F, ucRS);
}


/* Eビットを操作 */
void Lcd_RunEbit()
{
	/* Eポートに１をセット */
	PIN_LCD_E_PORT = DEF_LCD_E_HI;

	/* NOP（約200ns程度待ち） */
	/* 時間が足りない場合はもう１つNOPを入れる */
	asmNOP();

	/* Eポートに0をセット */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;
}

/* LCDで使用するポートの設定 */
void Lcd_PortInit()
{
	/* ポートをLow(0)に設定 */
	PIN_LCD_RS_PORT = 0;
	PIN_LCD_RW_PORT = 0;
	PIN_LCD_E_PORT = 0;
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;

	/* ポートを出力に設定 */
	PIN_LCD_RS_DDR = 1;
	PIN_LCD_RW_DDR = 1;
	PIN_LCD_E_DDR = 1;
	PIN_LCD_DB4_DDR = 1;
	PIN_LCD_DB5_DDR = 1;
	PIN_LCD_DB6_DDR = 1;
	PIN_LCD_DB7_DDR = 1;
}


/*****************************/
/* 公開（Busy読み出し時のみ）*/
/*****************************/

/* ビジー状態読み出しバージョンの場合 */
#ifdef CONF_LCD_BUSY_READ

/* 処理完了待ち */
uint8_t Lcd_WaitReady(uint8_t uOnce)
{
	uint8_t ucReady;

	/* Eポートに0をセット */
	PIN_LCD_E_PORT = DEF_LCD_E_LOW;

	/* RWポートに読み出しをセット */
	PIN_LCD_RW_PORT = DEF_LCD_RW_READ;

	/* RSポートをセット */
	PIN_LCD_RS_PORT = DEF_LCD_RS_COMM;

	/* ポートを入力に設定 */
	/* 最初に出力をLowに設定 */
	/* （入力に切り替えた時にプルアップとなるのを防ぐ） */
	PIN_LCD_DB4_PORT = 0;
	PIN_LCD_DB5_PORT = 0;
	PIN_LCD_DB6_PORT = 0;
	PIN_LCD_DB7_PORT = 0;

	/* ポートを入力に設定 */
	PIN_LCD_DB4_DDR = 0;
	PIN_LCD_DB5_DDR = 0;
	PIN_LCD_DB6_DDR = 0;
	PIN_LCD_DB7_DDR = 0;

	do {
		/* １回目の読み出し */
		/* Eポートに１をセット */
		/* EポートがHIの間のみ読み出しが可能 */
		PIN_LCD_E_PORT = DEF_LCD_E_HI;

		/* 待機 */
		asmNOP();

		/* 読み出し1回目 */
		ucReady = PIN_LCD_DB7_PIN;

		/* Eポートに0をセット */
		PIN_LCD_E_PORT = DEF_LCD_E_LOW;

		/* 待機 */
		asmNOP();

		
		/* ２回目の読み出し */
		/* ２回目は読み飛ばし */
		Lcd_RunEbit();

		/* １回確認の場合はここれでループ終了 */
		if(uOnce == DEF_LCD_WAIT_ONCE){
			break;
		}

	/* 通常の確認の場合はレディになるまで待機する */
	} while(ucReady == DEF_LCD_BUSY);

	/* ポートを出力に再設定 */
	PIN_LCD_DB4_DDR = 1;
	PIN_LCD_DB5_DDR = 1;
	PIN_LCD_DB6_DDR = 1;
	PIN_LCD_DB7_DDR = 1;

	/* RWポートに書き込みをセット */
	PIN_LCD_RW_PORT = DEF_LCD_RW_WRITE;

	return ucReady;
}

#endif
