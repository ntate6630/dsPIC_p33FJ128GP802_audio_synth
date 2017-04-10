// traps.c
// 19/02/2012

#include <p33FJ128GP802.h>


// Standard Exception Vector handlers if ALTIVT (INTCON2<15>) = 0 


void __attribute__((__interrupt__,no_auto_psv)) _OscillatorFail(void)
{
		long int delay;
        INTCON1bits.OSCFAIL = 0;
        while(1)
		{	
			delay = 524288;
			LATB = 0xFFFF;

			while(delay>0)
			{
				delay--;
			}

			delay = 524288;
			LATB = 0x0000;

			while(delay>0)
			{
				delay--;
			}
		}
}

void __attribute__((__interrupt__,no_auto_psv)) _AddressError(void)
{
	Nop();
	Nop();
	Nop();
        INTCON1bits.ADDRERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _StackError(void)
{
	
        INTCON1bits.STKERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _MathError(void)
{
	Nop();
	Nop();
	Nop();
        INTCON1bits.MATHERR = 0;
		LATB = 0xFFFF;
        while(1);
}

/*void __attribute__((__interrupt__,no_auto_psv)) _DMACError(void)
{

        INTCON1bits.DMACERR = 0;
        while(1);
}*/



// Alternate Exception Vector handlers if ALTIVT (INTCON2<15>) = 1 


void __attribute__((__interrupt__,no_auto_psv)) _AltOscillatorFail(void)
{

        INTCON1bits.OSCFAIL = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltAddressError(void)
{

        INTCON1bits.ADDRERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltStackError(void)
{

        INTCON1bits.STKERR = 0;
        while(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _AltMathError(void)
{

        INTCON1bits.MATHERR = 0;
        while(1);
}





