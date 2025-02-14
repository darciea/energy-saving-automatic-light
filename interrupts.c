#include <xc.h>
#include "interrupts.h"

//global variable 'hour' defined in interrupts.h

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/

void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source
    TMR0IE=1; //enable interrupts from timer
    INTCONbits.GIEH=1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)
    INTCONbits.GIEL = 1; // Peripheral Interrupt Enable bit
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    
    if (TMR0IF) { //check interrupt source - timer. Timer overflows every minute so interrupt is triggered every minute
        TMR0H=0b00001011;           //testing mode timer values
        TMR0L=0b00001011;
 
        //TMR0H=0b00011011;    //real time timer values        
        //TMR0L=0b00011101;
        minute += 1; //increment minute by 1.
        TMR0IF=0; //clear the interrupt flag
    }
    
    if (PIR2bits.C1IF){ //check the interrupt source - comparator
        if (CM1CON0bits.OUT == 0) { //check if the change is rising edge
            daylight_start_hour = hour; //log the time of the start of daylight hours
            daylight_start_min = minute;
            LATHbits.LATH3=0; // Change LED to off (doesn't matter if we are inside or outside the 1-5am zone)
            }
        else if (CM1CON0bits.OUT == 1){ // check if the change is falling edge
            daylight_end_hour = hour; //log the time of the end of daylight hours
            daylight_end_min = minute;
        if (hour <1 || hour >=5){ //check whether we are outside the 1-5am zone
            if (CM1CON0bits.OUT == 1) {LATHbits.LATH3=1;} //check if the change is falling edge. If so, change LED to on
            else {LATHbits.LATH3=0;} //if not, assume change has been rising edge and turn LED off
        }
        }
        PIR2bits.C1IF=0; //clear the interrupt flag
    }
}


 