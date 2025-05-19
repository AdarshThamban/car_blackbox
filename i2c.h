/* 
 * File:   i2c.h
 * Author: Adarsh
 *
 * Created on August 1, 2024, 11:03 AM
 */


#ifndef I2C_H
#define	I2C_H

#define FOSC            20000000

void init_i2c(unsigned long baud);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
unsigned char i2c_read(unsigned char ack);
int i2c_write(unsigned char data);

#endif	/* I2C_H */

