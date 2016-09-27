#ifndef WSNETWORK_H
#define WSNETWORK_H

#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_RF69_PAN.h>
#include <SPI.h>
#include "Storage.h"

#define HUB 0xF0
#define PAIR_REQ_INTERVAL 5000

class WSNetwork
{
private:
  bool pairMode;
  uint8_t randomId;
  unsigned long lastPairReqSent;
public:
  WSNetwork();
  bool begin();
  bool begin(uint8_t addr, uint8_t pan = 0x01, uint8_t * key = NULL);
  int connect();
  int read(unsigned char* buffer, int len, unsigned long timeout_ms);
  int write(unsigned char* buffer, int len, unsigned long timeout);
  int disconnect();
  bool sleep();

  bool enterPairMode();
  bool enterNormalMode();
  bool inPairMode();
  void sendPairReq();
  void loop();
  uint8_t getAddress();
  void setKey(uint8_t * key = NULL);
};

#endif //WSNETWORK_H
