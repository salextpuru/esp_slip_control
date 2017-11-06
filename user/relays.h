#ifndef RELAYS_H
#define RELAYS_H

void relays_init();

bool relay_status(uint8_t n);

bool relay_on(uint8_t n);

bool relay_off(uint8_t n);

#endif // RELAYS_H
