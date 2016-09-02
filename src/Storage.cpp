#include "Storage.h"

void StorageClass::begin()
{
}

uint8_t StorageClass::getAddr()
{
    uint8_t start = 0;
    return EEPROM.read(start);
}

void StorageClass::getKey(uint8_t * key)
{
    uint8_t start = 1;
    for(uint8_t i = 0; i < KEY_SIZE; i++)
    {
        key[i] = EEPROM.read(start + i);
    }
}

void StorageClass::setAddr(uint8_t addr)
{
    uint8_t start = 0;
    EEPROM.update(start, addr);
}

void StorageClass::setKey(uint8_t * key)
{
    uint8_t start = 1;
    for(uint8_t i = 0; i < KEY_SIZE; i++)
    {
        EEPROM.update(start + i, key[i]);
    }
}

StorageClass Storage;
