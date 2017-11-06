#include "portmap.h"
#include "iniconfig.h"
#include "lwip/ip.h"
#include "cmd_handler.h"

// запись пуста
#define IP_PROTO_EMPTY	0

/**
 * @brief Все записи
 */
static pmapRecord	maps[nPortMapRecords];

// -1 - ошибка
static int getProtoFromStr ( const char* s ) {
	if ( !strcmp ( "ICMP", s ) ) {
		return IP_PROTO_ICMP;
	} else if ( !strcmp ( "IGMP", s ) ) {
		return IP_PROTO_IGMP;
	} else if ( !strcmp ( "UDP", s ) ) {
		return IP_PROTO_UDP;
	} else if ( !strcmp ( "UDPLITE", s ) ) {
		return IP_PROTO_UDPLITE;
	} else if ( !strcmp ( "TCP", s ) ) {
		return IP_PROTO_TCP;
	}
	//
	return -1;
}

static const char* getProtoName ( uint8_t n ) {
	switch ( n ) {
	case IP_PROTO_ICMP: {
		return "ICMP";
	}
	case IP_PROTO_IGMP: {
		return "IGMP";
	}
	case IP_PROTO_UDP: {
		return "UDP";
	}
	case IP_PROTO_UDPLITE: {
		return "UDPLITE";
	}
	case IP_PROTO_TCP: {
		return "TCP";
	}
	default:
		return NULL;
	}
}

bool PortMapInit() {
	bool rval=true;
	memset ( maps,0,sizeof ( maps ) );
	//
	char*	spar[3]= {};
	char*	saveptr=NULL;
	//
	int i;
	for ( i=0; i<nPortMapRecords; i++ ) {
		// portmap show
		char name[maxParLen];
		char value[maxParLen];

		bool ok=true;
		// Извлекаем путь i
		os_sprintf ( name, " route_%i", i );
		cfgPar* par=getCfgPar ( name );
		if ( !par ) {
			continue;
		}
		// Пустой путь - игнор
		if ( !* ( par->param ) ) {
			continue;
		}
		//
		strcpy ( value, par->param );
		//
		int k;
		for ( k=0; k<3; k++ ) {
			if ( ! ( spar[k]=strtok_r ( k?NULL:value," ",&saveptr ) ) ) {
				rval=false;
				ok=false;
				break;
			}
		}
		// Не более 3х токенов!
		if ( strtok_r ( NULL," ",&saveptr ) ) {
			rval=false;
			ok=false;
		}
		//
		pmapRecord	r;
		int proto=getProtoFromStr ( spar[0] );
		int32_t mport=0;
		int32_t dport=0;
		if ( proto < 0 ) {
			rval=false;
			ok=false;
		}
		//
		if ( !str2int ( spar[1], &mport ) ) {
			rval=false;
			ok=false;
		}
		//
		if ( !str2int ( spar[2], &dport ) ) {
			rval=false;
			ok=false;
		}
		//
		if ( ( mport<1 ) || ( dport<1 ) || ( mport>65535 ) || ( dport>65535 ) ) {
			rval=false;
			ok=false;
		}
		//
		if ( !ok ) {
			memset ( par->param, 0, maxParLen );
		} else {
			maps[i].proto = proto;
			maps[i].mport = mport;
			maps[i].dport = dport;
		}
	}
	//
	return ( rval );
}

void showPortsMap() {
	int i;
	for ( i=0; i<nPortMapRecords; i++ ) {
		const char* pn= getProtoName ( maps[i].proto );

		if ( pn ) {
			cmd_handler_printf ( "%i: proto: %s\tmport %i\tdport %i\r\n", i, pn, maps[i].mport, maps[i].dport );
		} else {
			cmd_handler_printf ( "%i: -- empty potrmap record --\r\n", i );
		}
	}
}

void clearPortMapTables() {
	int i;
	for ( i=0; i<nPortMapRecords; i++ ) {
		// portmap show
		char name[maxParLen];

		os_sprintf ( name, " route_%i", i );
		cfgPar* par=getCfgPar ( name );
		if ( !par ) {
			continue;
		}
		memset ( par->param, 0, maxParLen );
	}
}

void setMap( ip_addr_t ext_ip, ip_addr_t int_ip){
	int i;
	for ( i=0; i<nPortMapRecords; i++ ) {
		if( maps[i].proto != IP_PROTO_EMPTY){
			ip_portmap_add(maps[i].proto , ext_ip.addr, maps[i].mport, int_ip.addr, maps[i].dport);
		}
	}
}
