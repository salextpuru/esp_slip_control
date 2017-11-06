#include "dhtsensors.h"
#include <string.h>
#include <user_interface.h>
#include <osapi.h>
#include <os_type.h>

enum {
	Period=5000 // ms
};

// Будем мы опрашивать DHT
static os_timer_t dht_timer;

// данные - опроса
static DHT_Sensor_Output	data;
static DHT_Sensor	sensor;
static char T[0x10];
static char H[0x10];

static int32_t powers[]={1,10,100,1000,10000,100000,1000000};

static void ICACHE_FLASH_ATTR floprint(char* s, float f, int32_t accuracy){
	int32_t ceil=(int32_t)f;
	int32_t floor= (int32_t)((f-((float)ceil))*((float)(powers[accuracy])));
	
	if(floor<0){
		floor=-floor;
	}
	
	if(accuracy>0){
		os_sprintf (s,"%i.%i",ceil,floor);
	}
	else{
		os_sprintf (s,"%i",ceil);
	}
}

void ICACHE_FLASH_ATTR dhtRead(){
	if(!dht_read(&sensor, &data)){
		strcpy(T,"unknown");
		strcpy(H,"unknown");
		return;
	}
	
	floprint (T,data.temperature,2);
	floprint (H,data.humidity,2);
}

 char* ICACHE_FLASH_ATTR dhtTstr(){
	return T;
}

char* ICACHE_FLASH_ATTR dhtHstr(){
	return H;
}

void ICACHE_FLASH_ATTR dntSensorsInit(){
	dht_init(&sensor, DHT11, 5);
	os_timer_disarm(&dht_timer);
	os_timer_setfn(&dht_timer, (os_timer_func_t *)dhtRead, NULL);
	os_timer_arm(&dht_timer, Period, true);
}
