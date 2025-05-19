#include <xc.h>
#include "main.h"
extern unsigned char sec, ret_time;
void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            if(sec > 0){
              sec --;  
            }
            if(sec == 0 && ret_time > 0){ //not in block screen 
                ret_time --;
            }
            count = 0;
            
            
        }
        
        TMR2IF = 0;
    }
}