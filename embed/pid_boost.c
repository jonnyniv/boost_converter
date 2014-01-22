/* embedded_coms_boost.c
 *
 *  Author: Steve Gunn & Klaus-Peter Zauner 
 * Licence: This work is licensed under the Creative Commons Attribution License. 
 *          View this license at http://creativecommons.org/about/licenses/
 *   Notes: 
 *          - Use with host_coms_boost.c
 *  
 *          - F_CPU must be defined to match the clock frequency
 *
 *          - Compile with the options to enable floating point
 *            numbers in printf(): 
 *               -Wl,-u,vfprintf -lprintf_flt -lm
 *
 *          - Pin assignment: 
 *            | Port | Pin | Use                         |
 *            |------+-----+-----------------------------|
 *            | A    | PA0 | Voltage at load             |
 *            | D    | PD0 | Host connection TX (orange) |
 *            | D    | PD1 | Host connection RX (yellow) |
 *            | D    | PD7 | PWM out to drive MOSFET     |
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <avr/interrupt.h>

#define BDRATE_BAUD  9600
#define BUFFSIZE 200

#define ADCREF_V     3.3
#define ADCMAXREAD   1023   /* 10 bit ADC */
//Set PID controller constants
#define K_P 0.008	//Proportionality constant
#define K_I 0	//Integral constant
#define K_D K_P/1000	//Derivative constant


#define GAMMA 0.1760299


/* Find out what value gives the maximum
   output voltage for your circuit:
*/
#define PWM_DUTY_MAX 235 
//Create a variable to ensure constant timings between measurements
volatile int cont = 0;
volatile uint8_t v = 0;
volatile uint8_t sp = 0;

ISR(TIMER0_COMPA_vect)
{
    cont = 1;
}

ISR(USART0_RX_vect)
{
    uint8_t recieved;
	recieved = UDR0;
	if(recieved == 0xFF)
	{
		UDR0 = v;
	}
	else if(recieved < 150)
	{
		sp = recieved;
		UDR0 = v;
	}
}

//Create structure to store PID values
typedef struct
{
    double d;
    double i;
    double p;
    double error;
	double erprev;
    //double errsum;
    
	double dt;
} pid_data;

void init_stdio2uart0(void);
int uputchar0(char c, FILE *stream);
int ugetchar0(FILE *stream);
void init_adc(void);
double v_load(void);
double calcPid(pid_data *pid);
void init_pwm(void);
void initTimer();
void pwm_duty(uint8_t x);


int main(void)
{
	//uint16_t cnt =0;
    //char cmd[BUFFSIZE];
    //int res;
	
	double prm;
    double newP;
    double setPoint;
	double currVoltage;
    double currADC;
    double dutyCycle;
	
	init_stdio2uart0();
	init_pwm(); 
	init_adc();
	
	pid_data *pid = (pid_data *) malloc(sizeof(pid));
    
    pid->dt =  0.0025;
    pid->i = 0;
	pid->error = 0;
	pid->d = 0;
    //Set default setpoint to the adc value of 1.5 V
    setPoint = 5.0f;
	prm = 50.0f;
    dutyCycle = prm/256;
    sp = (uint8_t)(setPoint*10);
	
	
    printf("\r\nIlMatto Coms Boost READY!\r\n");
    initTimer();
	sei();
	for(;;)
    {  
		cont = 0;
        //Retrieve current value for adc        
        //and calculate error relative to setPoint
        currADC = v_load();
        currVoltage = currADC / GAMMA;
        pid->error = setPoint - currVoltage;
        
        //Cache new value of PWM to a variable
        
        dutyCycle = dutyCycle + calcPid(pid);
		if(dutyCycle < 0) dutyCycle = 0;
		else if(dutyCycle > 0.90) dutyCycle = 0.90;
        newP = dutyCycle * 256;
        
        //Set prm depending on whether newP is out of bounds
        if(newP < 0) prm = 0;
        else if(newP > PWM_DUTY_MAX) prm = PWM_DUTY_MAX;
        else prm = newP;
        
        //Update PWM
        pwm_duty((uint8_t)prm);
        pid->erprev = pid->error;
        
		v = (uint8_t)(currVoltage * 10);
		if(sp != (uint8_t)(setPoint*10)) setPoint = (double) (sp/10);
        while(!cont);
	}
    free(pid);
    return 0;
}

//This timer is used to ensure polling remains constant
void initTimer()
{
    TCCR0A |= _BV(WGM01);    //CTC Mode 2
            
    TCCR0B |= _BV(CS02);      //Prescalar 1024
            
    OCR0A = 117;
    
    TIMSK0 |= _BV(OCIE0A);  //Enable interrupt
    TIFR0 |= _BV(OCF0A);
}

int uputchar0(char c, FILE *stream)
{
	if (c == '\n') uputchar0('\r', stream);
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = c;
	return c;
}

int ugetchar0(FILE *stream)
{
	while(!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

void init_stdio2uart0(void)
{
	/* Configure UART0 baud rate, one start bit, 8-bit, no parity and one stop bit */
	//Enable Rx complete interrupt
	UBRR0H = (F_CPU/(BDRATE_BAUD*16L)-1) >> 8;
	UBRR0L = (F_CPU/(BDRATE_BAUD*16L)-1);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

	/* Setup new streams for input and output */
	static FILE uout = FDEV_SETUP_STREAM(uputchar0, NULL, _FDEV_SETUP_WRITE);
	static FILE uin = FDEV_SETUP_STREAM(NULL, ugetchar0, _FDEV_SETUP_READ);

	/* Redirect all standard streams to UART0 */
	stdout = &uout;
	stderr = &uout;
	stdin = &uin;
}

double calcPid(pid_data *pid)
{
    //Calculate proportional gain
    pid->p = K_P * pid->error;
	
    //Calculate integral gain
    //pid->i = K_I * pid->errsum * pid->dt;
    //pid->errsum += pid->error;
	
	//Calculate derivative gain
	pid->d = K_D * (pid->error - pid->erprev) / pid->dt;
    return pid->p + pid->d;
}

void init_adc (void)
{
    /* REFSx = 0 : Select AREF as reference
     * ADLAR = 0 : Right shift result
     *  MUXx = 0 : Default to channel 0
     */
    ADMUX = 0x00;
    /*  ADEN = 1 : Enable the ADC
     * ADPS2 = 1 : Configure ADC prescaler
     * ADPS1 = 1 : F_ADC = F_CPU / 64
     * ADPS0 = 0 :       = 187.5 kHz
     */
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1);
}

double v_load(void)
{
     uint16_t adcread;
         
     /* Start single conversion */
     ADCSRA |= _BV ( ADSC );
     /* Wait for conversion to complete */
     while ( ADCSRA & _BV ( ADSC ) );
     adcread = ADC;
    
     printf("ADC=%4d", adcread);  
 
     return (double) (adcread * ADCREF_V/ADCMAXREAD);
}



void init_pwm(void)
{
    /* TIMER 2 */
    DDRD |= _BV(PD6); /* PWM out */
    DDRD |= _BV(PD7); /* inv. PWM out */
    

    TCCR2A = _BV(WGM20) | /* fast PWM/MAX */
	     _BV(WGM21) | /* fast PWM/MAX */
	     _BV(COM2A1); /* A output */
    TCCR2B = _BV(CS20);   /* no prescaling */   
}


/* Adjust PWM duty cycle
   Keep in mind this is not monotonic
   a 100% duty cycle has no switching
   and consequently will not boost.  
*/
void pwm_duty(uint8_t x) 
{
    x = x > PWM_DUTY_MAX ? PWM_DUTY_MAX : x;
    
    printf("PWM=%3u  ==>  ", x);  

    OCR2A = x;
}