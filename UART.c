//=============================================================================
//  uart.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

//#define _MAIN


extern void	initDevices(void);
extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void gotoLcdXY(unsigned char x, unsigned char y);
extern void writeData(char c);
extern void delay(int n);
extern void fndDelete(void);
extern int	getChar(void);
extern int	peekChar(void);

#ifdef	_MAIN
int
main(void)
{
	initDevices();
	initLcd();

	printLcd(2, 1, "Time:");
	while(1);
}
#endif

extern unsigned int		fndValue;
extern unsigned char	fndDigit[];
extern unsigned char	recalc;
extern unsigned char	fnd;
extern int				type;

void
editCharLcd(void)
{
	int		c;
    int     x ;
	

	while((peekChar() > -1)) {
		c = getChar();
		x = (c & 0x0f);		
		if ( c == 0x08 ){
			fndDelete();	
			printf("%d is hit.\n\r", c);
			}
		else {printf("%d is hit.\n\r", x);
			fndCountUp(x);}
	}
}
