#include "a_graph.h"

void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y){

	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		*y++ = *Buffer++;
	}
}