/*
 * File:   external_eeprom.c
 * Author: Adarsh
 *
 * Created on August 2, 2024, 12:11 PM
 */


#include "main.h"

unsigned char eeprom_read(unsigned char addr) {
    unsigned char data;

    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EEPROM);
    data = i2c_read(0);
    i2c_stop();

    return data;
}

void eeprom_write(unsigned char addr, unsigned char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(SLAVE_WRITE_EEPROM);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void eeprom_write_string(unsigned char addr, char *data) {
    while (*data != '\0') {
        eeprom_write(addr, *data);
        data++;
        addr++;
    }
}