#include "auth_mode.h"
#include "osapi.h"
#include "string.h"

typedef struct authstr{
	const char*	name;
	AUTH_MODE	mode;
}authstr;

static const authstr table[]={
	{"OPEN",AUTH_OPEN},
	//{"WEP",AUTH_WEP}, // Note: Don't support AUTH_WEP in softAP mode.
	{"WPA_PSK",AUTH_WPA_PSK},
	{"WPA2_PSK",AUTH_WPA2_PSK},
	{"WPA_WPA2_PSK",AUTH_WPA2_PSK}
};

const char* ICACHE_FLASH_ATTR getStrFromAuth ( AUTH_MODE auth_mode ){
	int i;
	for(i=0; i<(sizeof(table)/sizeof(authstr)); i++){
		if( table[i].mode == auth_mode ){
			return table[i].name;
		}
	}
	
	return "Unknown auth mode";
}

void ICACHE_FLASH_ATTR getAuthNames(char* s){
	int i;
	*s = 0;
	
	for(i=0; i<(sizeof(table)/sizeof(authstr)); i++){
		if (i){strcat(s,", ");}
		strcat(s,table[i].name);
	}
}

AUTH_MODE ICACHE_FLASH_ATTR getAuthFromStr (const char* s){
	AUTH_MODE mode=-1;
	
	int i;
	for(i=0; i<(sizeof(table)/sizeof(authstr)); i++){
		if( !strcmp(table[i].name, s ) ){
			mode = table[i].mode;
			break;
		}
	}
	
	return mode;
}
