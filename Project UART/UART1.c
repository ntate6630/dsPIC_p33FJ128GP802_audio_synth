// UART Test
// Nicholas Tate - 03/03/2012

#include "p33fxxxx.h"
#include "dsp.h"
#include "math.h"

#define FCY 40000000
#define BAUDRATE 9600
#define BRGVAL ((FCY/BAUDRATE)/16)-1


_FOSCSEL(FNOSC_PRIPLL);    						// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF  & POSCMD_XT);

_FWDT(FWDTEN_OFF);              				// Watchdog Timer Enabled/disabled by user software. (LPRC can be disabled by clearing SWDTEN bit in RCON register
_FGS(GCP_OFF);

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

	while(1)
	{

	}
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

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
	int serialdata;

	IFS0bits.U1RXIF = 0;    			// clear RX interrupt flag 

	serialdata = U1RXREG;

	while(U1STAbits.UTXBF);
	U1TXREG = serialdata;
}

