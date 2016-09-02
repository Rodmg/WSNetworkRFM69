#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>

// 40 bytes + 1 for NULL terminator
#define ADDR_SIZE (1)
#define KEY_SIZE (16)

#define STORAGE_SIZE (ADDR_SIZE + KEY_SIZE)

/*
    Saves and retrieves configured address and encryption key
    EEPROM structure: [ADDR <1>][KEY <16>]
 */

class StorageClass
{
public:
    void begin();

    uint8_t getAddr();
    void getKey(uint8_t * key);

    void setAddr(uint8_t addr);
    void setKey(uint8_t * key);
};

extern StorageClass Storage;

#endif // STORAGE_H
