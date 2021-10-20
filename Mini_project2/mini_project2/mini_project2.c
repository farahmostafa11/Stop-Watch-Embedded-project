/*
 * ex1.c
 *
 *  Created on: Sep 15, 2021
 *      Author: farah
 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include <util/delay.h>
int seconds=0,minutes=0,hours=0;
ISR(INT0_vect){
	//in case interrupt 0 is enabled (fired) the stop watch should be reseted
	seconds=0;
	minutes=0;
	hours=0;
	TCCR1B&=~0x07;	//No clock source to pause the timer
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);	//CTC mode & clkI/1024/1 (1024 prescaling)
	// restarting the clock to start counting from 0
}
ISR(INT1_vect){
	TCCR1B&=~0x07;	//No clock source to pause the timer
}
ISR(INT2_vect){
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);	//CTC mode & clkI/1024/1 (1024 prescaling)
}
ISR(TIMER1_COMPA_vect){
	seconds++;///////////////////sec units, tenth,minutes units,tenth or by division by 10
	if(seconds==60){
		seconds=0;
		minutes++;
		if(minutes==60){
			hours++;
			minutes=0;
			if(hours==24){
				hours=0;
			}
		}
	}
}
void Timer0_init(void){
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12);	//CTC mode & clkI/1024/1 (1024 prescaling)
	TIMSK=(1<<OCIE1A);  //enabling the compare match interrupt
	TCNT1=0;
	OCR1A=1000; //delay 1 second
}
void stop_watch(void){
	SREG   &= ~(1<<7);       // Disable interrupts by clearing I-bit
	DDRD&=~(0x0C); //addressing pin PD2,PD3(corresponding to INT0,INT1 respectively) to input ports
	PORTD|=(1<<PD2);	//enabling internal pull up of PD2 PIN
	PORTD|=(1<<PB2);	//enabling internal pull up of PB2 PIN
	DDRB&=~(1<<PB2);	//addressing pin PB2(corresponding to INT2) to input ports
	GICR|=(1<<INT0)|(1<<INT1)|(1<<INT2);	//Enabling External Interrupt 0,1,2
	MCUCR|=0x0E;	//setting bits 1,2,3 to ones without affecting other bits
	MCUCR&=~(1<<ISC00);	//setting bit 0 to 0 without affecting other bits
	//define falling edge trigger for interrupt0 by setting bit0->0,bit1->1
	//define rising edge trigger for interrupt1 by setting bit0->1,bit1->1
	MCUCSR&=~(1<<ISC2);	////define falling edge trigger for interrupt2 by setting bit6(ISC2)->0
}
int main(void){
	stop_watch();
	SREG|=(1<<7);      // Enable interrupts by setting I-bit
	Timer0_init();
	DDRC|=0X0F;	//addressing 1st 4-bits of portC to output ports
	DDRA|=(0x7F);	//addressing 1st 6-bits of portA to output ports
	int seg_i=0;
	int val;
	while(1){
		PORTA=(PORTA & 0X80)|(0X7F & (1<<seg_i));
		switch(seg_i){
			case 0:
				val=seconds%10;
				break;
			case 1:
				val=seconds/10;
				break;
			case 2:
				val=minutes%10;
				break;
			case 3:
				val=minutes/10;
				break;
			case 4:
				val=hours%10;
				break;
			case 5:
				val=hours/10;
				break;
		}
		//PORTC&=0X80;
		PORTC = (PORTC & 0xF0) | (val & 0x0F);
		_delay_ms(1);
		seg_i++;
		if(seg_i==7){
			seg_i=0;
		}
	}
}



