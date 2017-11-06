#ifndef __DHTXX_H__
#define __DHTXX_H__

#include "c_types.h"

typedef enum {
  DHT11, DHT22
} DHT_Type;

typedef struct {
  float temperature;
  float humidity;
} DHT_Sensor_Output;

typedef struct {
  // private data, don't change anything in here.
  // Set it with the dht_init() function
  uint8_t pin;
  DHT_Type type;
} DHT_Sensor;

/**
 * Quick and dirty sprintf(buffer, "%.2f", value) (two decimals).
 * It is not 100% accurate with the last decimal due to rounding errors,
 * but it seems to be off by at most +-0.01.
 * You better be sure that your buffer is large enough to hold the produced string.
 */
char* dht_float2String(char* buffer, float value);

/**
 * read the sensor (blocking function)
 * returns false if it fails to read the sensor
 */
bool dht_read(DHT_Sensor *sensor, DHT_Sensor_Output* output);

/**
 * Initializes the sensor, sets up the GPIO as output.
 * returns false if it fails (e.g if you use bad pin numbers)
 */
bool dht_init(DHT_Sensor *sensor, DHT_Type dht_type, uint8_t pin);

#endif // __DHTXX_H__
