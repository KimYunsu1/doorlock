//=============================================================================
//  clcd.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>

//#define _MAIN


extern void	initDevices(void);
extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void CloseDoor(void);
extern void delay(int n);

#ifdef	_MAIN
int
main(void)
{

	initDevices();
	initLcd();

	printLcd(2, 1, "Time:");
	printLcd(1, 1, "Noh Woong Jin");
	while(1) flushClock();
}
#endif

long int seconds = 0;
static char		hhmmss[9] = "00:00:00";
extern int		type;

void
updateClock(int secs)
{
	seconds -= secs;
	if(seconds >= 86400) seconds = 0;
	
	if(seconds > -1){
	hhmmss[0] = '0' + seconds / 36000;
	hhmmss[1] = '0' + seconds % 36000 / 3600;
	hhmmss[3] = '0' + seconds % 3600 / 600;
	hhmmss[4] = '0' + seconds % 600 / 60;
	hhmmss[6] = '0' + seconds % 60 / 10;
	hhmmss[7] = '0' + seconds % 10;
	}
	if(type == 1){
		printLcd(1, 1, hhmmss);
		printLcd(2, 1, "CLOSED TIMER:");
		if(seconds == 0) {
			CloseDoor();
			delay(1000);
			printLcd(1, 1, "               ");
			printLcd(2, 1, "CLOSED");
			}
		}
	else if(type == 3) {
		delay(1000);
		printLcd(1, 1, "               ");
		printLcd(2, 1, "CLOSED");
		}
	else if(type == 2){
	 	delay(1000);
		printLcd(1, 1, "               ");	
	 	printLcd(2, 1, "Wrong PW    ");
	 	}
}

void
SetTimer(int closetime)
{
	seconds += closetime;
	seconds %= 40 ;
}
