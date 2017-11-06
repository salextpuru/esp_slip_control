#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include "user_interface.h"
#include "ringbuf.h"

typedef struct ucmdl{
	const char* cmd;
	const char* hint;
	const char* help;
	void (*handler)(int argc, char* argv[], struct ucmdl* this_cmd);
}ucmdl;

#define endCmdList {NULL, NULL, NULL}

/**
 * @brief Готовимся мы обрабатывать команды, что кто-то шлет неутомимо нам
 */
void cmd_handler_init(const ucmdl* cmdlist);

/**
 * @brief Команда нам пришла. И чтож нам делать с нею?
 * 
 * @param rx - хранятся тут команды, что пришли к нам
 * @param tx - сюда ответы ложим мы покорно
 * @param signal - как все ответы нами учтены - сигнал мы посылаем: прочитайте их и передайте дальше
 */
void cmd_handler_exec(ringbuf_t* rx, ringbuf_t* tx, uint16_t signal);

/**
 * @brief Ищем описатель мы, что о команде все нам скажет
 * 
 * @param cmd - команды имя той, что ищем мы
 * @return ucmdl* - вся подноготная команды, что нашли мы. Иль NULL, коли команды нет.
 */
ucmdl* searchCmdDsc(const char* cmd);

/**
 * @brief Печать ответная. В силу глупости интерфейса - сквозб макрос.
 */
void cmd_handler_printf_copy_tx(char* buf);
#define cmd_handler_printf(arg, ...) \
	{ \
		char buf[0x200]; \
		os_sprintf(buf,arg, ##__VA_ARGS__ ); \
		cmd_handler_printf_copy_tx(buf); \
	}

#endif // CMD_HANDLER_H
