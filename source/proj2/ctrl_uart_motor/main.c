/*
 * ctrl_uart_motor.c
 *
 * Created: 2018-09-20 오전 9:33:22
 * Author : usuzin
 */ 

#define F_CPU	16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "UART1.h"
#include "PWM.h"
#include "I2C_LCD.h"

static FILE usart1_in = FDEV_SETUP_STREAM(NULL, USART1_receive, _FDEV_SETUP_RW);
static FILE usart1_out = FDEV_SETUP_STREAM(USART1_send,NULL, _FDEV_SETUP_WRITE);
volatile uint8_t p_count;

void PIN_Init(void);//PORTA0123 사용
enum{Go = 1, Right, Left, Stop};
	
ISR(TIMER0_COMP_vect)
{
	p_count++;
	TCNT0 = 0;
}
	
int main(void)
{
    //초기화
	PIN_Init();
	USART1_init(BR9600);
	Motor_init();
	Motor_TIM_Init();
	I2C_LCD_init();
	
	
	//입출력을 위한 변수
	stdin = &usart1_in;
	stdout = &usart1_out;

	int state_num;	
	
	ForwardMotor_L();
	ForwardMotor_R();
	SpeedMotor_L(1);
	SpeedMotor_R(1);
	_delay_ms(1000);
	
	sei();
	
	//초기 메세지
	I2C_LCD_write_string_XY(0, 0, "USUZIN0510");
	printf("********************************\r\n");
	printf("***********PROJ_2-USZ***********\r\n");
	
    while (1) 
    {
		//상태 설정 메세지
		printf("********************************\r\n");
		printf("***********MotorState.**********\r\n");
		printf("1. Go\r\n");
		printf("2. Right\r\n");
		printf("3. Left\r\n");
		printf("4. Stop\r\n");
		printf("Select Number>> \r\n");
		scanf("%d", &state_num);
		printf("State: %d \r\n", state_num);
			
		PORTA = 0x00;
		//OCR0 = 128; //비교일치 기준값
		//TIMSK |= (1<<OCIE0);
		
		switch(state_num)
		{
			case Go:
				PORTA |= (1 << PORTA0);	//Green
				ForwardMotor_L();
				ForwardMotor_R();
				SpeedMotor_L(80);
				SpeedMotor_R(80);
				_delay_ms(10);
				break;
			case Right:
				PORTA |= (1 << PORTA1);	//Yellow
				BackwardMotor_R();
				ForwardMotor_L();
				SpeedMotor_L(80);
				SpeedMotor_R(80);
				_delay_ms(10);
				break;
			case Left:
				PORTA |= (1 << PORTA2);	//Yellow
				ForwardMotor_R();
				BackwardMotor_L();
				SpeedMotor_L(80);
				SpeedMotor_R(80);
				_delay_ms(10);
				break;
			case Stop:
				PORTA |= (1 << PORTA3);	//Red
				PORTB = PORTE = 0x00;
				//ForwardMotor_L();
				//ForwardMotor_R();
				//SpeedMotor_L(1);
				//SpeedMotor_R(1);
				_delay_ms(1000);
				break;
			default:
				printf("Please check the number of state.\r\n");
				while(state_num = 1 | state_num == 2 | state_num == 3 | state_num == 4)
				{
					PORTA |= (1 << PORTA2);
					_delay_ms(1000);
					PORTA &= ~(1 << PORTA2);
					_delay_ms(1000);
				}
				break;
		}
    }
	//return 0;
}

void PIN_Init(void)
{
	//PORTA의 출력 초기화
	DDRA |= (1 << PORTA0) | (1 << PORTA1) | (1 << PORTA2) | (1 << PORTA3);
	PORTA = 0x0f;
}