#include <TROOT.h>
#include "defines.h"

//
typedef union _ADC_DATA
{
	unsigned short int wdata;

	struct
	{
		unsigned char lsb;
		unsigned char msb;
	};

} ADC_DATA;

typedef union _TCOUNTER_DATA
{
	unsigned int wdata;

	struct
	{
		unsigned char b0;
		unsigned char b1;
		unsigned char b2;
		unsigned char b3;
	};

} TCOUNTER_DATA;

//
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y, unsigned char c);
