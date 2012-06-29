#include "a_graph.h"
#include "defines.h"
//#include <conio.h>


//32 bits
void GetDWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned int data, unsigned short offset, const void *r), const void *y)
{
	Buffer+=offset*SLOT_SIZE;
	for(unsigned char i=0; i<size; i++)
	{
		action(BuildDWORD(Buffer), i, y);
		Buffer+=SLOT_SIZE;
	}
}

//LS 16 bits
void GetLSWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned short data, unsigned short offset, const void *r), const void *y)
{
	Buffer+=offset*SLOT_SIZE;
	for(unsigned char i=0; i<size; i++)
	{
		action(BuildLSWORD(Buffer), i, y);
		Buffer+=SLOT_SIZE;
	}
}

//MS 16 bits
void GetMSWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned short data, unsigned short offset, const void *r), const void *y)
{
	Buffer+=offset*SLOT_SIZE;
	for(unsigned char i=0; i<size; i++)
	{
		action(BuildMSWORD(Buffer), i, y);
		Buffer+=SLOT_SIZE;
	}
}

//*r is a pointer to an UNSIGNED INT.
void CopyData(unsigned int data, unsigned short offset, const void *r)
{
	const unsigned int *rptr = (const unsigned int *) r;
	unsigned int *vptr = (unsigned int*)rptr;


	vptr+=offset;
	*vptr = data;
}

//*r is a pointer to a SIGNED INT.
void GraphData(unsigned short data, unsigned short offset, const void *r)
{
	signed short temp = 0;
	const signed int *rptr = (const signed int *) r;
	signed int *vptr = (signed int*)rptr;

	temp = (signed short)(data<<6);
	temp = temp>>6;
	
	vptr+=offset;
	*vptr = temp;
}

//LS 16 bits - mode 1
unsigned short BuildLSWORD(unsigned char *Buffer)
{
	DATA_WORD data;

	data.byte0 = *(Buffer+0);
	data.byte1 = *(Buffer+1);

	return data.word;
}

//MS 16 bits - mode 2
unsigned short BuildMSWORD(unsigned char *Buffer)
{
	DATA_WORD data;

	data.byte0 = *(Buffer+2);
	data.byte1 = *(Buffer+3);

	return data.word;
}

//32 bits - mode 3
unsigned int BuildDWORD(unsigned char *Buffer)
{
	DATA_DWORD data;

	data.byte0 = *(Buffer+0);
	data.byte1 = *(Buffer+1);
	data.byte2 = *(Buffer+2);
	data.byte3 = *(Buffer+3);

	return data.dword;
}
