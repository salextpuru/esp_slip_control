#include "cmdset.h"
#include "iniconfig.h"
#include "string.h"
#include "wifi_common.h"
#include "portmap.h"
#include "auth_mode.h"
#include "dhtsensors.h"

// Помощь нужна всякому, потому её окажем прямо тут.
// А об остальном пусть пользователь заботится сам
void help_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// О всех параметрах системы доложим враз мы юзерам
void show_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Любой параметр нам установить под силу!
void set_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Сохраним настроечки на память
void save_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Вспомним все
void load_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Пути-дорожки
void portmap_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Что там с файфаем нашим приключилось - покажи!
void wifi_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Сеть сканировать WiFi и вывести из сумрака все AP
void wifi_scan_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

// Считать DHT11 и вывести на консоль
void dht11_handler ( int argc, char* argv[], struct ucmdl* this_cmd );

const ucmdl commndSet[]= {
	{
		.cmd=	"help",
		.hint=	"This help. More see 'help help'.",
		.help=	"There are two form of help: \"short\"\r\n\t\t"
		"# help\r\n\t"
		"and \"long\"\r\n\t\t"
		"# help <command>\r\n\t"
		"Use 'help config' for show info about all config parameters"
		"Use 'help auth' for show info about auth modes",
		.handler=help_handler
	},

	{
		.cmd=	"set",
		.hint=	"set any parameters",
		.help=	"Usage:\r\n\tset <parametr> <value>",
		.handler=set_handler,
	},

	{
		.cmd=	"show",
		.hint=	"show current parameters",
		.help=	"Usage:\r\n\t\"show\" or \"show <parameter>\" or \"show <parameter> help\"",
		.handler=show_handler
	},

	{
		.cmd=	"portmap",
		.hint=	"Map WiFi port to SLIP port",
		.help=	"Usage:\r\n\t"
		"show: portmap show\r\n\t"
		"clear all table: portmap clear\r\n\t"
		"add portmap record:  portmap <path number 0..7> <TCP | UDP | ICMP | IGMP | UDPLITE> <WiFi port> <SLIP port>\r\n\t"
		"del: portmap del <path number 0..7>"
		,
		.handler=portmap_handler
	},

	{
		.cmd=	"save",
		.hint=	"save parameters to flash",
		.help=	"Usage:\r\n\t\"save\" or \"save defaults\"",
		.handler=save_handler,
	},

	{
		.cmd=	"load",
		.hint=	"load parameters from flash",
		.help=	"Usage:\r\n\t\"load\" or \"load defaults\"",
		.handler=load_handler,
	},

	{
		.cmd=	"reset",
		.hint=	"reset device",
		.help=	"Usage:\r\n\t\"reset\"",
		.handler=0,
	},

	{
		.cmd=	"wifi",
		.hint=	"show wifi status",
		.help=	"show status in depends from mode (AP or STATION)",
		.handler=wifi_handler,
	},

	{
		.cmd=	"wifi_scan",
		.hint=	"scan WiFi",
		.help=	"scan WiFi and show all visiable AP",
		.handler=wifi_scan_handler,
	},
	
	{
		.cmd=	"dht11",
		.hint=	"read DHT11 sensor",
		.help=	"DHT11 is sensor of Temperature and Humidity",
		.handler=dht11_handler
	},

	//
	endCmdList
};

void ICACHE_FLASH_ATTR help_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc < 2 ) {
		cmd_handler_printf ( "\r\nShort help page\r\n" );
		const ucmdl* l=commndSet;
		while ( l->cmd ) {
			cmd_handler_printf ( "%s \t- %s\r\n", ( l->cmd ), ( l->hint ) );
			//
			l++;
		};
		return;
	}

	// help config. Do not make command 'config'!
	if ( argc == 2 ) {
		if ( !strcmp ( argv[1],"config" ) ) {
			int params=paramSetSize();
			cfgPar* par=paramSet();

			cmd_handler_printf ( "\r\n-- See all config parametres --\r\n" );

			while ( params ) {
				// С пробела начинаем мы внутренние параметры. Не видно их
				if ( par->name[0] >' ' ) {
					cmd_handler_printf ( "%s:\t%s\r\n", par->name, getCfgParHint ( par->name ) );
				}
				par++;
				params--;
			}

			cmd_handler_printf ( "\r\n-- all done --\r\n" );
			return;
		}
		if ( !strcmp ( argv[1],"auth" ) ) {
			char modes[0x80];
			getAuthNames(modes);
			cmd_handler_printf ( "You can use one of auth mode for AP: %s\r\n", modes );
			return;
		}
	}

	ucmdl* cmd = searchCmdDsc ( argv[1] );

	if ( cmd ) {
		cmd_handler_printf ( "Command: '%s'\r\n\t%s\r\n\t%s", cmd->cmd, cmd->hint, cmd->help );
	} else {
		cmd_handler_printf ( "I don't know command %s. Use 'help' command for show command list.", argv[1] );
	}
}

void ICACHE_FLASH_ATTR show_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc < 2 ) {
		cmd_handler_printf ( "\r\nParameter list:\r\n" );

		int params=paramSetSize();
		cfgPar* par=paramSet();

		while ( params ) {
			// С пробела начинаем мы внутренние параметры. Не видно их
			if ( par->name[0] >' ' ) {
				cmd_handler_printf ( "%s:\t%s\r\n", par->name, par->param );
			}
			par++;
			params--;
		}

	} else if ( argc == 2 ) {
		cfgPar* par=getCfgPar ( argv[1] );
		if ( par ) {
			cmd_handler_printf ( "%s:\t%s\r\n", par->name, par->param );
		} else {
			cmd_handler_printf ( "I don't know parameter %s. Use 'show' command for show all parameters.", argv[1] );
		}
	} else if ( ( argc == 3 ) && ( !strcmp ( argv[2], "help" ) ) ) {
		cfgPar* par=getCfgPar ( argv[1] );
		char*	h=getCfgParHint ( argv[1] );
		if ( ( !h ) || ( !*h ) ) {
			h="no help, sorry.";
		}

		if ( par ) {
			cmd_handler_printf ( "%s:\t%s\r\nHelp: %s\r\n", par->name, par->param, h );
		} else {
			cmd_handler_printf ( "I don't know parameter %s. Use 'show' command for show all parameters.", argv[1] );
		}
	} else {
		cmd_handler_printf ( "I don't know option %s.", argv[2] );
	}
}

void ICACHE_FLASH_ATTR set_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc < 2 ) {
		cmd_handler_printf ( "Wrong 'set' command!\r\n" );
		char* args[]= {"help","set"};
		help_handler ( 2, args, this_cmd );
		return;
	}

	char value[maxParLen]= {};
	int i;
	for ( i=2; i<argc; i++ ) {
		int l=strlen ( argv[i] );
		int v=strlen ( value );

		if ( ( v+l+1 ) >=maxParLen ) {
			break;
		}
		// Если параметр не первый - добавим пробел
		if ( v ) {
			strcpy ( value+v," " );
			v=v+1;
		}
		//
		strcpy ( value+v,argv[i] );
	}

	if ( setCfgPar ( argv[1],value ) ) {
		cmd_handler_printf ( "Parameter '%s' set to '%s'\r\n", argv[1], value );
	} else {
		cmd_handler_printf ( "I don't know parameter '%s'. Use 'show' command for show all parameters.", argv[1] );
	}
}

void ICACHE_FLASH_ATTR save_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc == 1 ) {
		saveCfgPar();
		cmd_handler_printf ( "Parameters has been saved to flash. Use 'reset' to apply.\r\n" );
	} else if ( ( argc == 2 ) && ( !strcmp ( "defaults",argv[1] ) ) ) {
		setDefaults();
		saveCfgPar();
		cmd_handler_printf ( "Parameters has been set to defaults and saved to flash. Use 'reset' to apply.\r\n" );
	} else {
		cmd_handler_printf ( "Wrong 'save' command!\r\n" );
		char* args[]= {"help","save"};
		help_handler ( 2, args, this_cmd );
	}
}

void ICACHE_FLASH_ATTR load_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc == 1 ) {
		loadCfgPar();
		cmd_handler_printf ( "Parameters has been load from flash. No aplied without 'save'!\r\n" );
	} else if ( ( argc == 2 ) && ( !strcmp ( "defaults",argv[1] ) ) ) {
		setDefaults();
		cmd_handler_printf ( "Parameters has been set to defaults. No aplied without 'save'!\r\n" );
	} else {
		cmd_handler_printf ( "Wrong 'load' command!\r\n" );
		char* args[]= {"help","load"};
		help_handler ( 2, args, this_cmd );
	}
}

void ICACHE_FLASH_ATTR reset_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	system_restart();
}

void ICACHE_FLASH_ATTR portmap_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	bool error=true;

	if ( ( argc == 2 ) && ( !strcmp ( argv[1],"show" ) ) ) {
		// portmap show
		showPortsMap();
		error=false;
	} else if ( ( argc == 2 ) && ( !strcmp ( argv[1],"clear" ) ) ) {
		clearPortMapTables();
		cmd_handler_printf ( "All portmap tables has been cleared. 'save' and 'reset' for apply!\r\n", argv[2] );
		PortMapInit();
		error=false;
	} else if ( ( argc == 3 ) && ( !strcmp ( argv[1],"del" ) ) ) {
		// portmap del <path number 0..7>
		char name[maxParLen];
		os_sprintf ( name, " route_%s", argv[2] );
		cfgPar* par=getCfgPar ( name );
		if ( !par ) {
			cmd_handler_printf ( "Wrong number %s\r\n", argv[2] );
		} else {
			memset ( par->param, 0, maxParLen );
			cmd_handler_printf ( "Map %s has been deleted. 'save' and 'reset' for apply!\r\n", argv[2] );
			PortMapInit();
			error=false;
		}
	} else if ( argc == 5 ) {
		// portmap <path number 0..7> <TCP | UDP> <WiFi port> <SLIP port>
		char name[maxParLen];
		char value[maxParLen];
		//
		os_sprintf ( name, " route_%s", argv[1] );
		cfgPar* par=getCfgPar ( name );
		if ( !par ) {
			cmd_handler_printf ( "Wrong number %s\r\n", argv[1] );
		} else {
			// Схороним на всякий случай
			memcpy ( value,par->param,maxParLen );
			//
			memset ( par->param, 0, maxParLen );
			os_sprintf ( par->param,"%s %s %s", argv[2], argv[3], argv[4] );
			cmd_handler_printf ( "Map %s has been set to '%s'. 'save' and 'reset' for apply!\r\n", argv[1],par->param );
			// Если не смогли установить - надо восстановить!
			if ( !PortMapInit() ) {
				cmd_handler_printf ( "Bad portmap definition! Restore from backup\r\n%s\r\n", value );
				memcpy ( par->param,value,maxParLen );
				PortMapInit();
			}
			error=false;
		}
	}
	//
	if ( error ) {
		cmd_handler_printf ( "Wrong 'portmap' command!\r\n" );
		char* args[]= {"help","portmap"};
		help_handler ( 2, args, this_cmd );
	}

}

void ICACHE_FLASH_ATTR wifi_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc !=1 ) {
		cmd_handler_printf ( "Wrong 'wifi' command!\r\n" );
		char* args[]= {"help","wifi"};
		help_handler ( 2, args, this_cmd );
		return;
	}
	//
	wifi_status();
}

void ICACHE_FLASH_ATTR wifi_scan_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc !=1 ) {
		cmd_handler_printf ( "Wrong 'wifi_scan' command!\r\n" );
		char* args[]= {"help","wifi_scan"};
		help_handler ( 2, args, this_cmd );
		return;
	}
	wifi_scan();
}

void ICACHE_FLASH_ATTR dht11_handler ( int argc, char* argv[], struct ucmdl* this_cmd ) {
	if ( argc !=1 ) {
		cmd_handler_printf ( "Wrong 'dht11' command!\r\n" );
		char* args[]= {"help","dht11"};
		help_handler ( 2, args, this_cmd );
		return;
	}
	//
	cmd_handler_printf ( "DHT11 is T=%s H=%s\r\n", dhtTstr(), dhtHstr() );
}
