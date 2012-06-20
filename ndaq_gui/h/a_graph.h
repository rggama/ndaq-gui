#include <TROOT.h>
#include "defines.h"

//
typedef union _DATA_WORD
{
	unsigned short int word;

	struct
	{
		unsigned char byte0;
		unsigned char byte1;
	};

} DATA_WORD;

typedef union _DATA_DWORD
{
	unsigned int dword;

	struct
	{
		unsigned char byte0;
		unsigned char byte1;
		unsigned char byte2;
		unsigned char byte3;
	};

} DATA_DWORD;

//
void GetDWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned int (*action)(unsigned int data), unsigned int *y);
//
void GetLSWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned short int (*action)(unsigned short int data), unsigned short int *y);
//
void GetMSWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned short int (*action)(unsigned short int data), unsigned short int *y);

//
unsigned int CopyData(unsigned int data);

//
unsigned short int BuildLSWORD(unsigned char *Buffer);
//
unsigned short int BuildMSWORD(unsigned char *Buffer);
//
unsigned int BuildDWORD(unsigned char *Buffer);

//
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y, unsigned char c);
