#include "WSNetwork.h"
#include <VirtualTimer.h>

// Singleton instance of the radio driver
RH_RF69 driver;
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, 0);

static uint8_t recBuffer[RH_RF69_MAX_MESSAGE_LEN];

WSNetwork::WSNetwork()
{
  pairMode = false;
  lastPairReqSent = 0;
  randomSeed(analogRead(7));
  randomId = random(0xFF);
}

bool WSNetwork::begin()
{
  uint8_t addr = Storage.getAddr();
  if(addr == 0 || addr == 0xFF) return enterPairMode();
  return begin(addr);
}

bool WSNetwork::begin(uint8_t addr)
{
  manager.setThisAddress(addr);
  if (!manager.init())
    return false;
  driver.setFrequency(915.0);
  driver.setModemConfig(RH_RF69::GFSK_Rb57_6Fd120);
  return true;
}

int WSNetwork::connect()
{
  return 0;
}

int WSNetwork::read(unsigned char* buffer, int len, unsigned long timeout_ms)
{
  // Dont allow normal operation in pair mode
  if(pairMode) return 0;
  // Wait for a message addressed to us from the client
  uint8_t from;
  if(manager.recvfrom(recBuffer, (uint8_t*)&len, &from))
  {
    memcpy(buffer, recBuffer, len);
    return len;
  }
  return 0;
}

int WSNetwork::write(unsigned char* buffer, int len, unsigned long timeout)
{
  // Dont allow normal operation in pair mode
  if(pairMode) return 0;

  if(manager.sendto(buffer, len, HUB))
  {
    manager.waitPacketSent();
  }
  else
  {
    // failed
    return 0;
  }
  return len;
}

int WSNetwork::disconnect()
{
  return 0;
}

bool WSNetwork::sleep()
{
  return driver.sleep();
}

bool WSNetwork::enterPairMode()
{
  pairMode = true;
  return begin(0x00);
}

bool WSNetwork::enterNormalMode()
{
  pairMode = false;
  return begin();
}

bool WSNetwork::inPairMode()
{
  return pairMode;
}

void WSNetwork::sendPairReq()
{
  uint8_t req[3] = { 3, 0x03, randomId };
  if(manager.sendto(req, 3, HUB))
  {
    manager.waitPacketSent();
  }
}

uint8_t WSNetwork::getAddress()
{
  return manager.thisAddress();
}

// Only needed for pair mode
void WSNetwork::loop()
{
  if(pairMode)
  {
    unsigned long now = millis();
    if(lastPairReqSent + PAIR_REQ_INTERVAL < now)
    {
      sendPairReq();
      lastPairReqSent = now;
    }

    // Handle PAIRRES messages
    uint8_t from;
    uint8_t len = 4;
    if(manager.recvfrom(recBuffer, (uint8_t*)&len, &from))
    {
      // Check if its addressed to us
      if(recBuffer[0] == 4 && recBuffer[1] == 0x03 && recBuffer[2] == randomId)
      {
        // Save given address
        Storage.setAddr(recBuffer[3]);
        enterNormalMode();
      }
    }
  }
}
