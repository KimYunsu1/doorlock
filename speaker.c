//=============================================================================
//  speaker.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>

//#define _MAIN
#define		EX_LED	(* (volatile unsigned char *) 0X8008)

extern void				initDevices(void);

extern unsigned char	musicKey;
extern unsigned char	autoPlay;


#ifdef	_MAIN
int
main(void)
{
	initDevices();
	while(1) ;
}
#endif



static unsigned char playOpen[] = {
	0x04, 0x14, 0x24, 0x24, 0x04, 0x24, 0x42, 0x52, 0x44
	};

static unsigned char playClose[] ={
	0x04, 0x24, 0x44, 0x04, 0x24, 0x44, 0x54, 0x54, 0x54, 0x46
	};

static unsigned char playFail[] = {
	0x64, 0x94, 0x64, 0x94, 0x64, 0x94
	};


unsigned char			musicNote = 0;
static unsigned int 	noteLength = 0;


void
playMusic(void)
{
	if(PINB & 0x02)
		autoPlay = 0;
	else if(autoPlay == 1)
		{ if(noteLength > 0) noteLength--;
		  else if(musicNote < 9) {
			musicKey = playOpen[musicNote] >> 4;
			noteLength = 100 * (playOpen[musicNote] & 0x0f );
			musicNote++;
			} else musicKey = 100;
		}
	else if(autoPlay == 3)
		{ if(noteLength > 0) noteLength--;
		  else if(musicNote < 10) {
			musicKey = playClose[musicNote] >> 4;
			noteLength = 100 * (playClose[musicNote] & 0x0f );
			musicNote++;
			}else musicKey = 100;
			 
		}
	else if(autoPlay == 2)
		{ if(noteLength > 0) noteLength--;
		  else if(musicNote < 6) {
			musicKey = playFail[musicNote] >> 4;
			noteLength = 100 * (playFail[musicNote] & 0x0f );
			musicNote++;
			} else musicKey = 100;
		} 
		else 	musicKey = 100;
}

