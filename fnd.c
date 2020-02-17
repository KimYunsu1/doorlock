//=============================================================================
//  FND.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>

//#define _MAIN


extern void	initDevices(void);
extern void delay(int n);
void		flushFndDigits(void);	
void		fndClear(void);	

#ifdef	_MAIN
int
main(void)
{
	initDevices();
	while(1) flushFndDigits();
}
#endif

extern unsigned char	fndDigit[];
unsigned int 			fndValue = 0;
static unsigned char	recalc = 0;

/*void
updateFndDigits(void)
{
	fndValue-;
	recalc = 1;
}
*/



void
fndCountUp(int incr)
{
	fndValue += incr;
	recalc = 1;

}

void
fndDelete(void)
{
	fndValue = fndValue / 10;
	recalc = 1;

}

void
fndClear(void)
{
	fndValue = 0;
	recalc = 1;
}


void
flushFndDigits(void)
{
	if(!recalc) return;
	recalc = 0;


		fndDigit[0] = fndValue / 1000;
		fndDigit[1] = fndValue % 1000 / 100;
		fndDigit[2] = fndValue % 100 / 10;
		fndDigit[3] = fndValue % 10;
		fndValue =fndValue * 10;

}

