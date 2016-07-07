#include "WSNetwork.h"
#include <VirtualTimer.h>

// Singleton instance of the radio driver
RH_RF69 driver;
// Class to manage message delivery and receipt, using the driver declared above
RHDatagram manager(driver, 0);

static uint8_t recBuffer[RH_RF69_MAX_MESSAGE_LEN];

WSNetwork::WSNetwork()
{
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
