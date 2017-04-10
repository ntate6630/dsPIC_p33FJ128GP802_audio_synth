
// 29/01/2012
// Nicholas Tate
// Generate sine waves

#include "p33fxxxx.h"
#include "dsp.h"
#include "math.h"
#define PI 3.14159

int delaycount; int x; int dacout; double w; double sinetable[100];

_FOSCSEL(FNOSC_PRIPLL);    						// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_XT);

_FWDT(FWDTEN_OFF);              				// Watchdog Timer Enabled/disabled by user software. (LPRC can be disabled by clearing SWDTEN bit in RCON register
_FGS(GCP_OFF);

void initDac(void);

int main (void)
{ 
	// Configure Oscillator to operate the device at 40Mhz
 	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
  	// Fosc= 10M*32(2*2)=80Mhz for 10MHz input clock
  	// Fcy  = Fosc / 2 = 40 MIPS
  	PLLFBD = 30;  
  	CLKDIVbits.PLLPOST = 0;   // N1=2
  	CLKDIVbits.PLLPRE  = 0;   // N2=2
  	OSCTUN             = 0;   // Tune FRC oscillator, if FRC is used
  	RCONbits.SWDTEN=0;     // Disable Watch Dog Timer
  	while(OSCCONbits.LOCK!=1) {}; // Wait for PLL to lock	
	
	ADPCFG = 0xFFFF;	//make ADC pins all digital
	TRISA = 0;			//Make all PORTs all outputs
	TRISB = 0;

	initDac();


	for(x=0; x <= 100; x++) 
	{
		w = 2 * PI * x / 100;
		sinetable[x] = 32000 * sin(w);
	}		
	
	DAC1LDAT = 0x8000;
	DAC1RDAT = 0x8000;

	while(1)
	{
		for(x = 0; x <= 100; x++)
		{
			dacout = (signed int)sinetable[x];
		}
		
	//	for(delaycount = 0; delaycount <= 50; delaycount ++)
	//	{
	//	}
	
									
		//Test code to create SQUARE wave.
		//x = 60000;		//Send 'x' to the DAC
		//delay();
		//x = 0;
		//delay();


		//Test code to create SAW wave. 
		//for(x = 0; x < 8192; x ++)
		//{
		//	dacout = x;
		//	delay();
		//}
	}	
}

void initDac(void)
{
	// Initiate DAC Clock
	ACLKCONbits.SELACLK = 0;		// FRC w/ Pll as Clock Source 
	ACLKCONbits.AOSCMD = 0;			// Auxiliary Oscillator Disabled
	ACLKCONbits.ASRCSEL = 0;		// Auxiliary Oscillator is the Clock Source
	ACLKCONbits.APSTSCLR = 6;		// FRC divide by 1, no divide

	DAC1STATbits.LOEN = 1;			// Left Channel DAC Output Enabled
	DAC1STATbits.ROEN = 1;			// Right Channel DAC Output Enabled 

	DAC1STATbits.LITYPE = 0; 		// Left Channel Interrupt if FIFO not full
	DAC1STATbits.RITYPE = 0; 		// Right Channel Interrupt if FIFO not full

	DAC1CONbits.AMPON = 0;			// Analog Output Amplifier is enabled during Sleep Mode/Stop-in Idle mode
	DAC1CONbits.DACFDIV =1;			// Divide High Speed Clock by DACFDIV+1

	DAC1CONbits.FORM = 1;			// Data Format is signed integer
	DAC1DFLT = 0x8000;				// DAC Default value is the midpoint
	
	IFS4bits.DAC1LIF = 0; 			// Clear Left Channel Interrupt Flag 
	IFS4bits.DAC1RIF = 0; 			// Clear Right Channel Interrupt Flag 

	IEC4bits.DAC1LIE = 1; 			// Left Channel Interrupt Enabled 
	IEC4bits.DAC1RIE = 1; 			// Right Channel Interrupt Enabled

	DAC1CONbits.DACEN = 1;			// DAC1 Module Enabled 
}

void __attribute__((interrupt, no_auto_psv))_DAC1LInterrupt(void) 
{
	IFS4bits.DAC1LIF = 0; 			// Clear Left Channel Interrupt Flag 
	DAC1LDAT = dacout;					// User Code to Write to FIFO Goes Here
}

void __attribute__((interrupt, no_auto_psv))_DAC1RInterrupt(void) 
{	 
	IFS4bits.DAC1RIF = 0; 			// Clear Right Channel Interrupt Flag
	DAC1RDAT = dacout;					// User Code to Write to FIFO Goes Here 
}

