//=============================================================================
//  dotMatrix.c --
//=============================================================================

#include	<avr/io.h>
#include	<avr/interrupt.h>

//#define _MAIN


extern void	initDevices(void);

extern short	*dotMatrixData;

static unsigned char	dotMatrixPage = 1;

static short			page1[2][10] = {
	{0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000 }
	,{0x0fc, 0x1fe, 0x303, 0x303, 0x303, 0x303, 0x303, 0x303, 0x1fe, 0x0fc,}};

static short			page2[2][10] = {
	{0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000 },
	{0x000, 0x303, 0x186, 0x0cc, 0x030 ,0x030, 0x0cc, 0x186, 0x303, 0x000 }};

static short			page3[2][10] = {
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000 };

#ifdef	_MAIN
int
main(void)
{
	dotMatrixData = page3[0];
	initDevices();
	while(1) ;
}
#endif

unsigned int	dot = 0;
int 			Count1;
void
turnDotMatrixPage(void)
{	int Count1;
		
		if(Count1 > 3) dot = 3;		

		if(dot == 1){
			dotMatrixData = page1[dotMatrixPage];
			dotMatrixPage++;
			dotMatrixPage %= 2;
			++Count1;
}
		else if(dot == 2){
			dotMatrixData = page2[dotMatrixPage];
			dotMatrixPage++;
			dotMatrixPage %= 2;
			++Count1;}

		else if(dot == 3){
			dotMatrixData = page3[dotMatrixPage];
			dotMatrixPage++;
			dotMatrixPage %= 1;
				++Count1;
}			
	

}
