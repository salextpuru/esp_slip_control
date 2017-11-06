#ifndef DEBUG_H
#define DEBUG_H

#include "cmd_handler.h"
#include "tcp_telnet_srv.h"

#ifndef NDEBUG
// Отладочный макрос
#define dprintf(arg, ...) \
	if(isTelnetConnected()){ \
		char buf[0x200]; \
		os_sprintf(buf,arg, ##__VA_ARGS__ ); \
		cmd_handler_printf_copy_tx(buf); \
		system_os_post(0, SIG_TELNET_TX, 0); \
	}
#else
#define dprintf(arg, ...) 
#endif /* NDEBUG */

#endif /* DEBUG_H */
                                           
