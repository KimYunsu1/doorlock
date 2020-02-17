//=============================================================================
//  led.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>

//#define _MAIN
#define		EX_LED	(* (volatile unsigned char *) 0X8008)

extern void	initDevices(void);

#ifdef _MAIN
int
main(void)
{
	initDevices();
	while(1);
}
#endif

static unsigned char 	n1kHzCycles = 0;
static unsigned char	ledData = 0;
static unsigned char	ledDelay = 250;
static unsigned char	ledDir = 0;
extern short			Count = 0;
unsigned int			type = 0;



void
updateLed(void)
{
	n1kHzCycles++;
	int x;
	
	
	if(n1kHzCycles == ledDelay) {
		EX_LED = ledData;
	
		if(type == 1){
			if(Count < 32){
				if(ledDir == 0) { // to left
					if(ledData == 0x80) ledDir = 1;
					else {
						ledData <<= 1;
						ledDelay -=25;
					}
				}else{ // to right
					if(ledData == 0x01) ledDir = 0;
					else {
						ledData >>= 1;
						ledDelay += 25;
						}
					}
				}else ledData =0x00;
				++Count;
				

			}
		else if(type ==0) ledData =0x00;		
		else {
			if(Count < 32){
				x = Count % 2;
				if(x == 1){
				ledData = 0xff;			
				ledDelay +=25;}
				else {
				ledData = 0x00;			
				ledDelay -=25;}
			}
			else ledData = 0x00;}
				++Count;
	n1kHzCycles = 0;
		}


}
