/* 
 * File:   car_black_box.h
 * Author: Adarsh
 *
 * Created on August 1, 2024, 5:47 PM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dashboard(char event[], unsigned char speed);
void log_car_event(char event[], unsigned char speed);
void clear_screen(void);
unsigned char login(unsigned char key, unsigned char reset_flag);
unsigned char menu_screen(unsigned char key, unsigned char reset_flag);
#endif	/* CAR_BLACK_BOX_H */

