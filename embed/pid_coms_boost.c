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


#define BDRATE_BAUD  9600
#define BUFFSIZE 200

#define TUNING_DELAY 10

#define ADCREF_V     3.3
#define ADCMAXREAD   1023   /* 10 bit ADC */
//Set PID controller constants
#define K_P 0.1
#define K_I 0   //Place holder
#define K_D 0   //Place holder
#define GAMMA 0.176


/* Find out what value gives the maximum
   output voltage for your circuit:
*/
#define PWM_DUTY_MAX 250    

typedef struct pc
{
    //double d;
    double i;
    double p;
    double error;
    double errsum;
    double dt;
} pid_data;

void init_stdio2uart0(void);
int uputchar0(char c, FILE *stream);
int ugetchar0(FILE *stream);
void init_adc(void);
double v_load(void);
double calcPid(pid_data *pid);
void init_pwm(void);
void pwm_duty(uint8_t x);

int main(void)
{
	//uint16_t cnt =0;
    //char cmd[BUFFSIZE];
    //int res;
	
	double prm;
    double newP;
    //Create adc cache variable
    double setPoint;
    double currVoltage;
    double currADC;
    double dutyCycle;
    
	init_stdio2uart0();
	init_pwm(); 
	init_adc();
	pid_data *pid = (pid_data *) malloc(sizeof(pid));
    
    pid->dt = TUNING_DELAY * 1e-3;
    
    //Set default setpoint to the adc value of 1.5 V
    setPoint = 5.0f;
	prm = 50.0f;
    dutyCycle = prm/256;
    
    printf("\r\nIlMatto Coms Boost READY!\r\n");
	sei();
	for(;;)
    {  
        //Retrieve current value for adc        
        //and calculate error relative to setPoint
        currADC = v_load();
        currVoltage = currADC / GAMMA;
        pid->error = currVoltage - setPoint;
        
        //Cache new value of PWM to a variable
        
        dutyCycle -= calcPid(pid);
        newP = dutyCycle * 256;
        
        //Set prm depending on whether newP is out of bounds
        if(newP < 0) prm = 0;
        else if(newP > PWM_DUTY_MAX) prm = PWM_DUTY_MAX;
        else prm = newP;
        
        //Update PWM
        pwm_duty((uint8_t)prm);
        
        
        _delay_ms(TUNING_DELAY);
	}
    free(pid);
    return 0;
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
	UBRR0H = (F_CPU/(BDRATE_BAUD*16L)-1) >> 8;
	UBRR0L = (F_CPU/(BDRATE_BAUD*16L)-1);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
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
    //Calcullate proportional error
    pid->p = K_P * pid->error;
    //Calculate integral error
    pid->i = K_I * pid->errsum * pid->dt;
    pid->errsum += pid->error;
    return pid->p + pid->i;
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