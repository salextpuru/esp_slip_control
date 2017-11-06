#ifndef HTTP_SRV_H
#define HTTP_SRV_H

#include "user_interface.h"

/**
 * @brief Очистить все, что нужно нам и подготовить для сеанса
 * 
 * @return true - все хорошо, сервер поднялся.
 * 	Иначе надо сделать save и перезапустить устройство. Параметы переустановлены по дефолту
 */
bool http_tcp_init();

#endif // HTTP_SRV_H
