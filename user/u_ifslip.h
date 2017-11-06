#ifndef U_IFSLIP_H
#define U_IFSLIP_H
#include "user_interface.h"

/**
 * @brief Инициализация интерфейса SLIP
 * 
 * @param ipaddr
 * @param netmask
 * @param gw
 * @return int 0 - все ок
 */
int u_ifsleep_init(ip_addr_t ipaddr, ip_addr_t netmask, ip_addr_t gw);

/**
 * @brief Чтенье конфига, а затем сделать что писано в нем
 * 
 * @return true - все хорошо, сервер поднялся.
 * 	Иначе надо сделать save и перезапустить устройство. Параметы переустановлены по дефолту
 */
bool u_ifsleep_init_cfg();

/**
 * @brief Must be called on UART buffer signal
 * 
 * @return int
 */
void u_ifsleep_process_rxqueue();

/**
 * @brief Берем SLIP-интерфейса все и отдаем комуто
 */
struct ip_info u_ifsleep_ip_info();

#endif // U_IFSLIP_H
