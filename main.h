/* 
 * File:   MAIN.h
 * Author: Adarsh
 *
 * Created on August 1, 2024, 5:57 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "car_black_box.h"
#include "external_eeprom.h"
#include <string.h>
#include "timers.h"

#define LOGIN_SUCCESS       0x01
#define RETURN_BACK         0x03

#define DASH_BOARD_FLAG     0x01
#define LOGIN_FLAG          0x02
#define LOGIN_MENU_FLAG     0x04

#define RESET_PASSWORD      0X01
#define RESET_NOTHING       0X00
#define RESET_LOGIN_MENU    0x05

#define DISPLAY_ON_AND_CURSOR_ON    0x0F

#endif	/* MAIN_H */

