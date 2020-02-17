//=============================================================================
//  stepper.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

#define _MAIN


extern void	initDevices(void);
extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void setMotorSpeed(short ds1);
extern void flushFndDigits(void);
//extern void flushClock(void);
extern void	FailPW(void);
extern void CorrectPW(void);
extern unsigned int fndValue;
extern void SetMotor(void);


#ifdef	_MAIN
int
main(void)
{

	initDevices();
	initLcd();


	while(1){
		flushFndDigits();
	//	flushClock();
		SetMotor();
		if(fndValue > 9999){
			fndValue = fndValue / 10;
			if(fndValue == 1234) { fndValue = 0; CorrectPW();}
			else {fndValue = 0; FailPW();} }
	}
}
#endif
