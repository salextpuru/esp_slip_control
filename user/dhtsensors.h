#ifndef DHTSENSORS_H
#define DHTSENSORS_H
#include "dhtxx.h"

/**
* @brief Инициализация опроса DHT
*/
void dntSensorsInit();

/**
* @brief Температура в виде строки
*/
char* dhtTstr();

/**
* @brief Влажность в виде строки
*/
char* dhtHstr();

#endif // DHTSENSORS_H
