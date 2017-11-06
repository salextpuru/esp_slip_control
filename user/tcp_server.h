#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "user_interface.h"
#include "stdint.h"

/**
 * @brief Создать TCP-сервер
 * 
 * @param port - слушать заданный порт
 * @param callback 
 * @return int 0-ок
 */
bool tcp_server_create(uint16_t port, void (*callback)(void* arg));


#endif // TCP_SERVER_H
