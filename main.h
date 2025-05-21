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
#define RETURN_BACK         0x02

#define DASH_BOARD_FLAG         0x10
#define LOGIN_FLAG              0x12
#define LOGIN_MENU_FLAG         0x13
#define VIEW_LOG_FLAG           0x14
#define CLEAR_LOG_FLAG          0x15    
#define DOWNLOAD_LOG_FLAG       0x16 
#define SET_TIME_FLAG	 		0x17
#define CHANGE_PASSWORD_FLAG 	0x18    


#define RESET_PASSWORD      0X03
#define RESET_NOTHING       0X04
#define RESET_LOGIN_MENU    0x05
#define RESET_VIEW_LOG_POS  0x06
#define LONG_P              0x07
#define SHORT_P             0x08
#define RESET_MEMORY        0x09

#define DISPLAY_ON_AND_CURSOR_ON    0x0F

#endif	/* MAIN_H */

