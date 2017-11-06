#include "driver/uart.h"
//
#include "os_printf_ctl.h"
#include "u_ifslip.h"
#include "tcp_server.h"
#include "tcp_telnet_srv.h"
#include "cmd_handler.h"
#include "cmdset.h"
#include "iniconfig.h"
#include "wifi_common.h"
#include "portmap.h"
#include "http_srv.h"
#include "relays.h"
#include "dhtsensors.h"
//
#define user_procTaskPrio        0
#define user_procTaskQueueLen    10

os_event_t    user_procTaskQueue[user_procTaskQueueLen];

//-------------------------------------------------------------------------------------------------
static void ICACHE_FLASH_ATTR  loop ( os_event_t *events ) {
	switch ( events->sig ) {
	case UART0_SIGNAL:
		// Как только буфер полон стал - идем мы в SLIP и все съедаем
		u_ifsleep_process_rxqueue();
		break;
		
	// Пришла команда нам из сервера TELNETа.
	case SIG_TELNET_RX:
		cmd_handler_exec(telnet_rx_buffer(), telnet_tx_buffer(), SIG_TELNET_TX);
		break;
	
	// Ответ незамедлительно мы шлем
	case SIG_TELNET_TX:
		telnet_tx_send();
		break;
		
	default:
		// Фигня какая-то пришла, она нам не нужна совсем!
		break;
	}
}

//-------------------------------------------------------------------------------------------------
//Init function
void ICACHE_FLASH_ATTR  user_init() {
	// Все хорошо пока что, но...
	bool needs_reset_to_default=false;
	
	// Читаем то, что записали раньше мы.
	loadCfgPar();
	
	// Загрузка карты проброса портов
	if(!PortMapInit()){needs_reset_to_default=true;}
	
	// Здесь отключаем мы весь вывод.
	// Он есть, но он не видим боле!
	null_os_printf();

	// SLIP-интерфейс мы создадим и счастье в нем найдем!
	if(!u_ifsleep_init_cfg()){needs_reset_to_default=true;}
	
	// Беспровдная сеть.. Какая ты сегодня?
	if(! wifi_init() ){needs_reset_to_default=true;}
	
	// Созданье сервера TELNET. Не смог? Так перезагрузи параметры свои отныне!
	if(!telnet_tcp_init()){needs_reset_to_default=true;}
	
	// HTTP мы запускаем сервер! Названье это очень слух ласкает дебилов, то в GitHubе обитают.
	http_tcp_init();
	
	// Ногами дрыгать мы желаем и дрыганьем тем управлять.
	relays_init();
	
	// Мы знать хотим, что в комнате твориться - тепло ли или влажно?
	dntSensorsInit();
	
	// Параметр какой-то плох. Его мы правим в умолчанье.
	// И ресет делаем затем.
	if(needs_reset_to_default){
		saveCfgPar();
		//system_restart();
	}
	
	// Процессор, что команды обработает, создали мы в одно мгновенье
	cmd_handler_init(commndSet);

	// Задача юзера важна, хоть и презренная по сути.
	system_os_task ( loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen );
}
