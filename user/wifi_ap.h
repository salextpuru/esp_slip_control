#ifndef WIFI_AP_H
#define WIFI_AP_H

#include "os_type.h"

/**
 * @brief Прочесть пароль и имя, а также все что дале
 * 	и запустить вавай
 */
bool wifi_ap_init_cfg();

/**
 * @brief Прочесть что есть сейчас и вывести в командный буфер
 */
void wifi_ap_status();

#endif // WIFI_AP_H
