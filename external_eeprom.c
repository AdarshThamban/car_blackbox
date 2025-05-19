/*
 * File:   external_eeprom.c
 * Author: Adarsh
 *
 * Created on August 2, 2024, 12:11 PM
 */


    #include <xc.h>
#include "i2c.h"
#include "external_eeprom.h"

void init_at24c04(void)
{
    unsigned char dummy;
   
    dummy = eeprom_at24c04_read(SEC_ADDR);
    dummy = dummy & 0x7F;
    eeprom_at24c04_byte_write(SEC_ADDR, dummy);// ch = 0
}

unsigned char eeprom_at24c04_read(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(SLAVE_WRITE_EE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(SLAVE_READ_EE);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void eeprom_at24c04_byte_write(unsigned char addr, unsigned char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(SLAVE_WRITE_EE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void eeprom_at24c04_str_write(unsigned char addr, unsigned char *data)
{
    while(*data != 0)
    {
        eeprom_at24c04_byte_write(addr, *data);
        data++;
        addr ++;
    }
}
