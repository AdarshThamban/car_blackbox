/*
 * File:   main.c
 * Author: Adarsh
 *
 * Created on August 1, 2024, 11:03 AM
 */


#include "main.h"
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)
unsigned char ret_time_edit = 0; ////llllllloooooooooooooooooong prss

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
    unsigned char speed = 0; // 00 to 99
    unsigned char gr = 0, key, menu;
    char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4"};
    unsigned long int l_press = 0;
    unsigned char key_st = 0;
    //extern unsigned char menu_pos;

    //log_car_event(event, speed);
    eeprom_at24c04_str_write(0x00, "1111");
    while (1) {
        key = read_digital_keypad(STATE);
        __delay_ms(20); // to avoid bouncing effect

        //        if (key == SW4 || key == SW5) {
        //            if (key != ALL_RELEASED) {
        //                if ((l_press++) > 100) {
        //                    if (key == SW4) {
        //                        key = LP_SW4;
        //                    } else
        //                        key = LP_SW5;
        //                }
        //            } else {
        //                if (l_press < 100 && l_press > 0) {
        //                    l_press = 0;
        //                    if (key == SW4) {
        //                        key = SW4;
        //                    } else
        //                        key = SW5;
        //                }
        //            }
        //
        //        } else
        //            l_press = 0;




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

        } else if ((control_flag == LOGIN_MENU_FLAG) && (key == LP_SW4)) {////////ppppppppppppp
            switch (menu) {
                case 0:
                    control_flag = VIEW_LOG_FLAG;
                    reset_flag = RESET_VIEW_LOG_POS;
                    //key = ALL_RELEASED;
                    break;
                case 1:
                    clear_screen();
                    control_flag = CLEAR_LOG_FLAG;
                    reset_flag = RESET_MEMORY;
                    break;
                case 2:
                    clear_screen();

                    clcd_print("      Open      ", LINE1(0));
                    clcd_print("     Cutecom    ", LINE2(0));
                    download_log();
                    __delay_ms(2000);
                    control_flag = LOGIN_MENU_FLAG;
                    reset_flag = RESET_LOGIN_MENU;
                    break;
                case 3:
                    control_flag = SET_TIME_FLAG;
                    clear_screen();
                    control_flag = SET_TIME_FLAG;
                    reset_flag = RESET_TIME;
                    break;
                case 4:
                    control_flag = CHANGE_PASSWORD_FLAG;
                    clear_screen();
                    clcd_print("Enter new pwd:  ", LINE1(0));
                    reset_flag = RESET_PASSWORD;
                    TMR2ON = 1;
                    break;

            }
        } else if ((control_flag == VIEW_LOG_FLAG)&& (key == LP_SW4)) {////////////pppppppppp
            control_flag = LOGIN_MENU_FLAG;
            reset_flag = RESET_LOGIN_MENU;

        } else if (key == LP_SW4 && control_flag == CHANGE_PASSWORD_FLAG) {
            control_flag = LOGIN_MENU_FLAG;
            clear_screen();
        } else if ((control_flag == LOGIN_MENU_FLAG)&& (key == LP_SW5)) {
            clear_screen();
            control_flag = DASH_BOARD_FLAG;
            ret_time_edit = 1; ////////////////lllllllllllllloooooooooooong press
        }

        switch (control_flag) {
            case DASH_BOARD_FLAG:
                display_dashboard(event, speed);


                break;
            case LOGIN_FLAG:
                //__delay_ms(1000);
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
                view_log(key, reset_flag);
                break;
            case CLEAR_LOG_FLAG:

                clear_log();
                control_flag = LOGIN_MENU_FLAG;
                //reset_flag = RESET_LOGIN_MENU;
                clear_screen();
                break;
            case DOWNLOAD_LOG_FLAG:
                //To Download Log Records
                clear_screen();
                clcd_print("      Open      ", LINE1(0));
                clcd_print("     Cutecom    ", LINE2(0));
                download_log();
                __delay_ms(2000);
                control_flag = LOGIN_MENU_FLAG;
                reset_flag = RESET_LOGIN_MENU;
                break;
            case SET_TIME_FLAG:
                if (change_time(key, reset_flag) == TASK_SUCCESS) {
                    control_flag = LOGIN_MENU_FLAG; /* go back to login menu */
                    reset_flag = RESET_LOGIN_MENU;
                    clear_screen();
                    continue;
                }
                break;
            case CHANGE_PASSWORD_FLAG:
                
                switch (change_password(key, reset_flag)) {
                    case TASK_SUCCESS:
                        __delay_ms(1000);
                        
                        control_flag = LOGIN_MENU_FLAG; /* go back to login menu */
                        reset_flag = RESET_LOGIN_MENU;
                        clear_screen();
                        break;

                    case RETURN_BACK:
                        clear_screen();
                        control_flag = LOGIN_MENU_FLAG; /* go back to dashboard */
                        reset_flag = RESET_LOGIN_MENU;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); //turn off cursor
                        TMR2ON = 0;
                        break;
                }
                break;

        }
        reset_flag = RESET_NOTHING;
    }
    return;
}
