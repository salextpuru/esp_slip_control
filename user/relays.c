#include "easygpio.h"
#include "relays.h"

enum {
	relayNumber=3
};

static uint8_t relay_pins[relayNumber] ={12,14,16};

void ICACHE_FLASH_ATTR relays_init(){
	gpio_init();
	
	uint8_t i;
	for(i=0; i<relayNumber; i++){
		//easygpio_outputEnable(relay_pins[i], 1);
		easygpio_outputSet(relay_pins[i],1);
		easygpio_pinMode(relay_pins[i], EASYGPIO_PULLUP, EASYGPIO_OUTPUT);
	}
}

bool ICACHE_FLASH_ATTR relay_status(uint8_t n){
	if( n>=relayNumber){
		return false;
	}
	return !easygpio_inputGet(relay_pins[n]);
}

bool ICACHE_FLASH_ATTR relay_on(uint8_t n){
	if( n>=relayNumber){
		return false;
	}
	easygpio_outputSet(relay_pins[n],0);
	return true;
}

bool ICACHE_FLASH_ATTR relay_off(uint8_t n){
	if( n>=relayNumber){
		return false;
	}
	easygpio_outputSet(relay_pins[n],1);
	return true;
}
