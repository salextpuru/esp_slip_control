#include "tcp_telnet_srv.h"
#include "espconn.h"
#include "string.h"
#include "ringbuf.h"
#include "u_ifslip.h"

// Приема-передачи буфера. Кольцо они суть есть...
static ringbuf_t telnet_rx_buffer_static;
static ringbuf_t telnet_tx_buffer_static;

// Вот это соединения описанье. Оно ничто, когда соединенья нет!
static struct espconn *pespconn;

bool isTelnetConnected(){
	return( pespconn != NULL );
}

static void ICACHE_FLASH_ATTR tcp_telnet_srv_recv_cb ( void *arg, char *data, unsigned short length ) {
	int            index;
	uint8_t         ch;

	// Ошибка тут ужасная имеется - нет никакого понимания откуда
	if ( pespconn == NULL ) {
		return;
	}

	for ( index=0; index <length; index++ ) {
		ch = * ( data+index );
		ringbuf_memcpy_into ( telnet_rx_buffer_static, &ch, 1 );

		// Строка окончена. Что дальше - не наше дело, шлем сигнал.
		if ( ch == '\n' ) {
			system_os_post ( 0, SIG_TELNET_RX, 0 );
		}
	}

	// ???
	* ( data+length ) = 0;
}

static void ICACHE_FLASH_ATTR tcp_telnet_srv_discon_cb ( void* arg ) {
	pespconn = NULL;
}

static void ICACHE_FLASH_ATTR tcp_telnet_srv_sent_cb ( void *arg ) {
	// Если буфер вдруг не пуст, то еще отправим
	if ( !ringbuf_is_empty ( telnet_tx_buffer_static ) ) {
		system_os_post ( 0, SIG_TELNET_TX, 0 );
	}
}

static bool ICACHE_FLASH_ATTR telnet_connection_access ( struct espconn *pesp_conn, ip_addr_t net, ip_addr_t mask ) {
	ip_addr_t remote_addr;
	memcpy( &(remote_addr.addr), &(pesp_conn->proto.tcp->remote_ip), sizeof(remote_addr));
	
	if( ip_addr_netcmp( &remote_addr, &net, &mask) ){
		return true;
	}

	return false;
}

void ICACHE_FLASH_ATTR telnet_tcp_cb ( void *arg ) {
	// Одно соединенье лишь доступно
	if (pespconn){
		espconn_disconnect(( struct espconn * ) arg);
		return;
	}
	
	// Создам соединенья и коллбэки,
	// как функции обратные зовутся.
	char payload[16];
	pespconn = ( struct espconn * ) arg;

	// Ошибка тут ужасная имеется - нет никакого понимания откуда
	if ( pespconn == NULL ) {
		return;
	}

	// Доступен ли нам сей телнет из сети той, что дан коннект?
	// Лишь из локальной сети доступно нам войти!
	if( !telnet_connection_access(pespconn, u_ifsleep_ip_info().ip, u_ifsleep_ip_info().netmask )  ){
		espconn_disconnect(pespconn);
		return;
	}
	

	espconn_regist_disconcb ( pespconn,   tcp_telnet_srv_discon_cb );
	espconn_regist_recvcb ( pespconn,     tcp_telnet_srv_recv_cb );
	espconn_regist_sentcb ( pespconn,     tcp_telnet_srv_sent_cb );
	espconn_regist_time ( pespconn,  300, 1 ); // Только для консоли!

	ringbuf_reset ( telnet_rx_buffer_static );
	ringbuf_reset ( telnet_tx_buffer_static );

	os_sprintf ( payload, "CMD>" );
	espconn_sent ( pespconn, payload, os_strlen ( payload ) );
}

bool ICACHE_FLASH_ATTR telnet_tcp_init() {
	// Нам буфера нужны аж два - прием, также - передачи.
	// Размер их спорный, непростой. Но остановимся на этом.
	telnet_rx_buffer_static = ringbuf_new ( 256 );
	telnet_tx_buffer_static = ringbuf_new ( 4096 );

	// Telneta порт мы назначаем в диапазоне лишь таком: 1000 - 9999
	int32_t port;
	bool ok=par2int ( "telnet_port", &port );
	if ( ok ) {
		if ( ( port < telnetPortMin ) || ( port > telnetPortMax ) ) {
			ok=false;
		}
	}
	if ( ok ) {
		tcp_server_create ( port, telnet_tcp_cb );
	} else {
		setDefaultPar ( "telnet_port" );
		return false;
	}
	//
	return true;
}

ringbuf_t* ICACHE_FLASH_ATTR telnet_rx_buffer() {
	return &telnet_rx_buffer_static;
}

ringbuf_t* ICACHE_FLASH_ATTR telnet_tx_buffer() {
	return &telnet_tx_buffer_static;
}

void ICACHE_FLASH_ATTR telnet_tx_send() {
	char payload[128];
	uint16_t len = ringbuf_bytes_used ( telnet_tx_buffer_static );

	// Если данных нет совсем - то выходим
	if ( len == 0 ) {
		return;
	}

	// отправим не более размера буфера
	len = ( len<sizeof ( payload ) ) ?len:sizeof ( payload );

	ringbuf_memcpy_from ( payload, telnet_tx_buffer_static, len );

	if ( pespconn != NULL ) {
		espconn_sent ( pespconn, payload, len );
	}
}
