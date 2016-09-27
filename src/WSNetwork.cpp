#include "WSNetwork.h"
#include <VirtualTimer.h>

// Singleton instance of the radio driver
static RH_RF69_PAN driver;
// Class to manage message delivery and receipt, using the driver declared above
static RHDatagram manager(driver, 0);

static uint8_t recBuffer[RH_RF69_MAX_MESSAGE_LEN];

#define ENC_KEY_SIZE 16
static uint8_t voidKey[ENC_KEY_SIZE] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static bool isVoidKey(uint8_t * key)
{
  if(!key) return true;
  for(uint8_t i = 0; i < ENC_KEY_SIZE; i++)
  {
    if(key[i] != voidKey[i]) return false;
  }
  return true;
}

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
  uint8_t pan = Storage.getPan();
  static uint8_t key[ENC_KEY_SIZE];
  Storage.getKey(key);
  if(addr == 0 || addr == 0xFF) return enterPairMode();
  if(pan == 0 || pan == 0xFF) return enterPairMode();
  return begin(addr, pan, key);
}

bool WSNetwork::begin(uint8_t addr, uint8_t pan, uint8_t * key)
{
  manager.setThisAddress(addr);
  if (!manager.init())
    return false;
  driver.setFrequency(915.0);
  driver.setModemConfig(RH_RF69_PAN::GFSK_Rb57_6Fd120);
  driver.setHeaderPan(pan);
  setKey(key);
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
  return begin(0x00, 0x00, NULL);
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
    uint8_t len = RH_RF69_MAX_MESSAGE_LEN;
    if(manager.recvfrom(recBuffer, (uint8_t*)&len, &from))
    {
      // Check if its addressed to us and has encryption key
      if(len >= 21 && recBuffer[0] == 21 && recBuffer[1] == 0x03 && recBuffer[2] == randomId)
      {
        // Save given address
        Storage.setAddr(recBuffer[3]);
        Storage.setPan(recBuffer[4]);
        Storage.setKey(&recBuffer[5]);
        enterNormalMode();
      }
      // No encryption key
      if(len >= 5 && recBuffer[0] == 5 && recBuffer[1] == 0x03 && recBuffer[2] == randomId)
      {
        // Save given address
        Storage.setAddr(recBuffer[3]);
        Storage.setPan(recBuffer[4]);
        Storage.setKey(voidKey);
        enterNormalMode();
      }
    }
  }
}

void WSNetwork::setKey(uint8_t * key)
{
  if(isVoidKey(key)) driver.setEncryptionKey(NULL);
  else driver.setEncryptionKey(key);
}
