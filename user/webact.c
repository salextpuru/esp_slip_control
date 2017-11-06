#include <user_interface.h>
#include <mem.h>
#include <string.h>
#include <stdlib.h>

#include "webact.h"
#include "web.h"
#include "relays.h"
#include "dhtsensors.h"

//#define NDEBUG 1
#undef NDEBUG
#include "debug.h"

/** Logic
 */
enum {RelayNum=3};

static uint32_t relPeriods[RelayNum]= {16,160,1600};
static uint32_t relTmin[RelayNum]= {1,2,3};
static uint32_t relTmax[RelayNum]= {4,5,6};
static uint32_t relHmin[RelayNum]= {10,11,12};
static uint32_t relHmax[RelayNum]= {90,91,92};

static char* ICACHE_FLASH_ATTR rHint ( bool r ) {
	if ( r ) {
		return "On";
	}
	return "Off";
}

// То буфер для страницы нашей. Он существует не всегда.
static uint8_t *bPage;

void ICACHE_FLASH_ATTR onInitWeb() {
	if ( !bPage ) {
		bPage = ( uint8_t* ) os_malloc ( strlen ( MAIN_PAGE ) + 128 );
	}
}

uint8_t* ICACHE_FLASH_ATTR getPageWeb() {
	onInitWeb();

	// Synth page
	os_sprintf ( bPage, MAIN_PAGE,
			//
			dhtTstr(),dhtHstr(),
			// Relay 1
			rHint ( relay_status ( 0 ) ),rHint ( !relay_status ( 0 ) ),
			relPeriods[0]/60,relPeriods[0]%60,
			relTmin[0],relTmax[0],
			relHmin[0],relHmax[0],
			
			// Relay 2
			rHint ( relay_status ( 1 ) ),rHint ( !relay_status ( 1 ) ),
			relPeriods[1]/60,relPeriods[1]%60,
			relTmin[1],relTmax[1],
			relHmin[1],relHmax[1],
			
			// Relay 3
			rHint ( relay_status ( 2 ) ),rHint ( !relay_status ( 2 ) ),
			relPeriods[2]/60,relPeriods[2]%60,
			relTmin[2],relTmax[2],
			relHmin[2],relHmax[2]
		   
	);
	return bPage;
}

void ICACHE_FLASH_ATTR onDoneWeb() {
	if ( bPage != NULL ) {
		os_free ( bPage );
		bPage = NULL;
	}
}


void ICACHE_FLASH_ATTR onGETWeb ( char *data, unsigned short length ) {
	char *kv, *sv;
	bool save = false;

	char *str = strstr ( data, " /?" );
	if ( str != NULL ) {
		str = strtok ( str+3," " );

		char* keyval = strtok_r ( str,"&",&kv );
		while ( keyval != NULL ) {
			char *key = strtok_r ( keyval,"=", &sv );
			char *val = strtok_r ( NULL, "=", &sv );

			keyval = strtok_r ( NULL, "&", &kv );

			if ( val != NULL ) {
				if ( strcmp ( key, "reset" ) == 0 ) {

				} else if ( strcmp ( key, "r1" ) == 0 ) {
					relay_status ( 0 ) ?relay_off ( 0 ) :relay_on ( 0 );
				} else if ( strcmp ( key, "r2" ) == 0 ) {
					relay_status ( 1 ) ?relay_off ( 1 ) :relay_on ( 1 );
				} else if ( strcmp ( key, "r3" ) == 0 ) {
					relay_status ( 2 ) ?relay_off ( 2 ) :relay_on ( 2 );
				}
				// Relay 1
				else if ( strcmp ( key, "p1m" ) == 0 ) {
					relPeriods[0]=atoi(val)*60+relPeriods[0]%60;
				} else if ( strcmp ( key, "p1s" ) == 0 ) {
					relPeriods[0]=relPeriods[0]/60;
					relPeriods[0]=atoi(val)+relPeriods[0]*60;
				} else if ( strcmp ( key, "t1min" ) == 0 ) {
					relTmin[0]=atoi(val);
				} else if ( strcmp ( key, "t1max" ) == 0 ) {
					relTmax[0]=atoi(val);
				} else if ( strcmp ( key, "h1min" ) == 0 ) {
					relHmin[0]=atoi(val);
				} else if ( strcmp ( key, "h1max" ) == 0 ) {
					relHmax[0]=atoi(val);
				}
				// Relay 2
				else if ( strcmp ( key, "p2m" ) == 0 ) {
					relPeriods[1]=atoi(val)*60+relPeriods[1]%60;
				} else if ( strcmp ( key, "p2s" ) == 0 ) {
					relPeriods[1]=relPeriods[1]/60;
					relPeriods[1]=atoi(val)+relPeriods[1]*60;
				} else if ( strcmp ( key, "t2min" ) == 0 ) {
					relTmin[1]=atoi(val);
				} else if ( strcmp ( key, "t2max" ) == 0 ) {
					relTmax[1]=atoi(val);
				} else if ( strcmp ( key, "h2min" ) == 0 ) {
					relHmin[1]=atoi(val);
				} else if ( strcmp ( key, "h2max" ) == 0 ) {
					relHmax[1]=atoi(val);
				}
				// Relay 3
				else if ( strcmp ( key, "p3m" ) == 0 ) {
					relPeriods[2]=atoi(val)*60+relPeriods[2]%60;
				} else if ( strcmp ( key, "p3s" ) == 0 ) {
					relPeriods[2]=relPeriods[2]/60;
					relPeriods[2]=atoi(val)+relPeriods[2]*60;
				} else if ( strcmp ( key, "t3min" ) == 0 ) {
					relTmin[2]=atoi(val);
				} else if ( strcmp ( key, "t3max" ) == 0 ) {
					relTmax[2]=atoi(val);
				} else if ( strcmp ( key, "h3min" ) == 0 ) {
					relHmin[2]=atoi(val);
				} else if ( strcmp ( key, "h3max" ) == 0 ) {
					relHmax[2]=atoi(val);
				}
				// save ?
				else if ( strcmp ( key, "save" ) == 0 ) {
					save=true;
				}
			} 
		}
	}
	// Если сохранить нам надо - то все клево сохраняем и настройки применяем!
	if( save ){
		
	}
}
