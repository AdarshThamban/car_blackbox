/* 
 * File:   external_eeprom.h
 * Author: Adarsh
 *
 * Created on August 2, 2024, 12:10 PM
 */

#ifndef AT24C04_H
#define	AT24C04_H

#define SLAVE_WRITE_EE             0xA0 // 
#define SLAVE_READ_EE              0xA1 // 

#define SEC_ADDR                0x00



void init_at24c04(void);
unsigned char eeprom_at24c04_read(unsigned char addr);
void eeprom_at24c04_byte_write(unsigned char addr, unsigned char data);
void eeprom_at24c04_str_write(unsigned char addr, unsigned char *data);

#endif	/* AT24C04_H */
