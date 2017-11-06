#ifndef WIFI_COMMON_H
#define WIFI_COMMON_H

#include "os_type.h"
#include "wifi_station.h"
#include "wifi_ap.h"

/**
 * @brief инициализация вайвая
 * 
 * @return int
 */
bool wifi_init();

/**
 * @brief Вывод состояния WiFi
 */
void wifi_status();

/**
 * @brief Сканировать сеть и вывести доступные AP
 */
void wifi_scan();

#endif // WIFI_COMMON_H
