#ifndef WIFI_STATION_H
#define WIFI_STATION_H

#include "os_type.h"

/**
 * @brief Прочесть пароль и имя, а также все что дале
 * 	и запустить вавай
 */
bool wifi_station_init_cfg();

/**
 * @brief Прочесть что есть сейчас и вывести в командный буфер
 */
void wifi_station_status();

/**
 * @brief Вызывается при сканировании. Вываливает все в Telnet
 */
void scan_done_cb ( void *arg, STATUS status );

#endif // WIFI_STATION_H
