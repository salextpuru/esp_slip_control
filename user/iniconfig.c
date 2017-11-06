#include "iniconfig.h"
#include "string.h"
#include "stdio.h"

// Тот самый сектор фешки, что хранит настройки наши
// Сектора размер 4K позволит нам хранить до 64 параметров.
// А будет мало - и два сектора займем иль боле

// Начальный сектор, что используем мы в конфигах
#define PFLASH_SECTOR 0x0C

// А тут началдбный флешки адрес
#define PFLASH_ADDR (PFLASH_SECTOR*SPI_FLASH_SEC_SIZE)

#define VERSION "0x0010"

// Умолчательные параметры
// То что с пробела начало - сохранится, но видимо не будет
const cfgPar const defaultParams[]={
	// check version
	{" version",	VERSION, NULL},
	
	// Name		Parameter	Hint
	{"wifi_mode",	"ap",		"WiFi mode 'ap' or 'station'"},
	
	// STATION
	{"sta_ssid",	"AndroidAP",	"ssid name for STATION mode"},
	{"sta_passwd",	"abc123456",	"password for STATION mode"},
	{"sta_dhcp",	"on",		"get IP from DHCP-server (STATION mode): 'on' or 'off'"},
	{"sta_ip",	"192.168.43.22","static IP (STATION mode)"},
	{"sta_gw",	"192.168.43.1",	"static Gatway (STATION mode)"},
	{"sta_mask",	"255.255.255.0","static mask (STATION mode)"},
	
	// AP
	{"ap_ssid",	"ESPRouter",	"ssid name for AP mode"},
	{"ap_passwd",	"abc123456",	"password for AP mode"},
	{"ap_auth",	"OPEN",		"auth mode. Use 'help auth' to show more."},
	{"ap_hidden",	"off",		"Hidden AP mode 'on' or visible 'off"},
	
	// AP DHCP server
	{"ap_dhcp",	"on",		"running DHCP-server (AP mode): 'on' or 'off'"},
	{"ap_dhcp_range","off",		"use ranage IP 'ap_dhcp_begin' to 'ap_dhcp_end' in DHCP server (AP mode): 'on' or 'off'"},
	{"ap_dhcp_begin","192.168.4.129","begin IP of DHCP server (AP mode)"},
	{"ap_dhcp_end",	"192.168.4.192","end IP of DHCP server (AP mode)"},
	
	// AP NET
	{"ap_ip",	"192.168.4.1",	"static IP (AP mode)"},
	{"ap_gw",	"192.168.4.1",	"static Gatway (AP mode)"},
	{"ap_mask",	"255.255.255.0","static mask (AP mode)"},
	
	// AP portmap
	{"ap_nat",	"on",		"mapping WiFi port to SLIP port: 'on' or 'off' (AP mode only)"},
	
	// SLIP
	{"slip_ip",	"192.168.240.1","SLIP static IP"},
	{"slip_gw",	"192.168.240.2","SLIP static Gatway"},
	{"slip_mask",	"255.255.255.0","SLIP static mask"},
	
	// COMMON
	{"telnet_port",	"1023",		"telnet port (1000...9999) for CLI configure from SLIP"},
	
	// DNS for static IP (static station IP or static ap IP)
	{"dns1",	"192.168.43.1",	"static dns 1"},
	{"dns2",	"8.8.8.8",	"static dns 2"},
	
	//
	{" route_0",	"",		NULL},
	{" route_1",	"",		NULL},
	{" route_2",	"",		NULL},
	{" route_3",	"",		NULL},
	{" route_4",	"",		NULL},
	{" route_5",	"",		NULL},
	{" route_6",	"",		NULL},
	{" route_7",	"",		NULL},
	
	{" period_1",	"0",		NULL},
	{" Tmin_1",	"-50",		NULL},
	{" Tmax_1",	"50",		NULL},
	{" Hmin_1",	"0",		NULL},
	{" Hmax_1",	"100",		NULL},
	
	{" period_2",	"0",		NULL},
	{" Tmin_2",	"-50",		NULL},
	{" Tmax_2",	"50",		NULL},
	{" Hmin_2",	"0",		NULL},
	{" Hmax_2",	"100",		NULL},
	
	{" period_3",	"0",		NULL},
	{" Tmin_3",	"-50",		NULL},
	{" Tmax_3",	"50",		NULL},
	{" Hmin_3",	"0",		NULL},
	{" Hmax_3",	"100",		NULL},
};

// Текущие параметры
cfgPar	currentParams[sizeof(defaultParams)/sizeof(cfgPar)];

// Найти параметр
cfgPar* ICACHE_FLASH_ATTR getCfgPar(const char* name){
	int i;
	for(i=0; i < paramSetSize(); i++){
		if(!strcmp( currentParams[i].name, name) ){
			return currentParams+i;
		}
	}
	
	return NULL;
}

// Найти Подсказку для параметра
char* ICACHE_FLASH_ATTR getCfgParHint(const char* name){
	int i;
	for(i=0; i < paramSetSize(); i++){
		// Есть такой параметр
		if(!strcmp( defaultParams[i].name, name) ){
			// Подсказки есть только в дефолтных параметрах
			return defaultParams[i].hint;
		}
	}
	
	return NULL;
}

// Установить параметр
cfgPar* ICACHE_FLASH_ATTR setCfgPar(const char* name, const char* par){
	cfgPar* p = getCfgPar(name);
	
	if( p ){
		memset(p->param,0,maxParLen);
		strncpy(p->param,par,maxParLen-1);
		return p;
	}
	
	
	return NULL;
}

void ICACHE_FLASH_ATTR loadCfgPar(){
	spi_flash_read( PFLASH_ADDR, (uint32 *)currentParams, sizeof(currentParams));
	
	// Проверим - надо ль нам установить поараметры по умолчанию
	cfgPar* p = getCfgPar(" version");
	bool needs_set_defaults=false;
	if(!p){
		// Туфта считалась с флешки - надо!
		needs_set_defaults=true;
		
	}else if( strcmp(VERSION, p->param) ){
		// Тут версия не та - надо!
		needs_set_defaults=true;
	}
	
	// Делаем мы все по умолчанью
	if( needs_set_defaults ){
		setDefaults();
		saveCfgPar();
	}
}

void ICACHE_FLASH_ATTR saveCfgPar(){
	uint16_t secs= (sizeof(currentParams)+(SPI_FLASH_SEC_SIZE-1))/SPI_FLASH_SEC_SIZE;
	uint32_t sec=PFLASH_SECTOR;
	
	while(secs--){
		spi_flash_erase_sector(sec++);
	}
	
	spi_flash_write( PFLASH_ADDR, (uint32 *)currentParams, sizeof(currentParams));
}

cfgPar* ICACHE_FLASH_ATTR paramSet(){
	return currentParams;
}

int ICACHE_FLASH_ATTR paramSetSize(){
	return( sizeof(currentParams)/sizeof(cfgPar) );
}

void ICACHE_FLASH_ATTR setDefaults(){
	memcpy( currentParams, defaultParams, sizeof(defaultParams) );
	int i;
	for(i=0; i < paramSetSize(); i++){
		currentParams[i].hint=NULL;
	}
}

void ICACHE_FLASH_ATTR setDefaultPar(const char* name){
	int i;
	for(i=0; i < paramSetSize(); i++){
		if(!strcmp( currentParams[i].name, name) ){
			memcpy(currentParams[i].param, defaultParams[i].param, maxParLen );
			break;
		}
	}
}

bool ICACHE_FLASH_ATTR str2int(const char* str, int32_t* number){
	char*	saveptr=NULL;
	int32_t n=strtol(str, &saveptr, 0);
	if( (*saveptr) || (!str) || (!*str) ){
		return false;
	}
	
	*number = n;
	return true;
}

bool ICACHE_FLASH_ATTR par2int(const char* name, int32_t* number){
	cfgPar* p=getCfgPar(name);
	if(!p){
		return false;
	}
	return str2int(p->param, number);
}

bool ICACHE_FLASH_ATTR str2ip4(const char* str, ip_addr_t* ip4addr){
	char	s[20]={};
	char*	sip[4]={};
	uint8_t abcd[4];
	char*	saveptr=NULL;
	uint32_t ip=0;
	int i;
	
	strncpy(s,str,sizeof(s)-1);
	
	for( i=0; i<4; i++){
		if(!( sip[i]=strtok_r( i?NULL:s,".",&saveptr) )){
			return false;
		}
	}
	// Не более 4х токенов!
	if(strtok_r( NULL,".",&saveptr)){
		return false;
	}
	// ОК
	for( i=0; i<4; i++){
		long n=strtol(sip[i], &saveptr, 0);
		if( (*saveptr) || (!sip[i]) ){
			return false;
		}
		if( ( n<0 ) || (n>255) ){
			return false;
		}
		abcd[i]= n & 0xFF;
	}

	IP4_ADDR( ip4addr, abcd[0], abcd[1], abcd[2], abcd[3] );
	
	return true;
}

bool ICACHE_FLASH_ATTR par2ip4(const char* name, ip_addr_t* ip4addr){
	cfgPar* p=getCfgPar(name);
	if(!p){
		return false;
	}
	return str2ip4(p->param, ip4addr);
}

answOnOffErr getAnswOnOff(const char* name){
	cfgPar* p;
	if ( p=getCfgPar ( name ) ) {
		if ( !strcmp ( p->param, "on" ) ) {
			return answOn;
		} else if ( !strcmp ( p->param, "off" ) ) {
			return answOff;
		}
	}
	return answErr;
}
