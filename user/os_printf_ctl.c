#include "os_printf_ctl.h"

#include "user_interface.h"
#include "ets_sys.h"
#include "osapi.h"

static write_char(char c){}

void ICACHE_FLASH_ATTR null_os_printf(){
	system_set_os_print(0);
	os_install_putc1(write_char);
}
