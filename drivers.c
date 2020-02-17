//=============================================================================
//  drivers.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

#define		_LED
#define		_SPEAKER
#define		_FND
#define		_DOT_MATRIX
#define		_CLCD
#define		_UART
#define		_STEPPER

#define		EX_CLCD_DATA	    (* (volatile unsigned char *) 0X8000)
#define		EX_CLCD_CONTROL	   	(* (volatile unsigned char *) 0X8001)
#define		EX_FND_DATA	    	(* (volatile unsigned char *) 0X8002)
#define		EX_FND_SELECT		(* (volatile unsigned char *) 0X8003)
#define		EX_DOT_MATRIX_LINE	(* (volatile unsigned int *) 0X8004)
#define		EX_DOT_MATRIX_DATA	(* (volatile unsigned int *) 0X8006)
#define		EX_LED	        	(* (volatile unsigned char *) 0X8008)
#define		EX_STEPPER	       	(* (volatile unsigned char *) 0X8009)


extern void	updateLed(void);
extern void playMusic(void);
//extern void updateFndDigits(void);
extern void fndCountUp(int incr);
extern void turnDotMatrixPage(void);
extern void updateClock(int secs);
extern void editCharLcd(void);
extern void	fndClear(void);
extern void SetTimer(int closetime);

unsigned char		 autoPlay = 0;
extern unsigned char musicNote;

static void toggleSpeaker(void);
static void playPiano(void);
static void updateFnd(void);
static void updateDotMatrix(void);
static void	stepMotor(void);
void SetMotor(void);
void FailPW(void);
void CorrectPW(void);
void CloseDoor(void);

static void
initPorts(void)
{
	DDRG = 0x10;
}

static void
initTimer0(void)
{
	TCCR0 = 0x00;
	TCNT0 = 0x06;
	TCCR0 = 0x04; 
}

static void
initTimer1(void)
{
	TCCR1B = 0x00;
	TCNT1L = 0x00;
	TCNT1H = 0x00;
	TCCR1B = 0x01; 
}


static void
initTimer2(void)
{
	TCCR2 = 0x00;
	OCR2  = 0x13;
	TCNT2 = 0x00;
	TCCR2 = 0x0a;
}

static void
initTimer3(void)
{
	TCCR3B = 0x00;
	TCNT3H = 0xc2;
	TCNT3L = 0xf7;
	TCCR3B = 0x05; 
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 0x06;
#ifdef	_LED
	updateLed();
#endif
#ifdef	_SPEAKER
	if(autoPlay) playMusic();
	else playPiano();
#endif
#ifdef	_FND
	updateFnd();
#endif
}


ISR(TIMER1_OVF_vect)
{
#ifdef	_SPEAKER
	toggleSpeaker();
#endif
}


ISR(TIMER2_COMP_vect)
{
	TCNT2 = 0xce;
#ifdef	_DOT_MATRIX
	updateDotMatrix();
#endif
#ifdef	_STEPPER
	stepMotor();
#endif
}



ISR(TIMER3_OVF_vect)
{
#ifdef	_FND
	TCNT3H = 0xc2;
	TCNT3L = 0xf7;
	//updateFndDigits();
#endif

#ifdef	_DOT_MATRIX
	 turnDotMatrixPage();
#endif

#ifdef	_CLCD
	 updateClock(1);
#endif
	
}

#define 	ATOMIC(x)	{ char _sreg = SREG; cli(); x; SREG = _sreg; }
#define		BUFFSIZE	128

unsigned int	txHead = 0, txTail = 0, rxHead = 0, rxTail = 0;
unsigned char	txBuff[BUFFSIZE], rxBuff[BUFFSIZE];

static void
initUart1(void)
{
	UCSR1B = 0x00;
	UCSR1C = 0x06;
	UBRR1L = 0x67;
	UBRR1H = 0x00;
	UCSR1B = 0xb8;
}

ISR(USART1_RX_vect)
{
	if((rxHead + 1) % BUFFSIZE != rxTail) {
		rxBuff[rxHead] = UDR1;
		rxHead = (rxHead + 1) % BUFFSIZE;
		}
		
#ifdef	_UART
	editCharLcd();
#endif
}


ISR(USART1_UDRE_vect)
{
	if(txHead != txTail) {
		UDR1 = txBuff[txTail];
		txTail = (txTail +1) % BUFFSIZE;
	}
}


static void
putChar(char c) //송신
{
	if(txHead == txTail) {
		if(UCSR1A & (1 << UDRE1)) {
			UDR1 = c;
			return;
		}
	} else if((txHead + 1) % BUFFSIZE == txTail) {
		while(!(UCSR1A & (1 << UDRE1))) ;
		UDR1 = txBuff[txTail];
		ATOMIC(txTail = (txTail + 1) % BUFFSIZE);
	}

		txBuff[txHead] = c;
		ATOMIC(txHead = (txHead + 1) % BUFFSIZE);
}

int
peekChar(void)
{
	if(rxTail == rxHead) return -1;
	return rxBuff[rxTail];
}

int
getChar(void) //수신
{	
	int		c;
	
	if(rxTail == rxHead) {
		while(!(UCSR1A & (1 << RXC1)) );
		return UDR1;
	}
	c = rxBuff[rxTail];
	ATOMIC(rxTail = (rxTail + 1) % BUFFSIZE);
	return c;	
}

void
initDevices(void)
{
	cli();
	initPorts();
	initTimer0();
	initTimer1();
	initTimer2();
	initTimer3();
	initUart1();
	fdevopen(putChar, 0);
	MCUCR = 0x80;
	TIMSK = 0x85; 
	ETIMSK = 0x04;
	sei();
}

unsigned char		musicKey = 100;
static unsigned int musicScale[] = {
	0xc440, 0xcac4, 0xd095, 0xd33b, 0xd817, 0xdc7e, 0xe100, 0xe300, 0xe562
	};


static void
toggleSpeaker(void)
{
	if(musicKey < 9) {
		TCNT1H = musicScale[musicKey] >> 8;
		TCNT1L = musicScale[musicKey] & 0xff;
		PORTG ^= 0x10; 
	} else
		PORTG &= 0xef; 
}
extern unsigned char	flush;
static unsigned char	chatter = 0;


static void
playPiano(void)
{
	chatter++;
	chatter %= 200;


	if(PINB & 0x80){
		if(PINB & 0x01 && !chatter){

		CloseDoor();
}
		else musicKey = 0 ;}
	else if(PINB & 0x20 ) musicKey = 2;
	else if(PINB & 0x10 ) musicKey = 3;
	else if(PINB & 0x08 ) musicKey = 4;
	else if(PINB & 0x04 ) musicKey = 5;
	else if(PINB & 0x02 ) musicKey = 6;
	else if(PINB & 0x01 ) musicKey = 9;
	else musicKey = 100;}

unsigned char				fndDigit[4] = { 0, 0, 0, 0 };
static unsigned char		fndPosition = 0;
static const unsigned char	fndSegment[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67
	};

static void
updateFnd(void)
{
	EX_FND_SELECT = 0x0f;
	EX_FND_DATA = fndSegment[fndDigit[fndPosition]];
	EX_FND_SELECT = ~(0x01 << fndPosition);

	fndPosition++;
	fndPosition %= 4;
}


short					*dotMatrixData = (short *) NULL;
static unsigned char	dotMatrixLine = 0;
static unsigned char	dmCount = 0;

void
updateDotMatrix(void)
{
	if(!dotMatrixData) return;
	if(++dmCount < 20) return;
	dmCount = 0;

	EX_DOT_MATRIX_LINE = 1 << dotMatrixLine;
	EX_DOT_MATRIX_DATA = dotMatrixData[dotMatrixLine];
	dotMatrixLine++;
	dotMatrixLine %= 10;
}

static char		clcdControl = 0;

#define LCD_EN_ON	(EX_CLCD_CONTROL = (clcdControl |= 0x04))
#define LCD_EN_OFF	(EX_CLCD_CONTROL = (clcdControl &= ~0x04))
#define LCD_RW_ON	(EX_CLCD_CONTROL = (clcdControl |= 0x02))
#define LCD_RW_OFF	(EX_CLCD_CONTROL = (clcdControl &= ~0x02))
#define LCD_RS_ON	(EX_CLCD_CONTROL = (clcdControl |= 0x01))
#define LCD_RS_OFF	(EX_CLCD_CONTROL = (clcdControl &= ~0x01))


void
delay(int n)
{
	for(volatile int i = 0; i < n ; i++)
		for(volatile int j = 0; j < 6 ; j++) ;
}

void
writeCommand(char value)
{
	LCD_EN_OFF;
	delay(1);
	LCD_RS_OFF;
	delay(1);
	LCD_RW_OFF;
	delay(1);
	LCD_EN_ON;
	delay(1);
	EX_CLCD_DATA = value;
	delay(1);
	LCD_EN_OFF;
	delay(1);
}

void
writeData(char value)
{
	LCD_EN_OFF;
	delay(1);
	LCD_RS_ON;
	delay(1);
	LCD_RW_OFF;
	delay(1);
	LCD_EN_ON;
	delay(1);
	EX_CLCD_DATA = value;
	delay(1);
	LCD_EN_OFF;
	delay(1);
}

void
initLcd(void)
{
	writeCommand(0x38); //40us
	writeCommand(0x0e);
	writeCommand(0x06);
	writeCommand(0x01); //1.64sec
	delay(100);
	writeCommand(0x02);
	delay(100);
}

void
writeString(char *str)
{
	while(*str)
		writeData(*str++);
}

void
gotoLcdXY(unsigned char x, unsigned char y) {
	switch(y) {
		case 1:
			writeCommand(0x80 + x - 1);
			break;
		case 2:
			writeCommand(0xc0 + x -1);
			break;
	}
}

void
printLcd(int row,int col, char *str)
{
	gotoLcdXY(col, row);
	writeString(str);
}


static short			sm1Speed = 0;
static short			sm1MaxCount = 250;
static short			smMaxCount[] = {
		250, 125, 84, 62, 50, 41, 35, 41, 28, 25
	};

#define MAXSPEED		((short) (sizeof(smMaxCount) / sizeof (short)))

void
setMotorSpeed(short ds1)
{

	if(sm1Speed + ds1 < -MAXSPEED) sm1Speed = -MAXSPEED;
	else if(sm1Speed + ds1 > MAXSPEED) sm1Speed = MAXSPEED;
	else sm1Speed += ds1;

	if(sm1Speed > 0) sm1MaxCount = smMaxCount[sm1Speed - 1];
	else if (sm1Speed < 0) sm1MaxCount = smMaxCount[-sm1Speed -1];
	else sm1MaxCount = 0;
}

static short			sm1Step = 0;
static unsigned char	smStepPhase[] = {
		0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09
	};

#define		NSTEPS (sizeof(smStepPhase) / sizeof(unsigned char))

static short			sm1Count = 0;

void
stepMotor(void)
{
	if(sm1Speed && ++sm1Count == sm1MaxCount) {
		EX_STEPPER = smStepPhase[sm1Step];
		if(sm1Speed > 0) sm1Step = (sm1Step + 1) % NSTEPS;
		else if(--sm1Step < 0) sm1Step = NSTEPS - 1;
		sm1Count = 0;
		}
}

int Open = 1;

void
SetMotor(void)
{
	int		 i , step = 0;

	if(Open == 1) {
			for(i = 0; i < 100; i++) { // 100 쓴 게 1/4바퀴 오른쪽 회전 
			EX_STEPPER = smStepPhase[step];
			step = (step + 1) % NSTEPS;
			delay(10);
		} 
	}
	else if(!Open){
				for(i = 0; i < 100; i++) {
				EX_STEPPER = smStepPhase[step];
				if(--step < 0) step = NSTEPS - 1;
				delay(10);
				//왼쪽으로 회전 
			} 
		}Open = 2;
}


short					Count;
extern unsigned int     fndValue;
extern unsigned int		type;
extern unsigned int		dot;
extern int				secs;
extern long int			seconds;

void
FailPW(void)
{	seconds = 0;
	Count = 0;
	type = 2;
	delay(100);
	musicNote = 0;
	autoPlay = 2;
	delay(100);
	dot = 2;
	fndClear();
	printf("PW is Worng\n\r");
}

void
CorrectPW(void)
{
	SetTimer(30);
	Count = 0;
	type = 1;
	delay(100);
	musicNote = 0;
	autoPlay = 1;
	delay(100);
	dot = 1;
	fndClear();

	Open = 1;
	printf("Door is opened\n\r");
}

void
CloseDoor(void)
{	
	seconds = 0;
	Count = 0;
	type = 3;
	delay(100);
	musicNote = 0;
	autoPlay = 3;
	dot = 3;
	seconds = 86401;
	fndClear();
	Open = 0 ;
	printf("Door is Closed\n\r");
}
