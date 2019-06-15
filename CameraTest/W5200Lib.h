//********************************************************************************
// Wiznet社のW5200にアクセスするための関数群
//********************************************************************************
#ifndef __W5200_LIB_H__
#define __W5200_LIB_H__

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>



typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned char		SOCKET;


//-----------------------------------------------------------------------------
// UDP送受信データ構造体（mallocで領域を確保）
//-----------------------------------------------------------------------------
#define UDP_HEADER_SIZE				( 8 )
#define W5200_UDP_BUFFER_SIZE		( 1000 )

typedef struct
{
	uint8					IP[4];								// IP Address
	uint16					Port;								// Port
	uint16					DataSize;							// Dataサイズ
	uint8					DataBuff[ W5200_UDP_BUFFER_SIZE]; 
} UDP_DATA_FORMAT_TABLE, *PUDP_DATA_FORMAT_TABLE;



//-----------------------------------------------------------------------------
// DHCP受信データ構造体
//-----------------------------------------------------------------------------
typedef struct
{
	uint8					IP[4];				// IP Address
	uint8					Subnet[4];			// Subnet Mask
	uint8					Gateway[4];			// Gateway
} DHCP_INFO_TABLE, *PDHCP_INFO_TABLE;



//======================================================================================================================
// W5200 Registers
//======================================================================================================================
//----------------------------------------------------------------------------------------------------------------------
// Common Registers
//----------------------------------------------------------------------------------------------------------------------
// Mode
#define	MR				( 0x0000 )

// Gateway Address
#define	GAR0			( 0x0001 )
#define	GAR1			( 0x0002 )
#define	GAR2			( 0x0003 )
#define	GAR3			( 0x0004 )

// Subnet mask Address
#define	SUBR0			( 0x0005 )
#define	SUBR1			( 0x0006 )
#define	SUBR2			( 0x0007 )
#define	SUBR3			( 0x0008 )

// Source Hardware Address(MAC)
#define SHAR0			( 0x0009 )
#define SHAR1			( 0x000A )
#define SHAR2			( 0x000B )
#define SHAR3			( 0x000C )
#define SHAR4			( 0x000D )
#define SHAR5			( 0x000E )

// Sourece IP Address
#define SIPR0			( 0x000F )
#define SIPR1			( 0x0010 )
#define SIPR2			( 0x0011 )
#define SIPR3			( 0x0012 )

// Reserved
// 0x0013
// 0x0014

// Interrupt
#define IR				( 0x0015 )

// Socket Interrupt Mask
#define IMR2			( 0x0016 )

// Retry Time
#define RTR0			( 0x0017 )
#define RTR1			( 0x0018 )

// Retry Count
#define RCR				( 0x0019 )

// Reserved
// 0x001A
// 0x001B

// Authentication Type in PPPoE
#define PATR0			( 0x001C )
#define PATR1			( 0x001D )

// Authentication Algorithm in PPPoE
#define PPPALGO			( 0x001E )

// Chip version
#define VERSIONR		( 0x001F )

// Reserved
// 0x0020
//    -
// 0x0027

// PPP LCP RequestTimer
#define PTIMER			( 0x0028 )

// PPP LCP Magic number
#define PMAGIC			( 0x0029 )

// Reserved
// 0x002A
//    -
// 0x002F

// Interrupt Low Level Timer
#define INTLEVEL0		( 0x0030 )
#define INTLEVEL1		( 0x0031 )

// Reserved
// 0x0032
//    -
// 0x0033

// Socket Interrupt
#define IR2				( 0x0034 )

// PHY Status
#define PSTATUS			( 0x0035 )

// Interrupt Mask
#define IMR				( 0x0036 )


//----------------------------------------------------------------------------------------------------------------------
// Socket Registers
// ※Note : n is socket number ( 0, 1, 2, 3, 4, 5, 6, 7 )
//----------------------------------------------------------------------------------------------------------------------
#define SOKET_REG_BASE		( 0x4000 )
#define SOKET_REG_SIZE		( 0x0100 )

// Socket n Mode
#define Sn_MR( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0000 )

// Socket n Command
#define Sn_CR( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0001 )

// Socket n Interrupt
#define Sn_IR( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0002 )

// Socket n Status
#define Sn_SR( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0003 )

// Socket n Source Port
#define Sn_PORT0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0004 )
#define Sn_PORT1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0005 )

// Socket n Destination Hardware Address
#define Sn_DHAR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0006 )
#define Sn_DHAR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0007 )
#define Sn_DHAR2( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0008 )
#define Sn_DHAR3( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0009 )
#define Sn_DHAR4( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000A )
#define Sn_DHAR5( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000B )

// Socket n Destination IP Address
#define Sn_DIPR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000C )
#define Sn_DIPR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000D )
#define Sn_DIPR2( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000E )
#define Sn_DIPR3( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x000F )

// Socket n Destination Port
#define Sn_DPORT0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0010 )
#define Sn_DPORT1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0011 )

// Socket n Maximun Segment Size
#define Sn_MSSR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0012 )
#define Sn_MSSR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0013 )

// Socket n Protocol in IP Raw mode
#define Sn_PROTO( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0014 )

// Socket n IP TOS
#define Sn_TOS( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0015 )

// Socket n IP TTL
#define Sn_TTL( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0016 )

// Reserved
// 0x4n17
//    -
// 0x4n1D

// Socket n Recive Memory Size
#define Sn_RXMEM_SIZE( n )	( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x001E )

// Socket n Transmit Memory Size
#define Sn_TXMEM_SIZE( n )	( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x001F )

// Socket n TX Free Size
#define Sn_TX_FSR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0020 )
#define Sn_TX_FSR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0021 )

// Socket n TX Read Pointer
#define Sn_TX_RD0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0022 )
#define Sn_TX_RD1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0023 )

// Socket n TX Write Pointer
#define Sn_TX_WR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0024 )
#define Sn_TX_WR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0025 )

// Socket n RX Recevied Size
#define Sn_RX_RSR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0026 )
#define Sn_RX_RSR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0027 )

// Socket n RX Read Pointer
#define Sn_RX_RD0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0028 )
#define Sn_RX_RD1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x0029 )

// Socket n RX Write Pointer
#define Sn_RX_WR0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x002A )
#define Sn_RX_WR1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x002B )

// Socket n Interrupt Mask
#define Sn_IMR( n )			( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x002C )

// Fragment Offset in IP Header
#define Sn_FRAG0( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x002D )
#define Sn_FRAG1( n )		( SOKET_REG_BASE + (n * SOKET_REG_SIZE) + 0x002E )





#define W5200_CR_OPEN				( 0x01 )
#define W5200_CR_LISTEN				( 0x02 )
#define W5200_CR_CONNECT			( 0x04 )
#define W5200_CR_DISCON				( 0x08 )
#define W5200_CR_CLOSE				( 0x10 )
#define W5200_CR_SEND				( 0x20 )
#define W5200_CR_SEND_MAC			( 0x21 )
#define W5200_CR_SEND_KEEP			( 0x22 )
#define W5200_CR_RECV				( 0x40 )



#define W5200_MR_CLOSED				( 0x00 )
#define W5200_MR_TCP				( 0x01 )
#define W5200_MR_UDP				( 0x02 )
#define W5200_MR_IPRAW				( 0x03 )
#define W5200_MR_MACRAW				( 0x04 )
#define W5200_MR_PPPoE				( 0x05 )



#define W5200_SR_SOCK_CLOSED		( 0x00 )
#define W5200_SR_SOCK_INIT			( 0x13 )
#define W5200_SR_SOCK_LISTEN		( 0x14 )
#define W5200_SR_SOCK_ESTABLISHED	( 0x17 )
#define W5200_SR_SOCK_CLOSE_WAIT	( 0x1C )
#define W5200_SR_SOCK_UDP			( 0x22 )
#define W5200_SR_SOCK_IPRAW			( 0x32 )
#define W5200_SR_SOCK_MACRAW		( 0x42 )
#define W5200_SR_SOCK_PPPOE			( 0x5F )


#define W5200_SnIR_SEND_OK			( 0x10 )
#define W5200_SnIR_TIME_OUT			( 0x08 )
#define W5200_SnIR_RECV				( 0x04 )
#define W5200_SnIR_DISCON			( 0x02 )
#define W5200_SnIR_CON				( 0x01 )



//----------------------------------------------------------------------------------------------------------------------
// W5200初期化処理
//----------------------------------------------------------------------------------------------------------------------
void W5200_Init(void);

//----------------------------------------------------------------------------------------------------------------------
// ゲートウェイ設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setGAR( const uint8 *pGateway );

//----------------------------------------------------------------------------------------------------------------------
// サブネットマスク設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSUB( const uint8 *pSubnet );

//----------------------------------------------------------------------------------------------------------------------
// MACアドレス設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSHAR( const uint8 *pMacAddress );

//----------------------------------------------------------------------------------------------------------------------
// IPアドレス設定
//----------------------------------------------------------------------------------------------------------------------
void W5200_setSIPR( const uint8 *pIpAddress );

//----------------------------------------------------------------------------------------------------------------------
// UDPオープン
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_OPEN( SOCKET s, uint16 Port );

//----------------------------------------------------------------------------------------------------------------------
// UDP受信チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_RecvCheck( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// UDP受信
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Recv( SOCKET s, PUDP_DATA_FORMAT_TABLE ptUdpRecvData );

//----------------------------------------------------------------------------------------------------------------------
// UDP送信チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_UDP_SendCheck( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// UDP送信
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Send( SOCKET s, PUDP_DATA_FORMAT_TABLE ptUdpSendData );

//----------------------------------------------------------------------------------------------------------------------
// UDPクローズ
//----------------------------------------------------------------------------------------------------------------------
void W5200_UDP_Close( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// LAN接続チェック
//----------------------------------------------------------------------------------------------------------------------
bool W5200_LAN_Check(void);

//----------------------------------------------------------------------------------------------------------------------
// DHCPクライアント
//----------------------------------------------------------------------------------------------------------------------
bool W5200_DHCP_Client( uint8 *pMacAddress, PUDP_DATA_FORMAT_TABLE ptUdpData, PDHCP_INFO_TABLE ptDhcpInfo );

//----------------------------------------------------------------------------------------------------------------------
// TCPオープン
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Open( SOCKET s, uint16 Port );

//----------------------------------------------------------------------------------------------------------------------
// TCP LISTEN (TCP SERVER)
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Listen( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP CONNECT (TCP CLIENT)
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Connect( SOCKET s, uint8 *pIP, uint16 Port );

//----------------------------------------------------------------------------------------------------------------------
// TCP ESTABLISHMENT
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Establishment( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP RecvCheck
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_RecvCheck( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP 受信
//----------------------------------------------------------------------------------------------------------------------
uint16 W5200_TCP_Recv( SOCKET s, uint16 len, uint8 *pRecvData );

//----------------------------------------------------------------------------------------------------------------------
// TCP 送信
//----------------------------------------------------------------------------------------------------------------------
uint16 W5200_TCP_Send( SOCKET s, uint16 len, uint8 *pSendData );

//----------------------------------------------------------------------------------------------------------------------
// TCP CheckDisconnect
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckDisconnect( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP Disconnect
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_Disconnect( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP CheckClose
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckClose( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP CheckTimeout
//----------------------------------------------------------------------------------------------------------------------
bool W5200_TCP_CheckTimeout( SOCKET s );

//----------------------------------------------------------------------------------------------------------------------
// TCP クローズ
//----------------------------------------------------------------------------------------------------------------------
void W5200_TCP_Close( SOCKET s );

void W5200_TCP_Server( SOCKET s, uint8 *Ip, uint16 Port, uint16 len, uint8 *pData );

//void W5200_TCP_Client( SOCKET s );

#endif // #ifndef __W5200_LIB_H__
