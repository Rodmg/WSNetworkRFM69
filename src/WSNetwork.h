#ifndef WSNETWORK_H
#define WSNETWORK_H

#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <SPI.h>

#define HUB 0xF0

class WSNetwork
{
public:
  WSNetwork();
  bool begin(uint8_t addr);
  int connect();
  int read(unsigned char* buffer, int len, unsigned long timeout_ms);
  int write(unsigned char* buffer, int len, unsigned long timeout);
  int disconnect();
  bool sleep();
};

#endif //WSNETWORK_H
