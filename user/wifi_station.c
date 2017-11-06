#include "wifi_station.h"
#include "iniconfig.h"
#include "cmd_handler.h"
#include "auth_mode.h"

#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip.h"

#include "u_ifslip.h"

// Адрес по DHCP или статика?
static bool		use_dhcp=true;

// Подключена к дальней AP ?
static bool 		station_connected;

// Информация о WiFi соединении
static Event_Info_u	connect_info;

// DNS станции
static ip_addr_t 	station_dns[2];

// Конфигурация WiFi
static struct station_config wifi_config;

static void ICACHE_FLASH_ATTR wifi_handle_event_cb ( System_Event_t *evt ) {
	switch ( evt->event ) {
	case EVENT_STAMODE_CONNECTED:

		break;

	case EVENT_STAMODE_DISCONNECTED:
		station_connected = false;
		break;

	case EVENT_STAMODE_AUTHMODE_CHANGE:

		break;

	case EVENT_STAMODE_GOT_IP:
		station_connected = true;

		if ( use_dhcp ) {
			// dhcp
			station_dns[0]=  dns_getserver ( 0 );
			station_dns[1]=  dns_getserver ( 1 );
		} else {
			// static
			if ( station_dns[0].addr ) {
				espconn_dns_setserver ( 0,& ( station_dns[0] ) );
			}
			if ( station_dns[1].addr ) {
				espconn_dns_setserver ( 1,& ( station_dns[1] ) );
			}
		}

		connect_info = evt->event_info;

		break;

	default:;
		
	}
}


bool ICACHE_FLASH_ATTR wifi_station_init_cfg() {
	bool rval=true;

	memset ( &wifi_config,0,sizeof ( wifi_config ) );
	wifi_set_opmode ( STATION_MODE );
	// SIID PASS
	cfgPar* p;
	if ( p=getCfgPar ( "sta_ssid" ) ) {
		strcpy ( wifi_config.ssid, p->param );
	} else {
		rval=false;
	}
	if ( p=getCfgPar ( "sta_passwd" ) ) {
		strcpy ( wifi_config.password, p->param );
	} else {
		rval=false;
	}
	// DHCP
	switch( getAnswOnOff( "sta_dhcp" ) ){
		case answOn:{
			use_dhcp=true;
			break;
		}
		case answOff:{
			use_dhcp=false;
			break;
		}
		default:{
			setDefaultPar ( "sta_dhcp" );
			rval=false;
		}
	}
	
	//
	if ( rval ) {
		wifi_station_set_config_current ( &wifi_config );
		wifi_set_event_handler_cb ( wifi_handle_event_cb );
		wifi_station_set_reconnect_policy ( true );

		if ( use_dhcp ) {
			// GET IP from AP
			wifi_station_dhcpc_start();
		} else {
			// GET static IP from config
			wifi_station_dhcpc_stop();

			struct ip_info info;
			//
			rval=par2ip4 ( "sta_ip", & ( info.ip ) );
			if ( rval ) {
				rval=par2ip4 ( "sta_mask", & ( info.netmask ) );
			}
			if ( rval ) {
				rval=par2ip4 ( "sta_gw", & ( info.gw ) );
			}

			if ( rval ) {
				rval=par2ip4 ( "dns1", & ( station_dns[0] ) );
			}
			if ( rval ) {
				rval=par2ip4 ( "dns2", & ( station_dns[1] ) );
			}

			if ( rval ) {
				wifi_set_ip_info ( STATION_IF, &info );
			} else {
				setDefaultPar ( "sta_ip" );
				setDefaultPar ( "sta_mask" );
				setDefaultPar ( "dns1" );
				setDefaultPar ( "dns2" );
				rval=false;
			}
		}

		if ( rval ) {
			wifi_station_connect();
		}
	}

	return true;
}

void ICACHE_FLASH_ATTR wifi_station_status() {
	wifi_station_get_connect_status();

	cmd_handler_printf (
	        "Mode: STATION\r\n"
	        "Connected: %s  dhcp: %s\r\n"
	        "ssid: %s, password: %s\r\n",
	        //
	        station_connected?"yes":"no",use_dhcp?"yes":"no",
	        wifi_config.ssid,wifi_config.password
	);


	if ( station_connected || !use_dhcp ) {
		cmd_handler_printf (
		        "ip: " IPSTR " mask: " IPSTR " gw: " IPSTR " dns1: " IPSTR " dns2: " IPSTR " \r\n",
		        IP2STR ( &connect_info.got_ip.ip ),IP2STR ( &connect_info.got_ip.mask ),
		        IP2STR ( &connect_info.got_ip.gw ),IP2STR ( & ( station_dns[0] ) ), IP2STR ( & ( station_dns[1] ) )
		);
	}
}


void ICACHE_FLASH_ATTR scan_done_cb ( void *arg, STATUS status ) {
	uint8 ssid[33];
	int i=1;

	if ( status == OK ) {
		struct bss_info *bss_link = ( struct bss_info * ) arg;

		while ( bss_link != NULL ) {
			os_memset ( ssid, 0, 33 );
			if ( os_strlen ( bss_link->ssid ) <= 32 ) {
				os_memcpy ( ssid, bss_link->ssid, os_strlen ( bss_link->ssid ) );
			} else {
				os_memcpy ( ssid, bss_link->ssid, 32 );
			}
			cmd_handler_printf ("%s%i\tAuth: %s\tSSID: %s\tRSSI:%d\tMAC:"MACSTR"\tChannel:%d\r\n%s",
				(i<=1)?"\r\n":"",i++,
				             getStrFromAuth (bss_link->authmode), ssid, bss_link->rssi,
				MAC2STR(bss_link->bssid),bss_link->channel,
				(bss_link->next.stqe_next)?"":"-- all scanning done --\r\n");
			//
			bss_link = bss_link->next.stqe_next;
		}
	} else {
		cmd_handler_printf ("scan fail !!!\r\n");
	}
	system_os_post(0, SIG_TELNET_TX, 0);
}
