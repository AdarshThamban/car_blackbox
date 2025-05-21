/*
 * File:   main.c
 * Author: Adarsh
 *
 * Created on August 1, 2024, 11:03 AM
 */


#include "main.h"
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)

static void init_config(void) {
    //write initialization code here
    init_i2c(100000); //IT SHOULD BE FIRST 
    init_ds1307();
    init_clcd();
    init_digital_keypad();
    init_adc();
    init_timer2();
    GIE = 1;
    PEIE = 1;
    //init_at24c04();


}

void main(void) {
    init_config();
    unsigned char control_flag = DASH_BOARD_FLAG, reset_flag;
    char event[3] = "ON";
    unsigned char speed = 0, pre_key = 0; // 00 to 99
    unsigned char gr = 0, key, menu;
    char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4"};
    unsigned long int l_press = 0;
    //extern unsigned char menu_pos;

    //log_car_event(event, speed);
    eeprom_at24c04_str_write(0x00, "1111");
    while (1) {
        key = read_digital_keypad(STATE);
        __delay_ms(20); // to avoid bouncing effect

        if (key == SW4 || key == SW5) {
            if (key != ALL_RELEASED) {
                if ((l_press++) > 200) {
                    if (key == SW4) {
                        key = LP_SW4;
                    } else
                        key = LP_SW5;
                }
            } else {
                if (l_press < 200 && l_press > 0) {
                    l_press = 0;
                    if (key == SW4) {
                        key = SW4;
                    } else
                        key = SW5;
                }
            }

        } else
            l_press = 0;




        speed = read_adc() / 10;
        if (speed > 99) {
            speed = 99;
        }
        if (key == SW1) {
            strcpy(event, "C ");
            log_car_event(event, speed);
        } else if (key == SW2 && gr < 6) {
            strcpy(event, gear[gr]);
            gr++;
            log_car_event(event, speed);
        } else if (key == SW3 && gr > 0) {
            gr--;
            strcpy(event, gear[gr]);
            log_car_event(event, speed);
        } else if ((key == SW4 || key == SW5) && control_flag == DASH_BOARD_FLAG) {
            control_flag = LOGIN_FLAG;
            clear_screen();
            clcd_print("Enter Password", LINE1(0));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISPLAY_ON_AND_CURSOR_ON, INST_MODE);
            reset_flag = RESET_PASSWORD; //1st time entering to login screen
            TMR2ON = 1; //timer2 on

        } else if ((control_flag == LOGIN_MENU_FLAG) && (key == LP_SW4)) {
            switch (menu) {
                case 0:
                    control_flag = VIEW_LOG_FLAG;
                    //reset_flag = RESET_VIEW_LOG_POS;
                    //key = ALL_RELEASED;
                    break;
                case 1:
                    control_flag = CLEAR_LOG_FLAG;
                    break;
                case 2:
                    control_flag = DOWNLOAD_LOG_FLAG;
                    break;
                case 3:
                    control_flag = SET_TIME_FLAG;
                    break;
                case 4:
                    control_flag = CHANGE_PASSWORD_FLAG;
                    break;
            }
        }


        switch (control_flag) {
            case DASH_BOARD_FLAG:
                display_dashboard(event, speed);
                break;
            case LOGIN_FLAG:
                switch (login(key, reset_flag)) {
                    case RETURN_BACK: //change screen to dashboard
                        clear_screen();
                        control_flag = DASH_BOARD_FLAG;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); //turn off cursor
                        TMR2ON = 0;
                        break;
                    case LOGIN_SUCCESS:
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); //turn off cursor
                        TMR2ON = 0;
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        //break;
                        continue; /*to skip reset_flag = RESET_NOTHING; at first time ,bcz by using continue controller goto begining of 
                        while loop, without executing below statements*/
                }
                break;
            case LOGIN_MENU_FLAG:
                menu = menu_screen(key, reset_flag);
                break;
            case VIEW_LOG_FLAG:
                //clear_screen();
                view_log(key,reset_flag);

        }
        reset_flag = RESET_NOTHING;
    }
    return;
}
