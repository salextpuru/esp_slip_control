#ifndef TCP_TELNET_SRV_H
#define TCP_TELNET_SRV_H

#include "ringbuf.h"
#include "user_interface.h"

/**
 * @brief Портов диапазон телнета. Хоть и один был бы хорош.
 */
enum telnetPortsRange{
	telnetPortMin=1000,
	telnetPortMax=9999
};

/**
 * @brief Очистить все, что нужно нам и подготовить для сеанса
 * 
 * @return true - все хорошо, сервер поднялся.
 * 	Иначе надо сделать save и перезапустить устройство. Параметы переустановлены по дефолту
 */
bool telnet_tcp_init();

// Добраться к данным, принятым в строке от юзера, что нам их посылает
ringbuf_t* telnet_rx_buffer();

// Добраться к данным, что юзеру мы передать хотим
ringbuf_t* telnet_tx_buffer();
 
/**
 * @brief Отправить буфер в никуда иль по дороге дальней
 */
void telnet_tx_send();

/**
 * @brief Процедура-callback для сокета с Telnetom
 * 
 * @param arg 
 */
void telnet_tcp_cb(void *arg);

/**
* @brief Законнекчен ли телнет? Иль все впустую?
* 
* @return bool
*/
bool isTelnetConnected();

#endif // TCP_TELNET_SRV_H
