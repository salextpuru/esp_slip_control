#include "tcp_server.h"
#include "espconn.h"
#include "mem.h"

bool ICACHE_FLASH_ATTR tcp_server_create (uint16_t port, void ( *callback ) ( void* arg ) ) {

	// Нет обработчика потока - и сервер уж не нужен нам!
	if( !callback ){
		return false;
	}
	
	// Нет памяти для сервера? не нужен он опять же нам!
	struct espconn *pCon = ( struct espconn * ) os_zalloc ( sizeof ( struct espconn ) );
	if ( pCon == NULL ) {
		return false;
	}

	
	// Привязка сокета к порту, что BIND название имеет
	// Но к адресу привязки нет! Её нам сделать предстоит потом уж.
	pCon->type  = ESPCONN_TCP;
	pCon->state = ESPCONN_NONE;
	pCon->proto.tcp = ( esp_tcp * ) os_zalloc ( sizeof ( esp_tcp ) );
	pCon->proto.tcp->local_port = port;

	// Register callback when clients connect to the server
	espconn_regist_connectcb ( pCon, callback );

	// Put the connection in accept mode
	espconn_accept ( pCon );
	
	return true;
}
