/*	Author: lab
 *  Partner(s) Name: Luofeng Xu
 *	Lab Section:022
 *	Assignment: Lab 8  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: Youtube URL>https://youtu.be/eF80S82u8pI
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif




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
unsigned char i;
unsigned char flag;
enum states{Start,ON,P,OFF,High,Low} state;

void Tick()
{
	switch(state) 
	{
		case Start:
			state = OFF;
			break;
		case OFF:
			flag=0;
			if((~PINA & 0x07)!=0x01){
				state = OFF;
				break;
			}
			else if((~PINA&0x07)==0x01){
				state = P;
				break;
			}
		case ON:
			flag=1;
			if((~PINA&0x07)==0x01){
				state = P;
				set_PWM(0);
				break;
			}
			if((~PINA&0x07)!=0x01) {
				if((~PINA&0x07)==0x02){
					state = High;
				}
				else if((~PINA&0x07)==0x04){
					state=Low;
				}
				else{
					state=ON;
				}
			}
			break;
		case P:
			if((~PINA&0x07)==0x01){
				state=P;
			}
			else if((~PINA&0x07)==0x00){
				if(flag==0){
					state=ON;
				}
				else if(flag==1){
					state=OFF;
				}
				break;
			}
			break;
		case High:
			if((~PINA&0x07)==0x02){
				state=High;
			}
			else{
				state=ON;
				if(i<7){
					i++;
				}
			}
			break;
		case Low:
			if((~PINA&0x07)==0x04){
                                state=Low;
                        }
                        else{
                                state=ON;
                                if(i>0){
                                        i--;
                                }
                        }
                        break;
		default:
			break;
	}
	switch(state)
	{
		case Start:
			i=0;
			flag=0;
			set_PWM(0);
			break;
		case OFF:
			i=0;
			set_PWM(0);
			break;
		case ON:
			flag=1;
			set_PWM(tone[i]);
			break;
		case High:
			break;
		case Low:
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
	while(1)
	{
		Tick();
	}
}





















