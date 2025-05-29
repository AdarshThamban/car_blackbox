/*
 * File:   car_black_box.c
 * Author: Adarsh
 *
 * Created on August 1, 2024, 12:21 PM
 */


#include "main.h"


extern char ret_time_edit;
unsigned char clock_reg[3], sec, ret_time;
char time[7]; //hh:mm:ss , avoid colon, so size = 7
char log[11]; //hhmmssevsp
int pos = -1, roll_over_flag = 0;
int index = 0;
char *menu[] = {"View log", "Clear log", "Download log", "Set time", "Change passwd"};

void get_time() {
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 

    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';



    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';


    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}

void display_time() {
    get_time();

    clcd_putch(time[0], LINE2(1));
    clcd_putch(time[1], LINE2(2));
    clcd_putch(':', LINE2(3));
    clcd_putch(time[2], LINE2(4));
    clcd_putch(time[3], LINE2(5));
    clcd_putch(':', LINE2(6));
    clcd_putch(time[4], LINE2(7));
    clcd_putch(time[5], LINE2(8));
}

void display_dashboard(char event[], unsigned char speed) {

    
    
    clcd_print("TIME     E  SP", LINE1(2));
    display_time();
    clcd_print(event, LINE2(11)); // event is a string, so using clcd_print()
    clcd_putch(speed / 10 + '0', LINE2(14)); //convert int to char
    clcd_putch(speed % 10 + '0', LINE2(15));
    
}

void log_event() {
    char addr = 0x05;
    pos++;
    if (pos == 10) {
        pos = 0;
        roll_over_flag = 1;
    }
    addr = pos * 10 + addr;
    eeprom_at24c04_str_write(addr, log);
    

}

void log_car_event(char event[], unsigned char speed) {
    get_time(); // to refresh time from RTC -----------updation
    strncpy(log, time, 6); //hhmmss (6), no need of null
    strncpy(log + 6, event, 2); //ON
    log[8] = speed / 10 + '0';
    log[9] = speed % 10 + '0';
    log[10] = '\0';
    log_event();
}

unsigned char menu_screen(unsigned char key, unsigned char reset_flag) {
    static unsigned char menu_pos;
    if (reset_flag == RESET_LOGIN_MENU) {
        
        menu_pos = 0;
        
    }
    
    if (key == SW4 && menu_pos < 4) {
        menu_pos++;

        clear_screen();
    } else if (key == SW5 && menu_pos > 0) {
        menu_pos--;
        clear_screen();
    }

    if (menu_pos == 4) {
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos ], LINE2(2));
        clcd_putch('*', LINE2(0));
    } else {
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));
        clcd_putch('*', LINE1(0));
    }
    return menu_pos;

}

void clear_screen() {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);

}

unsigned char login(unsigned char key, unsigned char reset_flag) {
    if(ret_time_edit == 1){///looooooooooooonng press
        ret_time =0;
        clear_screen();
    }//////////////
    
    static char u_password[4];
    static unsigned char i;
    static unsigned char attempt_left;

    if (reset_flag == RESET_PASSWORD) { //for clearing the old password when enter into 1st time
        //these code will run for the first time only, while entering the login screen
        i = 0;
        u_password[0] = '\0';
        u_password[1] = '\0';
        u_password[2] = '\0';
        u_password[3] = '\0';
        //initially SW4 or SW5 is used to enter in screen ,it should not record for password entering, so we need to clear that press action
        key = ALL_RELEASED;
        attempt_left = 3;
        ret_time = 5;
    }
    if (ret_time == 0) {
        ret_time_edit = 0;////////////loooooooooong prsss
        return RETURN_BACK;
    }
    if (key == SW4 && i < 4 /*&& reset_flag != RESET_PASSWORD*/) {
        u_password[i] = '1';
        clcd_putch('*', LINE2(4 + i));
        i++;
        ret_time = 5;
    } else if (key == SW5 && i < 4/*&& reset_flag != RESET_PASSWORD*/) {
        u_password[i] = '0';
        clcd_putch('*', LINE2(4 + i));
        i++;
        ret_time = 5;
    }

    if (i == 4) {
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); //turn off cursor
        unsigned char s_password[4];
        for (int j = 0; j < 4; j++) {
            s_password[j] = eeprom_at24c04_read(j);
        }
        if (strncmp(u_password, s_password, 4) == 0) {
            clear_screen();
            clcd_print("LOGIN SUCCESS", LINE1(2));
            __delay_ms(2000);
            return LOGIN_SUCCESS;
            //main menu
        } else { //password is wrong
            attempt_left--;
            clear_screen();

            if (attempt_left != 0) {
                clcd_print("Wrong passwrod", LINE1(0));
                clcd_putch((attempt_left + '0'), LINE2(0));
                clcd_print("attempts left", LINE2(2));
                __delay_ms(3000);

            } else if (attempt_left == 0) {
                clcd_print("You are blocked", LINE1(0));
                clcd_print("wait for", LINE2(0));
                clcd_print("secs", LINE2(12));
                //__delay_ms(2000);
                sec = 60;
                while (sec) {
                    clcd_putch(sec / 10 + '0', LINE2(9));
                    clcd_putch(sec % 10 + '0', LINE2(10));
                }
                //block screen for 15 mins
                //no of min left
                attempt_left = 3;


            }
            clear_screen();
            clcd_print("Enter Password", LINE1(0));
            clcd_write(LINE2(4), INST_MODE);
            clcd_write(DISPLAY_ON_AND_CURSOR_ON, INST_MODE);
            i = 0;
            ret_time = 5;

        }
    }
}

void display_logs(int i) {
    clear_screen();

    clcd_print("# TIME     E  SP", LINE1(0));
    clcd_putch(i + '0', LINE2(0));

    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10)), LINE2(2));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 1), LINE2(3));
    clcd_putch(':', LINE2(4));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 2), LINE2(5));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 3), LINE2(6));
    clcd_putch(':', LINE2(7));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 4), LINE2(8));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 5), LINE2(9));
    //clcd_putch(' ', LINE2(10));

    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 6), LINE2(11));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 7), LINE2(12));

    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 8), LINE2(14));
    clcd_putch(eeprom_at24c04_read(0x05 + (i * 10) + 9), LINE2(15));
}

void view_log(unsigned char key, unsigned char reset_flag) {


    clcd_print("# TIME     E  SP", LINE1(0));
    if (pos == -1) {
        clcd_print("No logs avalable", LINE2(0));
    } else {

        if (reset_flag == RESET_VIEW_LOG_POS) {
            index = 0;
            display_logs(index);
            __delay_ms(1500);
        } else {

            if (key == SW4) {
                if (roll_over_flag) {
                    index++;
                    if (index == 10) {
                        index = 0;
                    }
                } else {
                    index++;
                    if (index > pos) {
                        index = 0;
                    }
                }
                display_logs(index);
            } else if (key == SW5) {
                if (roll_over_flag) {
                    index--;
                    if (index == -1) {
                        index = 9;
                    }
                } else {
                    if (reset_flag == RESET_VIEW_LOG_POS) {
                        (index = pos);
                    } else {
                        index--;

                        if (index == -1) {
                            index = pos;
                        }
                    }
                }
                display_logs(index);
            }
        }



    }
}
void clear_log(){
    
    pos = -1;
    roll_over_flag = 0;
    clcd_print("  logs cleared  ", LINE1(0));
    __delay_ms(2000);
    clear_screen();
}

/* function for downloading the logs */
void download_log(void)
{
    int index = -1;
    char log[11];
    log[10] = 0;
    int position = 0;
    unsigned char add;

    if (pos == -1) /* if no logs available */
    {
        puts("No logs available");
    }
    else /* displaying all logs available */
    {
        puts("Logs :");
        putchar('\n');
        puts("#     Time        Event       Speed");
        putchar('\n');
        putchar('\r');

        while (index < pos)
        {
            position = index + 1;
            index++; /*incrementing to next log */

            for (int i = 0; i < 10; i++)
            {

                add = position * 10 + 5 + i; /* updating the address with that of event data */
                log[i] = eeprom_at24c04_read(add); /* reading the event data */
            }

            /* printing index */
            putchar(index + '0');
            puts("   ");

            /*printing hours value */
            putchar(log[0]);
            putchar(log[1]);
            putchar(':');

            /*printing minute  value */
            putchar(log[2]);
            putchar(log[3]);
            putchar(':');

            /*printing second  value */
            putchar(log[4]);
            putchar(log[5]);
            puts("      ");

            /*printing event  character  */
            putchar(log[6]);
            putchar(log[7]);
            puts("            ");

            /*printing speed value */
            putchar(log[8]);
            putchar(log[9]);
            putchar('\n');
            putchar('\r');
        }
    }
}

int change_time(unsigned char key, unsigned char reset_time)
{
    static unsigned int new_time[3];
    static unsigned int blink_pos;
    static unsigned char wait;
    static unsigned char blink;
    static char t_done = 0;
    char buffer;

    if (reset_time == RESET_TIME)
    {
        get_time(); /* get time */

        /*Storing values of new time hours */
        new_time[0] = (time[0] & 0x0F) * 10 + (time[1] & 0x0F);

        /*Storing values of new time minute*/
        new_time[1] = (time[2] & 0x0F) * 10 + (time[3] & 0x0F);

        /*Storing values of new time second */
        new_time[2] = (time[4] & 0x0F) * 10 + (time[5] & 0x0F);

        clcd_print("Time (HH:MM:SS)", LINE1(0));

        blink_pos = 2;
        wait = 0;
        blink = 0;
        t_done = 0;
        key = ALL_RELEASED;
    }

    if (t_done)
        return TASK_FAILURE;

    switch (key)
    {
        case SW4: 
            new_time[blink_pos]++;
            break;

        case SW5:
            blink_pos = (blink_pos + 1) % 3;
            break;

        case LP_SW4: /* storing the new time */

            get_time(); /* get the time */

            buffer = ((new_time[0] / 10) << 4) | new_time[0] % 10; 
            clock_reg[0] = (clock_reg[0] & 0xC0) | buffer; 
            write_ds1307(HOUR_ADDR, clock_reg[0]); 


            buffer = ((new_time[1] / 10) << 4) | new_time[1] % 10; 
            clock_reg[1] = (clock_reg[1] & 0x80) | buffer; 
            write_ds1307(MIN_ADDR, clock_reg[1]); 


            buffer = ((new_time[2] / 10) << 4) | new_time[2] % 10; 
            clock_reg[2] = (clock_reg[2] & 0x80) | buffer;
            write_ds1307(SEC_ADDR, clock_reg[2]);

            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            clcd_print("Time changed", LINE1(2));
            clcd_print("Successfully", LINE2(2));

            t_done = 1;
            __delay_ms(1000);
            return TASK_SUCCESS;
    }
    
    /* conditions for roll over */
    if (new_time[0] > 23)
        new_time[0] = 0;
    if (new_time[1] > 59)
        new_time[1] = 0;
    if (new_time[2] > 59)
        new_time[2] = 0;

    if (wait++ == 1)
    {
        wait = 0;
        blink = !blink;

        /*logic to blink at the current pos*/
        if (blink)
        {
            switch (blink_pos)
            {
                case 0:
                    clcd_print("  ", LINE2(0));
                    __delay_ms(160);
                    break;
                case 1:
                    clcd_print("  ", LINE2(3));
                    __delay_ms(160);
                    break;
                case 2:
                    clcd_print("  ", LINE2(6));
                    __delay_ms(160);
                    break;
            }
        }
    }

    /* Displaying hours field */
    clcd_putch(new_time[0] / 10 + '0', LINE2(0));
    clcd_putch(new_time[0] % 10 + '0', LINE2(1));
    clcd_putch(':', LINE2(2));

    /* Displaying mins field */
    clcd_putch(new_time[1] / 10 + '0', LINE2(3));
    clcd_putch(new_time[1] % 10 + '0', LINE2(4));
    clcd_putch(':', LINE2(5));

    /* Displaying secs field */
    clcd_putch(new_time[2] / 10 + '0', LINE2(6));
    clcd_putch(new_time[2] % 10 + '0', LINE2(7));

    return TASK_FAILURE;
}

int change_password(unsigned char key, char reset_pwd)
{
    static char new_pwd[9];
    static int pwd_pos = 0;

    if (reset_pwd == RESET_PASSWORD)
    {
        strncpy(new_pwd, "        ", 8);  // Clear all 8 chars
        pwd_pos = 0;
        ret_time = 5;
        key = ALL_RELEASED;
        __delay_ms(1500);

        clear_screen();
        clcd_print("Enter new pwd:  ", LINE1(0));
        clcd_write(LINE2(0), INST_MODE); // Cursor to first position
        clcd_write(DISPLAY_ON_AND_CURSOR_ON, INST_MODE); // Cursor blinking ON
        return TASK_FAILURE; // Return early to wait for key input
    }

    if (!ret_time)
        return RETURN_BACK;

    if (pwd_pos == 4)
{
    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); // Temporarily stop blinking
    clcd_print("                 ", LINE2(0)); // Clear LINE2
    clcd_print("Re-enter new pwd", LINE1(0));

    // Reposition cursor at start for re-entry
    clcd_write(LINE2(0), INST_MODE);
    clcd_write(DISPLAY_ON_AND_CURSOR_ON, INST_MODE); // Resume blinking
}

    // Handle key input
    if (pwd_pos < 8)
    {
        switch (key)
        {
            case SW4: // '1'
                new_pwd[pwd_pos] = '1';
                clcd_putch('*', LINE2(pwd_pos % 4));
                pwd_pos++;
                ret_time = 5;
                break;

            case SW5: // '0'
                new_pwd[pwd_pos] = '0';
                clcd_putch('*', LINE2(pwd_pos % 4));
                pwd_pos++;
                ret_time = 5;
                break;

            default:
                break;
        }

        // Update cursor to next position
        if (pwd_pos < 8)
        {
            clcd_write(LINE2(pwd_pos % 4), INST_MODE); // Move to next position
        }
    }

    // After password entry
    if (pwd_pos == 8)
    {
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); // Stop blinking

        if (strncmp(new_pwd, &new_pwd[4], 4) == 0)
        {
            new_pwd[8] = '\0';
            eeprom_at24c04_str_write(0x00, &new_pwd[4]);
            clear_screen();
            clcd_print("Password changed", LINE1(0));
            clcd_print("successfully", LINE2(2));
            __delay_ms(2000);
            return TASK_SUCCESS;
        }
        else
        {
            clear_screen();
            clcd_print("Password  change", LINE1(0));
            clcd_print("failed", LINE2(5));
            __delay_ms(2000);
            return TASK_SUCCESS;
        }
    }

    return TASK_FAILURE;
}

