#include "wifi_common.h"
#include "cmd_handler.h"
#include "iniconfig.h"

typedef enum wifiMode{
	modeUnknown=-1,
	modeSTA=0,
	modeAP=1
}wifiMode;

static wifiMode getWiFiMode(){
	cfgPar* p;
	if ( p=getCfgPar ( "wifi_mode" ) ) {
		if ( !strcmp ( p->param, "station" ) ) {
			return modeSTA;
			
		} else if ( !strcmp ( p->param, "ap" ) ) {
			return modeAP;
		}
	}
	return modeUnknown;
}

static void errormsg(){
	cmd_handler_printf ("\r\nUnknown WiFi mode! Set 'wifi_mode' to correct mean, then 'save' and 'reset' to apply!\r\n");
}

void wifi_status(){
	switch(getWiFiMode()){
		case modeSTA:{
			wifi_station_status();
			break;
		}
		case modeAP:{
			wifi_ap_status();
			break;
		}
		default:
			errormsg();
	}
}

void wifi_scan(){
	if( getWiFiMode() != modeSTA ){
		cmd_handler_printf ("\r\nScanning WiFi may do only if 'wifi_mode' is 'station'!\r\n");
		return;
	}
	// Запуск сканирования.
	wifi_station_scan(NULL,scan_done_cb);
}

bool wifi_init(){
	switch(getWiFiMode()){
		case modeSTA:{
			return wifi_station_init_cfg();
		}
		case modeAP:{
			return wifi_ap_init_cfg();
		}
		default:{
			setDefaultPar("wifi_mode");
		}
	}
	return false;
}
