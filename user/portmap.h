#ifndef PORTMAP_H
#define PORTMAP_H

#include "user_interface.h"

/**
 * @brief Одна запись для проброса портов
 */
typedef struct pmapRecord{
	uint8_t		proto;	// протокол
	uint16_t	mport;	// внешний порт
	uint16_t	dport;	// внутренний порт
}pmapRecord;

/**
 * @brief Количество путей
 */
enum {nPortMapRecords=8};

/**
 * @brief - Установить таблицу проброса портов
 * 
 * @param ext_ip - внешний IP
 * @param int_ip - внутренний IP
 */
void setMap( ip_addr_t ext_ip, ip_addr_t int_ip);

/**
 * @brief Считать таблицу из конфига.
 * 
 * @return true - все хорошо, false - плохие записи убиты. надо save() и reset()
 */
bool PortMapInit();

/**
 * @brief Вываливание в кофиг карты портов
 */
void showPortsMap();

/** 
 * @brief Очистить таблицы
 */
void clearPortMapTables();

#endif // PORTMAP_H
