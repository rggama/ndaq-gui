#include "a_graph.h"

/*
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y){

	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		*y++ = *Buffer++;
	}
}
*/

void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y){
	
	//signed int temp = 0;
	Int_t temp = 0;

	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		
		//printf("%0.2X - %0.2X\n", *Buffer+1, *Buffer);

		//temp=((*(Buffer+1)<<2)+*Buffer)<<6;	
		temp=_J_(Buffer);
		temp=temp>>6;

		*y++ = temp;
		Buffer+=2;
	}
}