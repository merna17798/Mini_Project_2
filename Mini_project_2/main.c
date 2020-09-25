 /******************************************************************************
 *
 * File Name: main.c
 *
 * Description: Application
 *
 * Author: Merna Mohamed
 *
 *******************************************************************************/

#include "adc.h"
#include "lcd.h"
uint16 res_value;
uint32 DC_motor_speed ;

void PWM_Timer0_Init(unsigned char set_duty_cycle)
{

	TCNT0 = 0; //Set Timer Initial value

	OCR0  = set_duty_cycle; // Set Compare Value

	DDRB  = DDRB | (1<<PB3); //set PB3/OC0 as output pin --> pin where the PWM signal is generated from MC.

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
/* External INT1 Interrupt Service Routine */
ISR(INT1_vect)
{
	// Rotate the motor --> anti-clock wise
	PORTB ^= (1<<0);
	PORTB ^= (1<<1);
}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	SREG  &= ~(1<<7);      // Disable interrupts by clearing I-bit
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
	// Trigger INT1 with the rising edge
	MCUCR |= (1<<ISC11)|(1<<ISC10);
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}



int main(void)
{
	INT1_Init();           // Enable and configure external INT1
	/* configure pin PC0 and PC1 as output pins */
	DDRB |= (1<<0)|(1<<1);
	ADC_init(); /* initialize ADC driver */
	PORTB &= (~(1<<0));
	PORTB |= (1<<1);
	//PWM_Timer0_Init(DC_motor_speed);
	LCD_init(); /* initialize LCD driver */

	LCD_clearScreen(); /* clear LCD at the beginning */
	/* display this string "ADC Value = " only once at LCD */
	LCD_displayString("ADC Value = ");
    while(1)
    {
		LCD_goToRowColumn(0,12); /* display the number every time at this position */

    	res_value = ADC_readChannel(0); /* read channel zero where the potentiometer is connect */
    	DC_motor_speed = ( 255 * res_value ) / 1023;
    	PWM_Timer0_Init(DC_motor_speed);

		LCD_intgerToString(res_value); /* display the ADC value on LCD screen */
    }
}


