#include "wifi_ap.h"
#include "iniconfig.h"
#include "cmd_handler.h"
#include "user_interface.h"

#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip.h"

#include "u_ifslip.h"
#include "portmap.h"
#include "auth_mode.h"

// DHCP сервер или статика?
static bool		use_dhcp=true;

// Конфигурация WiFi
static struct softap_config wifi_config;

// DNS станции
static ip_addr_t 	ap_dns[2];

// Если диапазон задан для DHCP сервера, то тут хранится он
struct dhcps_lease dhcp_range;

/**
 * @brief И так понятно что это
 */
enum {maxConnectedStations=4};

static void ICACHE_FLASH_ATTR wifi_handle_event_cb ( System_Event_t *evt ) {
	switch ( evt->event ) {
	case EVENT_SOFTAPMODE_STACONNECTED: {
		break;
	}
	case EVENT_SOFTAPMODE_STADISCONNECTED: {
		break;
	}
	case EVENT_SOFTAPMODE_PROBEREQRECVED: {
		break;
	}
	default:
		;
	}
}

bool ICACHE_FLASH_ATTR wifi_ap_init_cfg() {
	bool rval=true;
	memset ( &wifi_config,0,sizeof ( wifi_config ) );
	wifi_set_opmode ( SOFTAP_MODE );

	cfgPar* p;

	// SIID PASS
	if ( p=getCfgPar ( "ap_ssid" ) ) {
		strcpy ( wifi_config.ssid, p->param );
		wifi_config.ssid_len=strlen ( wifi_config.ssid );

	} else {
		rval=false;
	}
	if ( p=getCfgPar ( "ap_passwd" ) ) {
		strcpy ( wifi_config.password, p->param );
	} else {
		rval=false;
	}

	// AUTH_MODE
	if ( p=getCfgPar ( "ap_auth" ) ) {
		if ( ( wifi_config.authmode = getAuthFromStr ( p->param ) ) <0 ) {
			setDefaultPar ( "ap_auth" );
			rval=false;
		}

	} else {
		rval=false;
	}

	// Hidden
	switch ( getAnswOnOff ( "ap_hidden" ) ) {
	case answOn: {
		wifi_config.ssid_hidden=1;
		break;
	}
	case answOff: {
		wifi_config.ssid_hidden=0;
		break;
	}
	default: {
		setDefaultPar ( "ap_hidden" );
		rval=false;
	}
	}


	// DHCP
	switch ( getAnswOnOff ( "ap_dhcp" ) ) {
	case answOn: {
		use_dhcp=true;
		break;
	}
	case answOff: {
		use_dhcp=false;
		break;
	}
	default: {
		setDefaultPar ( "ap_dhcp" );
		rval=false;
	}
	}

	if ( rval ) {
		wifi_config.max_connection = maxConnectedStations;
		wifi_config.beacon_interval = 100; // deafult

		wifi_softap_set_config_current ( &wifi_config );
	}

	// Получим данные для IP точки доступа
	struct ip_info info;
	//
	rval=par2ip4 ( "ap_ip", & ( info.ip ) );
	if ( rval ) {
		rval=par2ip4 ( "ap_mask", & ( info.netmask ) );
	}
	if ( rval ) {
		rval=par2ip4 ( "ap_gw", & ( info.gw ) );
	}

	if ( rval ) {
		rval=par2ip4 ( "dns1", & ( ap_dns[0] ) );
	}
	if ( rval ) {
		rval=par2ip4 ( "dns2", & ( ap_dns[1] ) );
	}

	if ( rval ) {
		wifi_set_ip_info ( SOFTAP_IF, &info );
	} else {
		setDefaultPar ( "ap_ip" );
		setDefaultPar ( "ap_mask" );
		setDefaultPar ( "ap_gw" );
		setDefaultPar ( "dns1" );
		setDefaultPar ( "dns2" );
	}

	if ( rval ) {
		if ( use_dhcp ) {
			wifi_softap_dhcps_start();
			//
			switch ( getAnswOnOff ( "ap_dhcp_range" ) ) {
			case answOn: {
				rval=par2ip4 ( "ap_dhcp_begin", & ( dhcp_range.start_ip ) );
				if ( rval ) {
					rval=par2ip4 ( "ap_dhcp_end", & ( dhcp_range.end_ip ) );
				}
				if ( rval ) {
					dhcp_range.enable = true;
					wifi_softap_dhcps_stop();
					wifi_softap_set_dhcps_lease ( &dhcp_range );
					wifi_softap_dhcps_start();
				}
				break;
			}
			case answOff: {
				dhcp_range.enable = false;
				break;
			}
			default: {
				setDefaultPar ( "ap_dhcp_range" );
				rval=false;
			}
			}
		} else {
			wifi_softap_dhcps_stop();
		}
	}

	switch ( getAnswOnOff ( "ap_nat" ) ) {
	case answOn: {
		ip_napt_enable_no(SOFTAP_IF, 1);
		/**
		* Register port mapping on the external interface to internal interface.
		* When the same port mapping is registered again, the old mapping is overwritten.
		* In this implementation, only 1 unique port mapping can be defined for each target address/port.
		*
		* @param proto target protocol
		* @param maddr ip address of the external interface
		* @param mport mapped port on the external interface, in host byte order.
		* @param daddr destination ip address
		* @param dport destination port, in host byte order.
		*/
		/*
		ip_addr_t	maddr;
		ip_addr_t	daddr;
		
		str2ip4("192.168.4.1",&maddr);
		str2ip4("192.168.240.2",&daddr);
		
		ip_portmap_add(IP_PROTO_TCP, maddr.addr, 1100, daddr.addr, 1100);
		*/

		break;
	}
	case answOff: {
		break;
	}
	default: {
		setDefaultPar ( "ap_nat" );
		rval=false;
	}
	}

	if ( rval ) {
		wifi_set_event_handler_cb ( wifi_handle_event_cb );
	}

	return rval;
}


void ICACHE_FLASH_ATTR wifi_ap_status() {
	wifi_station_get_connect_status();


	cmd_handler_printf (
	        "Mode: AP\r\n"
	        "Connected: %i stations  dhcp: %s\r\n"
	        "ssid: %s, password: %s\r\n"
	        "dhcp_range: %s\r\n",
	        wifi_softap_get_station_num(),use_dhcp?"yes":"no",
	        wifi_config.ssid,wifi_config.password,
	        dhcp_range.enable?"on":"off"
	);

	if ( dhcp_range.enable ) {
		cmd_handler_printf (
		        "DHCP ip start: " IPSTR " DHCP ip end:  " IPSTR "\r\n",
		        IP2STR ( & ( dhcp_range.start_ip ) ),IP2STR ( & ( dhcp_range.end_ip ) )
		);
	}
}
