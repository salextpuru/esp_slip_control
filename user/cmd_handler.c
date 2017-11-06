#include "cmd_handler.h"

// копии для всех процедур
static ringbuf_t* rbrx;
static ringbuf_t* rbtx;

enum {
	maxTokens=16,
	// Состояния для токенодробильщика
	stSkip=0,
	stCopy=1
};

/**
 * @brief Список обработчиков команд
 */
static ucmdl* cmdList;

void ICACHE_FLASH_ATTR cmd_handler_init(const ucmdl* cmdlist){
	cmdList = (ucmdl*)cmdlist;
}

ucmdl* searchCmdDsc(const char* cmd){
	ucmdl* l=cmdList;
	
	while( l->cmd ){
		if( !strcmp( l->cmd, cmd ) ){
			break;
		}
		//
		l++;
	};
	
	if(l->cmd == NULL) return NULL;
	
	return(l);
}

static void ICACHE_FLASH_ATTR cmd_handler(int argc, char* argv[]){
	
	if((!cmdList) || (!argc)){
		return;
	}
	
	ucmdl* cmd = searchCmdDsc(argv[0]);
	
	if(cmd){
		cmd->handler( argc, argv, cmd );
	}
	else{
		cmd_handler_printf("I don't know command %s. Use 'help' command for more information.", argv[0]);
	}
}

static void ICACHE_FLASH_ATTR tokenizer() {
	// Нам буфер нужен под токены. Не больше чем у нас их есть!
	int blen=ringbuf_bytes_used ( *rbrx );

	char  tokbuf[blen];
	char* tokens[maxTokens];
	int   token_count=0;
	int   bufcount=0;
	//
	int st=stSkip;
	int i;
	uint8_t c;
	for ( i=0; i<blen; i++ ) {
		ringbuf_memcpy_from ( &c, *rbrx, 1 );
		switch ( st ) {
			case stSkip:{
				if( c>' ' ){
					st=stCopy;
					tokens[token_count++]= tokbuf+bufcount;
					tokbuf[bufcount++]=c;
					tokbuf[bufcount]=0;
				}
				break;
			}
			case stCopy:{
				if( c<=' ' ){
					st=stSkip;
					bufcount++;
					
					// Не желая сигфолта ужасного, мы защиту поставим простую
					// И она нас спасет от опасности, что грозит от сигфолта ужасного
					if( token_count >= maxTokens ){
						i=blen+1;
					}
				}
				else{
					tokbuf[bufcount++]=c;
					tokbuf[bufcount]=0;
				}
				break;
			}
		}
	}
	
	cmd_handler(token_count, tokens);
}

void ICACHE_FLASH_ATTR cmd_handler_exec ( ringbuf_t* rx, ringbuf_t* tx, uint16_t signal ) {
	rbrx=rx;
	rbtx=tx;
	// Бъем на слова мы принятую строку
	tokenizer();
	//ringbuf_copy(*tx, *rx, ringbuf_bytes_used(*rx) );

	// И завершаем все мы ожиданьес команды новой
	ringbuf_memcpy_into ( *rbtx,"\r\nCMD>",6 );
	system_os_post ( 0, signal, 0 );
}

void ICACHE_FLASH_ATTR cmd_handler_printf_copy_tx(char* buf){
	ringbuf_memcpy_into ( *rbtx, buf, strlen(buf) );
}
