#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint16_t servo[1];

#define PARKINGPOS 700
#define SWITCHOFFPOS 100
#define TIPSWITCH 250
#define PEEKPOS 400

//ISR (TIMER0_OVF_vect)
SIGNAL(TIM0_OVF_vect)
{
	uint16_t servoCount;

	servoCount = servo[0] + 250;
	PORTB |= (1 << PB0);
	while (servoCount--)
		;
	PORTB &= ~(1 << PB0);



}

uint8_t driveServoToPosition(uint16_t pos, uint16_t time)
{

	uint16_t i;

	if (time == 0)
	{
		servo[0] = pos;
		return 0;
	}
	while(servo[0] != pos)
	{
		if ( servo[0] > pos)
		{
			servo[0]--;
		} else {
			servo[0]++;
		}

		for ( i= 0; i < time; i++)
		{
			_delay_ms(1);
		}

		if (!( PINB & ( 1 << PB4)))
		{
			return 1;
		}

	}

	i = 0;
	if (pos == SWITCHOFFPOS)
	{
		while (( PINB & ( 1 << PB4)))
		{
			_delay_ms(1);
			i++;
			if  ( i > 2000)
			{
				return 1;
			}
		}
	}

	return 0;
}

//finish
void normalFastReaction()
{
	driveServoToPosition(SWITCHOFFPOS,0);
}

void slowReaction()
{
	driveServoToPosition(SWITCHOFFPOS,2);
}

void fastWaitFastReaction()
{
	driveServoToPosition(TIPSWITCH,0);
	_delay_ms(2000);
	driveServoToPosition(SWITCHOFFPOS,1);
}

void fastBounceFastReaction()
{
	driveServoToPosition(TIPSWITCH,0);
	_delay_ms(200);
	driveServoToPosition(TIPSWITCH + 80 ,0);
	_delay_ms(200);
	driveServoToPosition(TIPSWITCH - 40,0);
	_delay_ms(200);
	driveServoToPosition(TIPSWITCH + 80,0);
	_delay_ms(500);
	driveServoToPosition(SWITCHOFFPOS,1);
}

void verySlowReaction()
{
	driveServoToPosition(TIPSWITCH,10);
	_delay_ms(200);
	driveServoToPosition(SWITCHOFFPOS,1);
}

void shiverAngryReaction()
{
	uint8_t i;
	driveServoToPosition(TIPSWITCH,0);
	_delay_ms(200);
	for ( i = 0; i < 25; i++)
	{
		if (i % 2)
		{
			driveServoToPosition(TIPSWITCH - i,0);
		} else {
			driveServoToPosition(TIPSWITCH + i,0);
		}
		_delay_ms(50);
	}
	driveServoToPosition(SWITCHOFFPOS,1);
}


void peekBeforeReact()
{
	driveServoToPosition(PEEKPOS,0);
	_delay_ms(2000);
	driveServoToPosition(PARKINGPOS,10);
	_delay_ms(500);
	driveServoToPosition(SWITCHOFFPOS,1);
}

void peekBeforeReact2()
{
	driveServoToPosition(PEEKPOS,0);
	_delay_ms(2000);
	driveServoToPosition(PEEKPOS + 100,0);
	_delay_ms(500);
	driveServoToPosition(PEEKPOS,5);
	_delay_ms(500);
	driveServoToPosition(SWITCHOFFPOS,1);
}

void gotcha()
{
	driveServoToPosition(TIPSWITCH - 50,0);
	_delay_ms(200);
	driveServoToPosition(PEEKPOS - 100,15);
	driveServoToPosition(SWITCHOFFPOS,1);
}

int main()
{
	uint8_t i, on;
	DDRB |= ( 1 << PB3); // LED
	DDRB |= ( 1 << PB0); // SERVO1
	DDRB |= ( 1 << PB1); // SERVO2

	TCCR0B |= ( 1 << CS00) | ( 1 << CS01); // no prescale 1
	TIMSK0 |= ( 1 << TOIE0); // Interrupt enable;

	sei();
	servo[0] = 0;


	on = 0;

	while(1)
	{
		if ( PINB & ( 1 << PB4))
		{
			PORTB |= ( 1 << PB3);
			//driveServoToPosition(SWITCHOFFPOS,3);
			switch (i)
			{
			case 0:
				normalFastReaction();
				break;
			case 1:
				slowReaction();
				break;
			case 2:
				fastWaitFastReaction();
				break;
			case 3:
				fastBounceFastReaction();
				break;
			case 4:
				verySlowReaction();
				break;
			case 5:
				shiverAngryReaction();
				break;
			case 6:
				peekBeforeReact();
				break;
			case 7:
				peekBeforeReact2();
				break;
			case 8:
				gotcha();
				break;

			default:
				i = 0;
				break;
			}
			i++;
		} else {
			PORTB &= ~( 1 << PB3);
			driveServoToPosition(PARKINGPOS,0);
		}

	}
}
