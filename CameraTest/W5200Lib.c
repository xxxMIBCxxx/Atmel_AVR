//********************************************************************************
// Wiznet社のW5200にアクセスするための関数群
//********************************************************************************
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "W5200Lib.h"

#define UDP_SOCKET					( 0 )

#define W5200_DATA_READ_COMMAND		( 0x0000 )			// 読込みコマンド
#define W5200_DATA_WRITE_COMMAND	( 0x8000 )			// 書込みコマンド

#define RESET_ACTIVE_LOW_TIME		( 3 )				// リセット開始信号通知時間 (us)
#define RESET_WAIT_TIME				( 2500 )			// リセット待ち時間(ms)
#define	RESET_PIN					_BV( PINB1 )		// リセットPIN
#define	MOSI_PIN					_BV( PINB5 )		// MOSI PIN
#define	MISO_PIN					_BV( PINB6 )		// MISO PIN
#define	SCK_PIN						_BV( PINB7 )		// SCK PIN
#define	SS_PIN						_BV( PINB4 )		// SS PIN


#define	W5200_SOCKET_NUM			( 8 )
#define W5200_TX_MEMORY_BASE		( 0x8000 )
#define W5200_TX_MEMORY_NUM			( 2 )
#define W5200_TX_MEMORY_SIZE		( 1024 * W5200_TX_MEMORY_NUM )

#define W5200_RX_MEMORY_BASE		( 0xC000 )
#define W5200_RX_MEMORY_NUM			( 2 )
#define W5200_RX_MEMORY_SIZE		( 1024 * W5200_RX_MEMORY_NUM )

volatile uint16						g_Sn_TX_BASE[ W5200_SOCKET_NUM ];
volatile uint16						g_Sn_TX_MASK[ W5200_SOCKET_NUM ];
volatile uint16						g_Sn_RX_BASE[ W5200_SOCKET_NUM ];
volatile uint16						g_Sn_RX_MASK[ W5200_SOCKET_NUM ];


//======================================================================================================================
// DHCP関連
//======================================================================================================================
typedef struct
{
	uint8		op;
	uint8		htype;
	uint8		hlen;
	uint8		hops;
	uint8		xid[4];
	uint16		secs;
	uint16		flags;
	uint8		ciaddr[4];
	uint8		yiaddr[4];
	uint8		siaddr[4];
	uint8		giaddr[4];
	uint8		chaddr[16];
	uint8		sname[64];
	uint8		file[128];
	uint8		options[64];
} DHCP_MESSAGE_TABLE, *PDHCP_MESSAGE_TABLE;


#define DCHP_CLIENT_PORT 		( 68 )
#define DCHP_SERVER_PORT 		( 67 )

const uint8 DHCP_HEADER[4] 		= { 0x63, 0x82, 0x53, 0x63 };
const uint8 DHCP_DISCOVER[3]	= { 0x35, 0x01, 0x01 };
const uint8 DHCP_OFFER[3]  		= { 0x35, 0x01, 0x02 };
const uint8 DHCP_REQUEST[3] 	= { 0x35, 0x01, 0x03 };
const uint8 DHCP_PACK[3]   		= { 0x35, 0x01, 0x05 };
const uint8 DHCP_PNAK[3]   		= { 0x35, 0x01, 0x06 };
const uint8 DHCP_CLIENT_ID[3]	= { 0x3D, 0x07, 0x01 };
const uint8 DHCP_REQUEST_IP[2]  = { 0x32, 0x04 };
const uint8 DHCP_STOPPER		= 0xFF;




//#define __W5200_LCD_DEBUG_RX__
//#define __W5200_LCD_DEBUG_TX__





//======================================================================================================================
// DHCP関連
//======================================================================================================================
//----------------------------------------------------------------------------------------------------------------------
// DHCP Option解析（SubnetとGatewayを取得）
//----------------------------------------------------------------------------------------------------------------------
void analyze_dhcp_option( PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;
	uint8					*pPos = &ptDhcpMessage->options[7];
	uint16					i = 0;
	uint8					tag = 0;
	uint8					size = 0;


	while ( 1 )
	{
		// Stopper?
		if ( pPos[ i ] == DHCP_STOPPER )
		{
			break;
		}
		tag = pPos[ i++ ];
		size = pPos[ i++ ];

		if ( size == 0 )
		{
			break;
		}

		// Subnet, Gatewayの情報なら取得
		switch ( tag ) {
		case 1:		// Subnet Mask
			memcpy( &ptDhcpInfo->Subnet[0], &pPos[i], size );
			break;
		case 3:		// Router
			if ( size == 4 )
			{
				memcpy( &ptDhcpInfo->Gateway[0], &pPos[i], size );
			}
			break;
		default:
			break;
		}
		i = i + size;
	}

	return;
}



//----------------------------------------------------------------------------------------------------------------------
// DHCP PACK応答 or PNAK応答受信
//----------------------------------------------------------------------------------------------------------------------
bool dhcp_recv_ack( PUDP_DATA_FORMAT_TABLE ptUdpData, bool *pbAckFlag )
{
	bool					bRet = false;
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;

	
	// 受信チェック
	*pbAckFlag = false;
	if ( W5200_UDP_RecvCheck( UDP_SOCKET ) == true )
	{	
		// UDP受信
		W5200_UDP_Recv( UDP_SOCKET, ptUdpData );	

		// DHCPヘッダー & PACK応答か調べる
		if ( (memcmp(&ptDhcpMessage->options[0], DHCP_HEADER, 4) == 0) && 
		     (memcmp(&ptDhcpMessage->options[4], DHCP_PACK  , 3) == 0) )
		{
			*pbAckFlag = true;
			bRet = true;
		}
		// DHCPヘッダー & PNAK応答か調べる
		else if ( (memcmp(&ptDhcpMessage->options[0], DHCP_HEADER, 4) == 0) && 
		     	  (memcmp(&ptDhcpMessage->options[4], DHCP_PNAK  , 3) == 0) )
		{
			*pbAckFlag = false;
			bRet = true;
		}
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// DHCP Subnet & Gateway取得
//----------------------------------------------------------------------------------------------------------------------
bool dhcp_get_subnetmask( uint8 *pMacAddress, PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	bool					bAckFlag = false;
	bool					bRet = false;
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;
	uint16					i = 0;

	memset( ptUdpData, 0x00, sizeof(UDP_DATA_FORMAT_TABLE) );
	ptUdpData->IP[0] = 255;
	ptUdpData->IP[1] = 255;
	ptUdpData->IP[2] = 255;
	ptUdpData->IP[3] = 255;
	ptUdpData->Port  = DCHP_SERVER_PORT;
	ptUdpData->DataSize = sizeof(DHCP_MESSAGE_TABLE);
	
	// DHCPメッセージ部
	ptDhcpMessage->op		= 0x01;
	ptDhcpMessage->htype	= 0x01;
	ptDhcpMessage->hlen		= 0x06;
	ptDhcpMessage->hops		= 0x00;

	memcpy( &ptDhcpMessage->yiaddr[0], &ptDhcpInfo->IP[0], 4 );			// IP Address

	memcpy( &ptDhcpMessage->chaddr[0], &pMacAddress[0], 6 );			// MAC Address

	memcpy( &ptDhcpMessage->options[0], DHCP_HEADER, 4 );				// DHCP_HEADER
	memcpy( &ptDhcpMessage->options[4], DHCP_REQUEST, 3 );				// DHCP_REQUEST
	memcpy( &ptDhcpMessage->options[7], DHCP_CLIENT_ID, 3 );			// CLIENT ID

	memcpy( &ptDhcpMessage->options[10], &pMacAddress[0], 6 );			// MAC Address
	memcpy( &ptDhcpMessage->options[16], DHCP_REQUEST_IP, 2 );			// DHCP_REQUEST_IP
	memcpy( &ptDhcpMessage->options[18], &ptDhcpInfo->IP[0], 4 );		// IP Address

	ptDhcpMessage->options[22]	= 0x37;			// Request List
	ptDhcpMessage->options[23]	= 0x06;			// 6個あるよ！
	ptDhcpMessage->options[24]	= 0x01;			// Subnet
	ptDhcpMessage->options[25]	= 0x03;			// Router
	ptDhcpMessage->options[26]	= 0x06;			// Domain Name Server
	ptDhcpMessage->options[27]	= 0x0f;			// Domain name
	ptDhcpMessage->options[28]	= 0x1c;			// Broadcast Address
	ptDhcpMessage->options[29]	= 0x21;			// Static Route

	ptDhcpMessage->options[34] = DHCP_STOPPER;							// = Stopper =

	// Request 送信
	W5200_UDP_Send( UDP_SOCKET, ptUdpData );
	W5200_UDP_SendCheck( UDP_SOCKET ); 

	for ( i = 0 ; i < 1000 ; i++ )
	{
		// PACK応答 or PNAK応答受信
		bAckFlag = false;
		if ( dhcp_recv_ack( ptUdpData, &bAckFlag ) == true )
		{
			if ( bAckFlag == true )
			{
				analyze_dhcp_option( ptUdpData, ptDhcpInfo );
				bRet = true;
				break;
			}
		}
		_delay_us( 50 );
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// DHCP REQUEST送信
//----------------------------------------------------------------------------------------------------------------------
bool dhcp_send_request( uint8 *pMacAddress, PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;
	uint16					i = 0;
	bool					bAckFlag = false;


	ptUdpData->IP[0] = 255;
	ptUdpData->IP[1] = 255;
	ptUdpData->IP[2] = 255;
	ptUdpData->IP[3] = 255;
	ptUdpData->Port  = DCHP_SERVER_PORT;
	ptUdpData->DataSize = sizeof(DHCP_MESSAGE_TABLE);
	
	// DHCPメッセージ部
	memset( ptUdpData->DataBuff, 0x00, W5200_UDP_BUFFER_SIZE );
	ptDhcpMessage->op		= 0x01;
	ptDhcpMessage->htype	= 0x01;
	ptDhcpMessage->hlen		= 0x06;
	ptDhcpMessage->hops		= 0x00;

	memcpy( &ptDhcpMessage->yiaddr[0], &ptDhcpInfo->IP[0], 4 );			// IP Address

	memcpy( &ptDhcpMessage->chaddr[0], &pMacAddress[0], 6 );			// MAC Address

	memcpy( &ptDhcpMessage->options[0], DHCP_HEADER, 4 );				// DHCP_HEADER
	memcpy( &ptDhcpMessage->options[4], DHCP_REQUEST, 3 );				// DHCP_REQUEST
	memcpy( &ptDhcpMessage->options[7], DHCP_CLIENT_ID, 3 );			// CLIENT ID
	memcpy( &ptDhcpMessage->options[10], &pMacAddress[0], 6 );			// MAC Address
	memcpy( &ptDhcpMessage->options[16], DHCP_REQUEST_IP, 2 );			// DHCP_REQUEST_IP
	memcpy( &ptDhcpMessage->options[18], &ptDhcpInfo->IP[0], 4 );		// IP Address
	ptDhcpMessage->options[22] = DHCP_STOPPER;							// = Stopper =
	
	// Request 送信
	W5200_UDP_Send( UDP_SOCKET, ptUdpData );
	W5200_UDP_SendCheck( UDP_SOCKET ); 

	for ( i = 0 ; i < 1000 ; i++ )
	{
		// PACK応答 or PNAK応答受信
		if ( dhcp_recv_ack( ptUdpData, &bAckFlag ) == true )
		{
			break;
		}
		_delay_us( 50 );
	}

	return bAckFlag;
}


//----------------------------------------------------------------------------------------------------------------------
// DHCP OFFER受信
//----------------------------------------------------------------------------------------------------------------------
bool dhcp_recv_offer( PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	bool					bRet = false;
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;
	
	
	// 受信チェック
	if ( W5200_UDP_RecvCheck( UDP_SOCKET ) == true )
	{
		// UDP受信
		W5200_UDP_Recv( UDP_SOCKET, ptUdpData );
#if 0
		sprintf( g_szLcdBuff, "0x%02X,0x%02X,0x%02X,0x%02X", ptDhcpMessage->options[0], ptDhcpMessage->options[1], ptDhcpMessage->options[2], ptDhcpMessage->options[3]);
		LCD_DEBUG_OUT( 2, g_szLcdBuff );
		sprintf( g_szLcdBuff, "0x%02X,0x%02X,0x%02X", ptDhcpMessage->options[4], ptDhcpMessage->options[5], ptDhcpMessage->options[6] );
		LCD_DEBUG_OUT( 3, g_szLcdBuff );
		_delay_ms( 500 );
#endif
		// DHCPヘッダー & OFFER応答か調べる
		if ( (memcmp(&ptDhcpMessage->options[0], DHCP_HEADER, 4) == 0) && 
		     (memcmp(&ptDhcpMessage->options[4], DHCP_OFFER , 3) == 0) )
		{
			// とりあえずDHCPサーバーから教えてもらったIPアドレスを保存
			memcpy( &ptDhcpInfo->IP[0], &ptDhcpMessage->yiaddr[0], 4 );
			bRet = true;
		}
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// DHCP DISCOVER送信
//----------------------------------------------------------------------------------------------------------------------
bool dhcp_send_discover( uint8 *pMacAddress, PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	bool					bFlag = false;
	PDHCP_MESSAGE_TABLE		ptDhcpMessage = (PDHCP_MESSAGE_TABLE)ptUdpData->DataBuff;
	uint8					retry_count = 0;
	uint16					i = 0;


retry_dhcp_send_discover:

	memset( ptUdpData, 0x00, sizeof(UDP_DATA_FORMAT_TABLE) );
	ptUdpData->IP[0] = 255;
	ptUdpData->IP[1] = 255;
	ptUdpData->IP[2] = 255;
	ptUdpData->IP[3] = 255;
	ptUdpData->Port  = DCHP_SERVER_PORT;
	ptUdpData->DataSize = sizeof(DHCP_MESSAGE_TABLE);
	
	// DHCPメッセージ部
	ptDhcpMessage->op		= 0x01;
	ptDhcpMessage->htype	= 0x01;
	ptDhcpMessage->hlen		= 0x06;
	ptDhcpMessage->hops		= 0x00;

	memcpy( &ptDhcpMessage->chaddr[0], &pMacAddress[0], 6 );			// MAC Address

	memcpy( &ptDhcpMessage->options[0], DHCP_HEADER, 4 );				// DHCP_HEADER
	memcpy( &ptDhcpMessage->options[4], DHCP_DISCOVER, 3 );				// DISCOVER
	memcpy( &ptDhcpMessage->options[7], DHCP_CLIENT_ID, 3 );			// CLIENT ID
	memcpy( &ptDhcpMessage->options[10], &pMacAddress[0], 6 );			// MAC Address
	ptDhcpMessage->options[16] = DHCP_STOPPER;							// = Stopper =

	// Discover 送信
	W5200_UDP_Send( UDP_SOCKET, ptUdpData );
	W5200_UDP_SendCheck( UDP_SOCKET ); 

	// OFFER 受信
	for ( i = 0 ; i < 1000 ; i++  )
	{
		if ( dhcp_recv_offer( ptUdpData, ptDhcpInfo ) == true )
		{
			// Request 送信
			if ( dhcp_send_request( pMacAddress, ptUdpData, ptDhcpInfo ) == true )
			{
				bFlag = true;
				break;
			}
			else
			{
				break;
			}
		}
		_delay_us( 50 );
	}

	if ( bFlag == false )
	{
		retry_count++;
		if (  retry_count <= 3 )
		{
			goto retry_dhcp_send_discover;
		}
	}

	return bFlag;
}





//======================================================================================================================
// SPI関連
//======================================================================================================================
//----------------------------------------------------------------------------------------------------------------------
// SPI 初期化（主装置用）
//----------------------------------------------------------------------------------------------------------------------
void spi_master_init(void)
{
	// PORTB設定：MOSI, SCK, /SS, /RESETを出力に設定
	DDRB  = (MOSI_PIN | SCK_PIN | SS_PIN | RESET_PIN);
	PORTB = (SS_PIN);
	
	// SPI設定
#if 0
	// ATmega88P(1MHz)
	// マスター、クロックモード = 3、f_sck = f_osc / 4
	// (SPI割込み無効、MSBから転送）
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);
#else
	// ATmega644P(20MHz)
	SPCR = _BV(SPE) | _BV(MSTR)  | _BV(CPOL) | _BV(CPHA) | _BV(SPR1);
	SPSR = _BV(SPI2X);
#endif

}


//----------------------------------------------------------------------------------------------------------------------
// SPI データ転送（主装置用）
//----------------------------------------------------------------------------------------------------------------------
uint8 spi_master_transmit( uint8 SendData )
{
	uint8		RecvData = 0x00;
	
	
	SPDR = SendData;										// データ送信
	while( !(SPSR & (1<<SPIF)) );							// データ送信完了(SPIF)を待つ
	RecvData = SPDR;										// データ受信
	
	return RecvData;
}


//----------------------------------------------------------------------------------------------------------------------
// SPI開始
//----------------------------------------------------------------------------------------------------------------------
void spi_start(void)
{
	// /SS = lowにする。SPI転送開始をスレーブに伝える
	PORTB &= (~SS_PIN);
}


//----------------------------------------------------------------------------------------------------------------------
// SPI終了
//----------------------------------------------------------------------------------------------------------------------
void spi_end(void)
{
	// /SS = highにする。SPI転送終了をスレーブに伝える
	PORTB |= (SS_PIN);
}



//======================================================================================================================
// Wiznet W5200関連
//======================================================================================================================
//----------------------------------------------------------------------------------------------------------------------
// W5200 データ読込み
//----------------------------------------------------------------------------------------------------------------------
void w5200_read( uint16 Address, uint8 *pReadData, uint16 ReadSize )
{
	uint16				index = 0;
	

	// SPI開始
	spi_start();

	// 読込み先アドレスセット
	spi_master_transmit( ((Address & 0xFF00) >> 8) );
	spi_master_transmit( (Address & 0x00FF) );
	
	// コマンド・読込みデータサイズをセット
	spi_master_transmit( ((W5200_DATA_READ_COMMAND | (ReadSize & 0x7F00)) >> 8) );
	spi_master_transmit( (ReadSize & 0x00FF) );
	
	// データ読込み
	for ( index = 0 ; index < ReadSize ; index++ )
	{
		pReadData[ index ] = spi_master_transmit( 0 );					// W5200にダミーデータを送信して、W5200からのデータを取得
	}

	// SPI終了
	spi_end();
}


//----------------------------------------------------------------------------------------------------------------------
// W5200 データ読込み(Byte)
//----------------------------------------------------------------------------------------------------------------------
uint8 w5200_byte_read( uint16 Address )
{
	uint8		ReadData = 0;
	
	
	w5200_read( Address, &ReadData, 1 );

	return ( ReadData );
}


//----------------------------------------------------------------------------------------------------------------------
// W5200 データ読込み(Word)
//----------------------------------------------------------------------------------------------------------------------
uint16 w5200_word_read( uint16 Address )
{
	uint16			ReadData = 0;
	uint8			temp1 = 0;
	uint8			temp2 = 0;


	w5200_read( (Address + 0), &temp1, 1 );
	w5200_read( (Address + 1), &temp2, 1 );
	ReadData = temp1;
	ReadData = (ReadData << 8 ) + temp2;

	return ( ReadData );
}



//----------------------------------------------------------------------------------------------------------------------
// W5200 データ書込み
//----------------------------------------------------------------------------------------------------------------------
void w5200_write( uint16 Address, uint8 *pWriteData, uint16 WriteSize )
{
	uint16				index = 0;
	

	// SPI開始
	spi_start();

	// 読込み先アドレスセット
	spi_master_transmit( ((Address & 0xFF00) >> 8) );
	spi_master_transmit( (Address & 0x00FF) );
	
	// コマンド・書込みデータサイズをセット
	spi_master_transmit( ((W5200_DATA_WRITE_COMMAND | (WriteSize & 0x7F00)) >> 8) );
	spi_master_transmit( (WriteSize & 0x00FF) );
	
	// データ読込み
	for ( index = 0 ; index < WriteSize ; index++ )
	{
		spi_master_transmit( pWriteData[ index ] );
	}

	// SPI終了
	spi_end();
}


//----------------------------------------------------------------------------------------------------------------------
// W5200 データ書込み(Byte)
//----------------------------------------------------------------------------------------------------------------------
void w5200_byte_write( uint16 Address, uint8 WriteData )
{
	w5200_write( Address, &WriteData, 1 );
}


//----------------------------------------------------------------------------------------------------------------------
// W5200 データ書込み(Word)
//----------------------------------------------------------------------------------------------------------------------
void w5200_word_write( uint16 Address, uint16 WriteData )
{
	uint8			temp1 = 0;
	uint8			temp2 = 0;
	
	
	temp1 = (uint8)((WriteData & 0xFF00) >> 8);
	temp2 = (uint8)(WriteData & 0x00FF);
	w5200_write( (Address + 0), &temp1, 1 );
	w5200_write( (Address + 1), &temp2, 1 );
}



//----------------------------------------------------------------------------------------------------------------------
// W5200リセット処理
//----------------------------------------------------------------------------------------------------------------------
void w5200_reset(void)
{
	// アクティブLowにして、W5200をリセット開始信号を通知
	PORTB = PORTB & ~RESET_PIN;
	_delay_us( RESET_ACTIVE_LOW_TIME );

	// リセット開始信号を元にもどす
	PORTB = PORTB | RESET_PIN;
	_delay_ms( RESET_WAIT_TIME );
}


//----------------------------------------------------------------------------------------------------------------------
// W5200 送信・受信用ベースアドレスなどを初期化
//----------------------------------------------------------------------------------------------------------------------
void w5200_memory_init(void)
{
	SOCKET			s = 0;
	
	
	// 送信用メモリ初期化
	for ( s = 0 ; s < W5200_SOCKET_NUM ; s++ )
	{
		g_Sn_TX_BASE[ s ] = W5200_TX_MEMORY_BASE + (W5200_TX_MEMORY_SIZE * s);
		g_Sn_TX_MASK[ s ] = W5200_TX_MEMORY_SIZE - 1;
		w5200_byte_write( Sn_TXMEM_SIZE(s), W5200_TX_MEMORY_NUM );
	}
	
	// 受信用メモリ初期化
	for ( s = 0 ; s < W5200_SOCKET_NUM ; s++ )
	{
		g_Sn_RX_BASE[ s ] = W5200_RX_MEMORY_BASE + ( W5200_RX_MEMORY_SIZE * s);
		g_Sn_RX_MASK[ s ] = W5200_RX_MEMORY_SIZE - 1;
		w5200_byte_write( Sn_RXMEM_SIZE(s), W5200_RX_MEMORY_NUM );
	}
	
	return;
}


//----------------------------------------------------------------------------------------------------------------------
// W5200初期化処理
//----------------------------------------------------------------------------------------------------------------------
void W5200_Init(void)
{
	// SPIの初期化
	spi_master_init();
	
	// w5200 リセット
	w5200_reset();
	
	// 送信・受信用ベースアドレスなどを初期化
	w5200_memory_init();
}



//----------------------------------------------------------------------------------------------------------------------
// ゲートウェイ設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setGAR( const uint8 *pGateway )
{
	w5200_write( GAR0, (uint8 *)pGateway, 4 );
}

//----------------------------------------------------------------------------------------------------------------------
// サブネットマスク設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSUB( const uint8 *pSubnet )
{
	w5200_write( SUBR0, (uint8 *)pSubnet, 4 );
}

//----------------------------------------------------------------------------------------------------------------------
// MACアドレス設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSHAR( const uint8 *pMacAddress )
{
	w5200_write( SHAR0, (uint8 *)pMacAddress, 6 );
}

//----------------------------------------------------------------------------------------------------------------------
// IPアドレス設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSIPR( const uint8 *pIpAddress )
{
	w5200_write( SIPR0, (uint8 *)pIpAddress, 4 );
}



//----------------------------------------------------------------------------------------------------------------------
// UDPオープン
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_OPEN( SOCKET s, uint16 Port )
{
	bool		bRet = false;
	uint8		Command = 0x00;
	uint8		Status  = 0x00;
	uint16		count = 0;

	
	// Socket Mode Set(UDP)
	w5200_byte_write( Sn_MR(s), W5200_MR_UDP );
	
	// Source Port Set
	w5200_word_write( Sn_PORT0(s), Port );
	
	// Socket Command(OPEN)
	w5200_byte_write( Sn_CR(s), W5200_CR_OPEN );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}

	// Socket Statusが[SOCK_UDP]
	for ( count = 0 ; count < 1000 ; count++ )
	{
		Status = w5200_byte_read( Sn_SR(s) );
		if ( Status == W5200_SR_SOCK_UDP )
		{
			bRet = true;
			break;
		}
		_delay_us( 10 );
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// UDP受信チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_RecvCheck( SOCKET s )
{
	bool		bRet = false;
	uint8		ir = 0x00;
#ifdef __W5200_LCD_DEBUG_RX__
	uint16		RX_Read = 0x0000;
	uint16		RX_Write = 0x0000;
	uint16		RX_Recv_Size = 0x0000;

	
	RX_Read  = w5200_word_read( Sn_RX_RD0(s) );
	RX_Write = w5200_word_read( Sn_RX_WR0(s) );
	RX_Recv_Size = w5200_word_read( Sn_RX_RSR0(s) );

	sprintf( (char *)g_szLcdBuff, "RX_Read  : 0x%04X", RX_Read );
	LCD_DEBUG_OUT( 0, g_szLcdBuff );

	sprintf( (char *)g_szLcdBuff, "RX_Write : 0x%04X", RX_Write );
	LCD_DEBUG_OUT( 1, g_szLcdBuff );

	sprintf( (char *)g_szLcdBuff, "RX_Size  : 0x%04X", RX_Recv_Size );
	LCD_DEBUG_OUT( 2, g_szLcdBuff );

//	if ( RX_Write >= 0x1FE0 )
//	{
//		_delay_ms( 3000 );
//	}
#endif

	ir = w5200_byte_read( Sn_IR(s) );
	if ( (ir & W5200_SnIR_RECV) == W5200_SnIR_RECV )
	{
		w5200_byte_write( Sn_IR(s), W5200_SnIR_RECV );
		bRet = true;
	}

	return ( bRet );
}


//----------------------------------------------------------------------------------------------------------------------
// UDP受信
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Recv( SOCKET s, PUDP_DATA_FORMAT_TABLE ptUdpRecvData )
{
	uint8			header[ UDP_HEADER_SIZE ];
	uint8			Command = 0x00;
	uint16			index = 0;
	uint16			Sn_RX_RD = 0x0000;
	uint16			scr_mask = 0x0000;
	uint16			scr_ptr  = 0x0000;
	uint16			upper_size = 0;
	uint16			left_size  = 0;
	uint16			count = 0;

	
	memset( ptUdpRecvData, 0x00, sizeof(UDP_DATA_FORMAT_TABLE) );

	// 受信用読込みアドレスを取得
	Sn_RX_RD = w5200_word_read( Sn_RX_RD0(s) );
	scr_mask = (Sn_RX_RD & g_Sn_RX_MASK[ s ]);
	scr_ptr  = g_Sn_RX_BASE[ s ] + scr_mask;

#ifdef __W5200_LCD_DEBUG_RX__
	uint16		RX_Read = 0x0000;
	uint16		RX_Write = 0x0000;
	uint16		RX_Recv_Size = 0x0000;

	
	RX_Read  = w5200_word_read( Sn_RX_RD0(s) );
	RX_Write = w5200_word_read( Sn_RX_WR0(s) );
	RX_Recv_Size = w5200_word_read( Sn_RX_RSR0(s) );

	sprintf( (char *)g_szLcdBuff, "RX_Read  : 0x%04X", RX_Read );
	LCD_DEBUG_OUT( 0, g_szLcdBuff );

	sprintf( (char *)g_szLcdBuff, "RX_Write : 0x%04X", RX_Write );
	LCD_DEBUG_OUT( 1, g_szLcdBuff );

	sprintf( (char *)g_szLcdBuff, "RX_Size  : 0x%04X", RX_Recv_Size );
	LCD_DEBUG_OUT( 2, g_szLcdBuff );

	if ( RX_Recv_Size >= 20 )
	{
		_delay_ms( 3000 );
	}
#endif

#ifdef __W5200_LCD_DEBUG_RX__
//	sprintf( (char *)g_szLcdBuff, "Sn_RX_RD : 0x%04X", Sn_RX_RD );
//	LCD_DEBUG_OUT( 1, g_szLcdBuff );
//	sprintf( (char *)g_szLcdBuff, "scr_mask : 0x%04X", scr_mask );
//	LCD_DEBUG_OUT( 2, g_szLcdBuff );
//	sprintf( (char *)g_szLcdBuff, "scr_ptr  : 0x%04X", scr_ptr );
//	LCD_DEBUG_OUT( 3, g_szLcdBuff );
#endif

	// ヘッダー情報読込み
	index = 0;
	if ( (scr_mask + UDP_HEADER_SIZE) > (g_Sn_RX_MASK[ s ] + 1) )							// ヘッダー情報を読む際、RX_MEMORYの領域を超えていないかを確認
	{
		upper_size = (g_Sn_RX_MASK[ s ] + 1) - scr_mask;									// オーバーフローする前までのデータを取得
		w5200_read( scr_ptr, &header[ index ], upper_size );
		
		index += upper_size;
		left_size = UDP_HEADER_SIZE - upper_size;
		
		w5200_read( g_Sn_RX_BASE[ s ], &header[ index ], left_size );						// オーバーフローしたデータを取得
		
		scr_mask = left_size;
	}
	else
	{
		w5200_read( scr_ptr, &header[ index ], UDP_HEADER_SIZE );
		scr_mask += UDP_HEADER_SIZE;
	}
	
	// 読み込んだヘッダー情報をUPDデータ構造体にセット
	ptUdpRecvData->IP[ 0 ] = header[ 0 ];													// IP
	ptUdpRecvData->IP[ 1 ] = header[ 1 ];
	ptUdpRecvData->IP[ 2 ] = header[ 2 ];
	ptUdpRecvData->IP[ 3 ] = header[ 3 ];
	ptUdpRecvData->Port = header[ 4 ];														// Port
	ptUdpRecvData->Port = (ptUdpRecvData->Port << 8) | ((uint16)header[ 5 ]);
	ptUdpRecvData->DataSize = header[ 6 ];													// Dataサイズ
	ptUdpRecvData->DataSize = (ptUdpRecvData->DataSize << 8) | ((uint16)header[ 7 ]);

	// 読込みアドレスを更新
	scr_ptr  = g_Sn_RX_BASE[ s ] + scr_mask;
	
	// 受信データ読込み
	index = 0;
	if ( (scr_mask + ptUdpRecvData->DataSize) > (g_Sn_RX_MASK[ s ] + 1) )					// 受信データを読む際、RX_MEMORYの領域を超えていないかを確認
	{
		upper_size = (g_Sn_RX_MASK[ s ] + 1) - scr_mask;									// オーバーフローする前までのデータを取得
		w5200_read( scr_ptr, &ptUdpRecvData->DataBuff[ index ], upper_size );

		index += upper_size;
		left_size = ptUdpRecvData->DataSize - upper_size;

		w5200_read( g_Sn_RX_BASE[ s ], &ptUdpRecvData->DataBuff[ index ], left_size );		// オーバーフローしたデータを取得
	}
	else
	{
		w5200_read( scr_ptr, &ptUdpRecvData->DataBuff[ index ], ptUdpRecvData->DataSize );
	}

	// 受信用読込みアドレスを更新
	Sn_RX_RD = Sn_RX_RD + UDP_HEADER_SIZE + ptUdpRecvData->DataSize;

#ifdef __W5200_LCD_DEBUG_RX__
	sprintf( (char *)g_szLcdBuff, "=>%s : 0x%04X", ptUdpRecvData->DataBuff, Sn_RX_RD );
	LCD_DEBUG_OUT( 3, g_szLcdBuff );
#endif

	w5200_word_write( Sn_RX_RD0(s), Sn_RX_RD );

	// [RECV]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_RECV );
	for ( count = 0 ; count < 1000 ; count++ )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
		_delay_us( 10 );
	}

	return;
}


//----------------------------------------------------------------------------------------------------------------------
// UDP送信チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_SendCheck( SOCKET s )
{
	bool		bRet = false;
	uint16		Count = 0;

	for ( Count = 0 ; Count < 1000 ; Count++ )
	{
		if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_SEND_OK )
		{
			w5200_byte_write(Sn_IR(s), W5200_SnIR_SEND_OK);
			bRet = true;
			break;
		}
		_delay_ms( 50 );
	}

	return ( bRet );
}


//----------------------------------------------------------------------------------------------------------------------
// UDP送信
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Send( SOCKET s, PUDP_DATA_FORMAT_TABLE ptUdpSendData )
{
	uint16			FreeSize = 0;
	uint8			Command = 0x00;
	uint16			index = 0;
	uint16			Sn_TX_WR = 0x0000;
	uint16			dst_mask = 0x0000;
	uint16			dst_ptr  = 0x0000;
	uint16			upper_size = 0;
	uint16			left_size  = 0;

	
	// 送信できる空き領域ができるまで待つ(※永久ループになる可能性あり)
	FreeSize = w5200_word_read( Sn_TX_FSR0(s) );
	while( FreeSize < ptUdpSendData->DataSize )
	{
		FreeSize = w5200_word_read( Sn_TX_FSR0(s) );
	}
	
	// 送信先IPアドレスをセット
	w5200_write( Sn_DIPR0(s), ptUdpSendData->IP, 4 );

	// 送信先Portをセット
	w5200_word_write( Sn_DPORT0(s), ptUdpSendData->Port );
	
	// 送信用書込みアドレスを取得
	Sn_TX_WR = w5200_word_read( Sn_TX_WR0(s) );
	dst_mask = Sn_TX_WR & g_Sn_TX_MASK[ s ];
	dst_ptr  = g_Sn_TX_BASE[ s ] + dst_mask;

#ifdef __W5200_LCD_DEBUG_TX__
	sprintf( (char *)g_szLcdBuff, "Sn_TX_WR : 0x%04X", Sn_TX_WR );
	LCD_DEBUG_OUT( 0, g_szLcdBuff );
	sprintf( (char *)g_szLcdBuff, "dst_mask : 0x%04X", dst_mask );
	LCD_DEBUG_OUT( 1, g_szLcdBuff );
	sprintf( (char *)g_szLcdBuff, "dst_ptr  : 0x%04X", dst_ptr );
	LCD_DEBUG_OUT( 2, g_szLcdBuff );
#endif

	// 送信データ書込み
	index = 0;
	if ( (dst_mask + ptUdpSendData->DataSize) > (g_Sn_TX_MASK[ s ] + 1) )					// 送信データを書き込む際、TX_MEMORYの領域を超えていないかを確認
	{
		upper_size = (g_Sn_TX_MASK[ s ] + 1) - dst_mask;									// オーバーフローする前までのデータを書き込む
		w5200_write( dst_ptr, &ptUdpSendData->DataBuff[ index ], upper_size );

		index += upper_size;
		left_size = ptUdpSendData->DataSize - upper_size;
		w5200_write( g_Sn_TX_BASE[ s ], &ptUdpSendData->DataBuff[ index ], left_size );		// オーバーフローしたデータを書き込む
	}
	else
	{
		w5200_write( dst_ptr, &ptUdpSendData->DataBuff[ index ], ptUdpSendData->DataSize );
	}

	// 送信用書込みアドレスを更新
//	Sn_TX_WR = (Sn_TX_WR + ptUdpSendData->DataSize) & g_Sn_TX_MASK[ s ];
	Sn_TX_WR = (Sn_TX_WR + ptUdpSendData->DataSize);
	w5200_word_write( Sn_TX_WR0(s), Sn_TX_WR );

#ifdef __W5200_LCD_DEBUG_TX__
	sprintf( (char *)g_szLcdBuff, "Sn_TX_WR : 0x%04X", Sn_TX_WR );
	LCD_DEBUG_OUT( 3, g_szLcdBuff );
#endif

	// [SEND]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_SEND );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}
	
	return;
}


//----------------------------------------------------------------------------------------------------------------------
// UDPクローズ
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Close( SOCKET s )
{
	uint8			ir = 0x00;
	uint8			Command = 0x00;
	
	
	w5200_byte_write( Sn_IR(s), 0xFF );
	while( 1 )
	{
		ir = w5200_byte_read( Sn_IR(s) );
		if ( ir == 0x00 ) break;
	}
	
	// [CLOSE]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_CLOSE );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}
	
	return;
}


//----------------------------------------------------------------------------------------------------------------------
// LAN接続チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_LAN_Check(void)
{
	bool		bRet = false;
	uint8		PHY_Status = 0x00;


	// PHY STATUS取得
	PHY_Status = w5200_byte_read( PSTATUS );
	if ( PHY_Status & 0x20 )
	{
		bRet = true;
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// DHCPクライアント
//----------------------------------------------------------------------------------------------------------------------
bool W5200_DHCP_Client( uint8 *pMacAddress, PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo )
{
	bool		bRet = false;

	// UDPオープン
	if( W5200_UDP_OPEN( UDP_SOCKET, DCHP_CLIENT_PORT ) == false )
	{
		return false;
	}

	// DHCP DISCOVER送信
	bRet = dhcp_send_discover( pMacAddress, ptUdpData, ptDhcpInfo );
	if ( bRet == true )
	{
		bRet = dhcp_get_subnetmask( pMacAddress, ptUdpData, ptDhcpInfo );
	}

	// UDPクローズ
	W5200_UDP_Close( UDP_SOCKET );


	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCPオープン
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Open( SOCKET s, uint16 Port )
{
	bool		bRet = false;
	uint8		Command = 0x00;
	uint8		Status  = 0x00;
	uint16		count = 0;


	// Socket Mode Set(TCP)
	w5200_byte_write( Sn_MR(s), W5200_MR_TCP );

	// Source Port Set
	w5200_word_write( Sn_PORT0(s), Port );

	// Socket Command(OPEN)
	w5200_byte_write( Sn_CR(s), W5200_CR_OPEN );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}
	
	// Socket Statusが[SOCK_INIT]
	for ( count = 0 ; count < 1000 ; count++ )
	{
		Status = w5200_byte_read( Sn_SR(s) );
		if ( Status == W5200_SR_SOCK_INIT )
		{
			bRet = true;
			break;
		}
		_delay_us( 10 );
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP LISTEN (TCP SERVER)
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Listen( SOCKET s )
{
	bool		bRet = false;
	uint8		Command = 0x00;
	uint8		Status  = 0x00;


	// Socket Statusが[SOCK_INIT]
	Status = w5200_byte_read( Sn_SR(s) );
	if ( Status == W5200_SR_SOCK_INIT )
	{
		// Socket Command(LISTEN)
		w5200_byte_write( Sn_CR(s), W5200_CR_LISTEN );
		while ( 1 )
		{
			Command = w5200_byte_read( Sn_CR(s) );
			if ( Command == 0x00 ) break;
		}

		// Socket Statusが[SOCK_LISTEN]
		while ( 1 )
		{
			Status = w5200_byte_read( Sn_SR(s) );
			if ( Status == W5200_SR_SOCK_LISTEN )
			{
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}



//----------------------------------------------------------------------------------------------------------------------
// TCP CONNECT (TCP CLIENT)
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Connect( SOCKET s, uint8 *pIP, uint16 Port )
{
	bool		bRet = false;
	uint8		Command = 0x00;


	// 接続先IPアドレスをセット
	w5200_write( Sn_DIPR0(s), pIP, 4 );

	// 接続先Portをセット
	w5200_word_write( Sn_DPORT0(s), Port );
	
	// Socket Command(CONNECT)
	w5200_byte_write( Sn_CR(s), W5200_CR_CONNECT );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 )
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}



//----------------------------------------------------------------------------------------------------------------------
// TCP ESTABLISHMENT
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Establishment( SOCKET s )
{
	bool		bRet = false;


	if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_CON )
	{
		w5200_byte_write(Sn_IR(s), W5200_SnIR_CON);
		bRet = true;
	}


	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP RecvCheck
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_RecvCheck( SOCKET s )
{
	bool		bRet = false;


	if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_RECV )
	{
		w5200_byte_write(Sn_IR(s), W5200_SnIR_RECV);
		bRet = true;
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP 受信
//----------------------------------------------------------------------------------------------------------------------
uint16 W5200_TCP_Recv( SOCKET s, uint16 len, uint8 *pRecvData )
{
	uint8			Command = 0x00;
	uint16			index = 0;
	uint16			Sn_RX_RD = 0x0000;
	uint16			scr_mask = 0x0000;
	uint16			scr_ptr  = 0x0000;
	uint16			upper_size = 0;
	uint16			left_size  = 0;
	uint16			count = 0;


	if ( len == 0 ) return 0;

	// 受信用読込みアドレスを取得
	Sn_RX_RD = w5200_word_read( Sn_RX_RD0(s) );
	scr_mask = (Sn_RX_RD & g_Sn_RX_MASK[ s ]);
	scr_ptr  = g_Sn_RX_BASE[ s ] + scr_mask;
	
	// 受信データ読込み
	index = 0;
	if ( (scr_mask + len) > (g_Sn_RX_MASK[ s ] + 1) )										// 受信データを読む際、RX_MEMORYの領域を超えていないかを確認
	{
		upper_size = (g_Sn_RX_MASK[ s ] + 1) - scr_mask;									// オーバーフローする前までのデータを取得
		w5200_read( scr_ptr, &pRecvData[ index ], upper_size );

		index += upper_size;
		left_size = len - upper_size;

		w5200_read( g_Sn_RX_BASE[ s ], &pRecvData[ index ], left_size );					// オーバーフローしたデータを取得
	}
	else
	{
		w5200_read( scr_ptr, &pRecvData[ index ], len );
	}

	// 受信用読込みアドレスを更新
	Sn_RX_RD = Sn_RX_RD + len;
	w5200_word_write( Sn_RX_RD0(s), Sn_RX_RD );

	// [RECV]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_RECV );
	for ( count = 0 ; count < 1000 ; count++ )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
		_delay_us( 10 );
	}

	return len;
}
	

//----------------------------------------------------------------------------------------------------------------------
// TCP 送信
//----------------------------------------------------------------------------------------------------------------------
uint16 W5200_TCP_Send( SOCKET s, uint16 len, uint8 *pSendData )
{
	uint16			FreeSize = 0;
	uint8			Command = 0x00;
	uint16			index = 0;
	uint16			Sn_TX_WR = 0x0000;
	uint16			dst_mask = 0x0000;
	uint16			dst_ptr  = 0x0000;
	uint16			upper_size = 0;
	uint16			left_size  = 0;
	uint16			read_ptr_befor_send = 0x0000;
	uint16			read_ptr_after_send = 0x0000;
	uint16			ret = 0;

	
	// 送信できる空き領域ができるまで待つ(※永久ループになる可能性あり)
	FreeSize = w5200_word_read( Sn_TX_FSR0(s) );
	while( FreeSize < len )
	{
		FreeSize = w5200_word_read( Sn_TX_FSR0(s) );
	}
	
	// 送信用書込みアドレスを取得
	Sn_TX_WR = w5200_word_read( Sn_TX_WR0(s) );
	dst_mask = Sn_TX_WR & g_Sn_TX_MASK[ s ];
	dst_ptr  = g_Sn_TX_BASE[ s ] + dst_mask;

	// 送信データ書込み
	index = 0;
	if ( (dst_mask + len) > (g_Sn_TX_MASK[ s ] + 1) )						// 送信データを書き込む際、TX_MEMORYの領域を超えていないかを確認
	{
		upper_size = (g_Sn_TX_MASK[ s ] + 1) - dst_mask;					// オーバーフローする前までのデータを書き込む
		w5200_write( dst_ptr, &pSendData[ index ], upper_size );

		index += upper_size;
		left_size = len - upper_size;
		w5200_write( g_Sn_TX_BASE[ s ], &pSendData[ index ], left_size );	// オーバーフローしたデータを書き込む
	}
	else
	{
		w5200_write( dst_ptr, &pSendData[ index ], len );
	}

	// 送信用書込みアドレスを更新
	Sn_TX_WR = Sn_TX_WR + len;
	w5200_word_write( Sn_TX_WR0(s), Sn_TX_WR );

	// 送信前のSn_TX_RDを取得
	read_ptr_befor_send = w5200_word_read( Sn_TX_RD0(s) );

	// [SEND]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_SEND );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}

	// SEND OK
	while ( (w5200_word_read(Sn_IR(s)) & W5200_SnIR_SEND_OK) != W5200_SnIR_SEND_OK )
	{	
		// 相手が切断していた場合
		if ( (w5200_word_read(Sn_IR(s)) & W5200_SnIR_DISCON) == W5200_SnIR_DISCON )
		{
			// クローズ処理が必要？
			return 0;
		}
	}
	w5200_word_write( Sn_IR(s), W5200_SnIR_SEND_OK );

	// 送信後のSn_TX_RDを取得
	read_ptr_after_send = w5200_word_read( Sn_TX_RD0(s) );

	if ( read_ptr_after_send > read_ptr_befor_send )
	{
		ret = read_ptr_after_send - read_ptr_befor_send;
	}
	else
	{
		ret = (0xffff - read_ptr_befor_send) + read_ptr_after_send + 1;
	}

	return ret;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP CheckDisconnect
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckDisconnect( SOCKET s )
{
	bool		bRet = false;


	if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_DISCON )
	{
		w5200_byte_write(Sn_IR(s), W5200_SnIR_DISCON);
		bRet = true;
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP Disconnect
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Disconnect( SOCKET s )
{
	bool		bRet = false;
	uint8		Command = 0x00;


	// Socket Command(DISCONNECT)
	w5200_byte_write( Sn_CR(s), W5200_CR_DISCON );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 )
		{
			bRet = true;
			break;
		}
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP CheckClose
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckClose( SOCKET s )
{
	bool		bRet = false;


	if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_DISCON )
	{
//		w5200_byte_write(Sn_IR(s), W5200_SnIR_DISCON);
		bRet = true;
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP CheckTimeout
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckTimeout( SOCKET s )
{
	bool		bRet = false;


	if ( w5200_byte_read(Sn_IR(s)) == W5200_SnIR_TIME_OUT )
	{
		w5200_byte_write(Sn_IR(s), W5200_SnIR_TIME_OUT);
		bRet = true;
	}

	return bRet;
}


//----------------------------------------------------------------------------------------------------------------------
// TCP クローズ
//----------------------------------------------------------------------------------------------------------------------
void W5200_TCP_Close( SOCKET s )
{
	uint8			ir = 0x00;
	uint8			Command = 0x00;
	
	
	w5200_byte_write( Sn_IR(s), 0xFF );
	while( 1 )
	{
		ir = w5200_byte_read( Sn_IR(s) );
		if ( ir == 0x00 ) break;
	}
	
	// [CLOSE]コマンドをセット
	w5200_byte_write( Sn_CR(s), W5200_CR_CLOSE );
	while ( 1 )
	{
		Command = w5200_byte_read( Sn_CR(s) );
		if ( Command == 0x00 ) break;
	}
	
	return;
}





void W5200_TCP_Server( SOCKET s, uint8 *Ip, uint16 Port, uint16 len, uint8 *pData )
{

	switch ( w5200_byte_read(Sn_SR(s)) ) {
	case W5200_SR_SOCK_ESTABLISHED:
		if ( (len != 0) && (pData != NULL) )
		{
			W5200_TCP_Send( s, len, pData );
		}
		break;
	case W5200_SR_SOCK_CLOSE_WAIT:
		W5200_TCP_Disconnect( s );
		break;
	case W5200_SR_SOCK_CLOSED:
		W5200_TCP_Open( s, Port );
		break;
	case W5200_SR_SOCK_INIT:
		W5200_TCP_Listen( s );
		break;
	default:
		break;
	}
}




#if 0
void W5200_TCP_Client( SOCKET s )
{
	static	uint16	i = 0;
	uint8			szData[ 20 ];
	uint8			Ip[4] = { 192, 168, 11, 12 };


	switch ( w5200_byte_read(Sn_SR(s)) ) {
	case W5200_SR_SOCK_ESTABLISHED:
		sprintf( szData, "%u", i );
		W5200_TCP_Send( s, strlen(szData), szData );
		i++;
		break;
	case W5200_SR_SOCK_CLOSE_WAIT:
		W5200_TCP_Disconnect( s );
		break;
	case W5200_SR_SOCK_CLOSED:
		W5200_TCP_Open( s, 777 );
		break;
	case W5200_SR_SOCK_INIT:
		W5200_TCP_Connect( s, Ip, 10000 );
		break;
	default:
		break;
	}
}
		
#endif	

	
		

