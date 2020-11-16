/*	Author: lab
 *  Partner(s) Name: Luofeng Xu
 *	Lab Section:022
 *	Assignment: Lab 8  Exercise 3
 *	Exercise Description: I don't understand anything about music, so I just use some random melody searched in google,
 *	it will total last 50*111=5550ms, I assume a little bit more than 5 sec doesn't really matter cause I don't want to cut some part from this melody.
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL>https://youtu.be/wqxdzjlYZHs
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif



volatile unsigned char TimerFlag=0;
unsigned long _avr_timer_M=1;
unsigned long _avr_timer_cntcurr=0;
void TimerOn(){
	TCCR1B=0x0B;
	OCR1A=125;
	TIMSK1=0x02;
	TCNT1=0;
	_avr_timer_cntcurr=_avr_timer_M;
	SREG |=0x80;
}
void TimerOff(){
	TCCR1B=0x00;
}
void TimerISR(){
	TimerFlag=1;
}
ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr==0){
		TimerISR();
		_avr_timer_cntcurr=_avr_timer_M;
	}
}
void TimerSet(unsigned long M){
	_avr_timer_M=M;
	_avr_timer_cntcurr=_avr_timer_M;
}


void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) TCCR3B &= 0x08;
		else TCCR3B |= 0x03; 
		if (frequency < 0.954) OCR3A = 0xFFFF;
		else if (frequency > 31250) OCR3A = 0x0000;
		
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; 
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

const unsigned short tone[8]={261.63,293.66,329.63,349.23,392.00,440.00,493.88,523.25};
const unsigned char melody[37]={8,8,8,8,5,7,8,7,8,5,4,5,4,7,7,6,7,6,5,4,3,4,2,5,4,5,4,7,7,6,7,6,5,4,5,7,8};
unsigned char i;
unsigned char t;
enum states{Start,Wait,Play,Finish} state;

void Tick()
{
	switch(state) 
	{
		case Start:
			state = Wait;
			break;
		case Wait:
			if((~PINA & 0x01)==0x00){
				state = Wait;
			}
			else if((~PINA&0x01)==0x01){
				state = Play;
				i=0;
				t=0;
			}
			break;
		case Play:
			if(t<111){
				state = Play;
			}
			else if(t>=111){
				state=Finish;
			}
			break;
		case Finish:
			if((~PINA & 0x01)!=0x00){
				state=Finish;
			}
			else if((~PINA & 0x01)==0x00){
				state=Wait;
			}
			break;
		default:
			break;
	}
	switch(state)
	{
		case Start:
			set_PWM(0);
			break;
		case Wait:
			i=0;
			t=0;
			set_PWM(0);
			break;
		case Play:
			i=t/3;
			if (t%3!=2){
				set_PWM(tone[melody[i]-1]);
			}
			else if(t%3==2){
				set_PWM(0);
			}
			t++;
			break;
		case Finish:
			set_PWM(0);
			break;
		default:
			break;
		
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = Start;
	PWM_on();
	TimerSet(50);
	TimerOn();
	while(1)
	{
		Tick();
		while(!TimerFlag){};
		TimerFlag=0;
	}
}





















