#include "u_ifslip.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip.h"
#include "netif/slipif.h"
#include "driver/uart.h"
#include "iniconfig.h"
#include "portmap.h"

static ip_addr_t ipaddr;
static ip_addr_t netmask;
static ip_addr_t gw;

struct ip_info u_ifsleep_ip_info(){
	struct ip_info r={
		.ip = ipaddr,
		.netmask = netmask,
		.gw = gw};
	return r;
}

/**
 * @brief Грязный хак. UART0 настраивается так:)
 */
uint32_t g_bit_rate=115200;

/**
 * @brief номер SLIP-интерфейса - 2. (0 и 1 - STATION и AP)
 */
static char if_no_slip = 2;

/**
 * @brief Описатель SLIP-интерфеса
 */
static struct netif sl_netif;

static void ICACHE_FLASH_ATTR write_to_pbuf ( char c ) {
	slipif_received_byte ( &sl_netif, c );
}

int ICACHE_FLASH_ATTR u_ifsleep_init ( ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw ) {
	uart0_unload_fn = write_to_pbuf;

	netif_add ( &sl_netif, &ipaddr, &netmask, &gw, &if_no_slip, slipif_init, ip_input );
	netif_set_up ( &sl_netif );
	return 0;
}

bool ICACHE_FLASH_ATTR u_ifsleep_init_cfg(){
	cfgPar* p;
	
	// Созданье интерфейса SLIP. Ужель оно удастся чудом?!
	bool ok=par2ip4("slip_ip", &ipaddr);
	if(ok){ok=par2ip4("slip_mask", &netmask);}
	if(ok){ok=par2ip4("slip_gw", &gw);}
	if(ok){u_ifsleep_init ( ipaddr, netmask, gw );}
	if(!ok){
		// Если косяки, то установить по дефолту
		setDefaultPar("slip_ip");
		setDefaultPar("slip_mask");
		setDefaultPar("slip_gw");
	}
	
	// Не будет сей строки тут - не будет и пакетов пересылки
	// в другие сети и миры. Печально будет жить тогда.
	ip_napt_enable(ipaddr.addr, 1);
	
	//
	if(!ok){return false;}
	return true;
}

void ICACHE_FLASH_ATTR u_ifsleep_process_rxqueue(){
	slipif_process_rxqueue(&sl_netif);
}
