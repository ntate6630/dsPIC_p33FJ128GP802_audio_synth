// Project 4 - 19/02/2012
// Generate more complicated waveforms.
// Use the UART to perform basic comunication with Hyperterminal.
// Setup Traps and test there operation.

#include "p33fxxxx.h"
#include "dsp.h"
#include "math.h"
#define PI 3.14159

#define FCY 40000000
#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16)-1

int x; int dacout1; int dacout2; double w1; double w2; double wavetable1[100]; double wavetable2[100]; double wavetable3[100];

_FOSCSEL(FNOSC_PRIPLL);    						// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_XT);

_FWDT(FWDTEN_OFF);              				// Watchdog Timer Enabled/disabled by user software. (LPRC can be disabled by clearing SWDTEN bit in RCON register
_FGS(GCP_OFF);

void initDAC(void);
void initUART(void);

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

	initUART();
	initDAC();

	for(x=0; x <= 100; x++) 
	{
		w1 = 2 * PI * x / 100;
		wavetable1[x] = 32000 * sin(w1);

		w2 = 4 * PI * x / 100;
		wavetable2[x] = 32000 * sin(w2);
		
		wavetable3[x] = 0.5 * (wavetable1[x] + wavetable2[x]);  // Add the two tables together and reduce the overall value by 0.5 to prevent overflow error at the DAC.
	}	

	while(1)
	{	
		for(x = 0; x <= 100; x++)
		{
			dacout1 = (signed int)wavetable3[x];
			dacout2 = (signed int)wavetable3[x];
		}
	}	
}

void initDAC(void)
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

void initUART(void)
{
	int i;
	
	 __builtin_write_OSCCONL(OSCCON & ~(1<<6));			// Unlock registers
	RPINR18 = 9;										// Make Pin RP9 U1RX
	RPOR4bits.RP8R = 3;									// Make Pin RP8 U1TX
	__builtin_write_OSCCONL(OSCCON | (1<<6));			// Lock registers

	TRISBbits.TRISB9 = 1;			// set TRIS bit 9 to input for the UART1 RX
	
	U1MODEbits . UARTEN = 0;		//Turn OFF UART while configuring it
	U1STAbits . UTXEN = 0;

	U1MODEbits.USIDL = 0; 			// Continue in idle mode
	U1MODEbits.LPBACK = 0; 			// Disable loopback
	U1MODEbits.IREN = 0; 			// Disable the IrDA (infrared encoder / decoder)
	U1MODEbits.RTSMD = 1; 			// Simplex mode (not flow control mode)
	U1MODEbits.UEN = 0; 			// RX ,TX enabled, CTS , RTS disabled
	U1MODEbits.URXINV = 0;       	// Receive Polarity Inversion bit so that idle state is '1'
	U1MODEbits.STSEL = 0; 			// 1-stop bit
	U1MODEbits.PDSEL = 0; 			// No Parity, 8-data bits
	U1MODEbits.ABAUD = 0; 			// Auto-Baud Disabled
	U1MODEbits.BRGH = 0; 			// Low Speed mode
	U1BRG = BRGVAL; 				// BAUD Rate Setting for 9600

	U1STAbits.UTXINV = 0; 			// Ttransmit polarity so that idle state is ’1’
	U1STAbits.UTXBRK = 0; 			// Disable sync break
	U1STAbits.ADDEN = 0; 			// Disable address detect mode

	IFS0bits.U1RXIF = 0;     		// Clear the receive flag
	U1STAbits.URXISEL = 0; 			// Interrupt after one RX character is received;
	IPC2bits.U1RXIP = 4; 			// Set receive interrupt priority level to medium priority level (1= lowest, 7= highest)

	U1STAbits.UTXISEL0 = 0; 		// Interrupt after one Tx character is transmitted
	U1STAbits.UTXISEL1 = 0;
	IEC0bits.U1RXIE = 1; // 		// Enable receive interrupts
	IEC0bits.U1TXIE = 0; 			// Disable UART TX interrupt
	U1MODEbits.UARTEN = 1; 			// Enable UART
	U1STAbits.UTXEN = 1; 			// Enable UART TX

	/* wait at least 104 usec (1/9600) before sending first char */
	for(i = 0; i < 4160; i++)
	{
		Nop();
	}
}

void __attribute__((interrupt, no_auto_psv))_DAC1LInterrupt(void) 
{
	IFS4bits.DAC1LIF = 0; 				// Clear Left Channel Interrupt Flag 
	DAC1LDAT = dacout1;					// User Code to Write to FIFO Goes Here
}

void __attribute__((interrupt, no_auto_psv))_DAC1RInterrupt(void) 
{	 
	IFS4bits.DAC1RIF = 0; 				// Clear Right Channel Interrupt Flag
	DAC1RDAT = dacout2;					// User Code to Write to FIFO Goes Here 
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
	int serialdata;

	IFS0bits.U1RXIF = 0;    			// clear RX interrupt flag 

	serialdata = U1RXREG;

	while(U1STAbits.UTXBF);
	U1TXREG = serialdata;
}

