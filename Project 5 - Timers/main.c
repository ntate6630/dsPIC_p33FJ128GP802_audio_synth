// Project 5 - Timer test routine
// Nicholas Tate 
// 05/03/2012

#include "p33fxxxx.h"
#include "dsp.h"
#include "math.h"
#define PI 3.14159
#define FCY 40000000

_FOSCSEL(FNOSC_PRIPLL);    						// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_XT);

_FWDT(FWDTEN_OFF);              				// Watchdog Timer Enabled/disabled by user software. (LPRC can be disabled by clearing SWDTEN bit in RCON register
_FGS(GCP_OFF);

void initTIMER(void);

int main(void)
{
	// Configure Oscillator to operate the device at 40Mhz
 	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
  	// Fosc= 10M*32(2*2)=80Mhz for 10MHz input clock
  	// Fcy  = Fosc / 2 = 40 MIPS
  	PLLFBD = 30;  
  	CLKDIVbits.PLLPOST = 0;   		// N1=2
  	CLKDIVbits.PLLPRE  = 0;   		// N2=2
  	OSCTUN             = 0;   		// Tune FRC oscillator, if FRC is used
  	RCONbits.SWDTEN=0;     			// Disable Watch Dog Timer
  	while(OSCCONbits.LOCK!=1) {}; 	// Wait for PLL to lock	

	ADPCFG = 0xFFFF;				//make ADC pins all digital
	TRISA = 0;						//Make all PORTs all outputs
	TRISB = 0;

	LATB = 0x0000;	

	initTIMER();

	while(1)
	{

	}
}

void initTIMER(void)
{
	T1CONbits.TON = 0; 		// Disable Timer
	T1CONbits.TCS = 0; 		// Select internal instruction cycle clock
	T1CONbits.TGATE = 0; 	// Disable Gated Timer mode
//	T1CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
	T1CON = 0x8010;			// Fosc/2, 1:8 Prescaler
	TMR1 = 0x0000; 			// Clear timer register
	PR1 = 0x97;				// Load the period value
	IPC0bits.T1IP = 0x04; 	// Set Timer1 Interrupt Priority Level
	IFS0bits.T1IF = 0; 		// Clear Timer1 Interrupt Flag
	IEC0bits.T1IE = 1; 		// Enable Timer1 interrupt
	T1CONbits.TON = 1; 		// Start Timer
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
	int count;
	IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
    T1CONbits.TON = 0;
   	count++;
	if(count == 32767)
	{
		count		   = 0;  
		LATBbits.LATB2 = ~LATBbits.LATB2;		    
	}
    TMR1          = 0;
	T1CONbits.TON = 1;
	/* reset Timer 1 interrupt flag */
}

