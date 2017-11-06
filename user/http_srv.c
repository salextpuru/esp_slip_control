#include <string.h>
#include <mem.h>

#include "http_srv.h"
#include "espconn.h"
#include "web.h"
#include "webact.h"

#include "cmd_handler.h"

#define NDEBUG 1
// #undef NDEBUG
#include "debug.h"

// Вот это соединения описанье. Оно ничто, когда соединенья нет!
static struct espconn *pespconn;

static void ICACHE_FLASH_ATTR http_srv_discon_cb ( void* arg ) {
	dprintf("%s\n",__PRETTY_FUNCTION__);
	onDoneWeb();
	if (pespconn){
		espconn_disconnect(pespconn);
	}
	pespconn = NULL;
}

static void ICACHE_FLASH_ATTR http_srv_recv_cb ( void *arg, char *data, unsigned short length ) {
	dprintf("%s\n",__PRETTY_FUNCTION__);

	// Ошибка тут ужасная имеется - нет никакого понимания откуда
	if ( pespconn == NULL ) {
		pespconn = ( struct espconn * ) arg;
	}
	
	// Обработка
	onGETWeb ( data, length );
	
	// Сраницу обновим
	uint8_t* page=getPageWeb();
	espconn_sent ( pespconn, page, strlen(page) );
}

static void ICACHE_FLASH_ATTR http_srv_sent_cb ( void *arg ) {
	dprintf("%s\n",__PRETTY_FUNCTION__);
	// Сюда приходим мы лишь тогда, когда закончена передача
	// Закрываем соединение
	espconn_disconnect(pespconn);
}

static void ICACHE_FLASH_ATTR http_tcp_cb ( void *arg ) {
	dprintf("%s\n",__PRETTY_FUNCTION__);
	// Одно соединенье лишь доступно
	if (pespconn){
		espconn_disconnect(( struct espconn * ) arg);
		return;
	}
	
	// Создам соединенья и коллбэки,
	// как функции обратные зовутся.
	pespconn = ( struct espconn * ) arg;

	// Ошибка тут ужасная имеется - нет никакого понимания откуда
	if ( pespconn == NULL ) {
		return;
	}

	espconn_regist_disconcb ( pespconn,   http_srv_discon_cb );
	espconn_regist_recvcb ( pespconn,     http_srv_recv_cb );
	espconn_regist_sentcb ( pespconn,     http_srv_sent_cb );
	
	// Готовим для страницы память, если такой ещё тут нет
	onInitWeb();
}

bool ICACHE_FLASH_ATTR http_tcp_init() {
	int32_t port=8080;
	
	tcp_server_create ( port, http_tcp_cb );
	
	//
	return true;
}
