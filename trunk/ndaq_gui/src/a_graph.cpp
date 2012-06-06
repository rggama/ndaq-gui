#include "a_graph.h"
#include <conio.h>
/*
//8 bits
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y){

	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		*y++ = *Buffer++;
	}
}
*/


/*
//10 bits - The _J_ function builds a sample from two bytes.
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y, Int_t offset){
	
	//signed int temp = 0;
	Int_t temp = 0;

	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		
		//printf("%0.2X - %0.2X\n", *Buffer+1, *Buffer);

		temp=_J_(Buffer);
		temp=temp>>6;

		*y++ = temp+offset;
		Buffer+=2;
	}
}
*/

void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y, unsigned char c){
	
	//signed int temp = 0;
	ADC_DATA		data;
	signed short	temp = 0;
	
	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		
		//printf("0x%02X - 0x%02X\n", *(Buffer+1), *(Buffer+0));
		//_getch();

		data.lsb = (unsigned char)*(Buffer+0+c);
		data.msb = (unsigned char)*(Buffer+1+c);

		//printf("0x%04X\n", data.wdata);
		//_getch();

		temp = data.wdata<<6;

		//printf("%d\n", temp);
		//_getch();

		//temp=_J_(Buffer);
		temp=temp>>6;

		//printf("%d\n", temp);
		//_getch();

		*y++ = temp;
		Buffer+=4;
	}
}


/*
//10 bits Interleaved
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y){
	
	Int_t temp = 0;

	for(UInt_t i=0;i<EVENT_SIZE;i+=2){
		*x++ = i;	//0..2..4...126
		*x++ = i+1; //1..3..5...127

		temp=_J_(Buffer);		//Even ADC Sample 
		temp=temp>>6;
		*y++ = temp;

		temp=_J_(Buffer+256);	//Odd ADC Sample
		temp=temp>>6;
		*y++ = temp;

		Buffer+=2;
	}
}
*/