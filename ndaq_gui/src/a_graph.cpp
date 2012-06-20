#include "a_graph.h"
#include "defines.h"
#include <conio.h>


//32 bits
void GetDWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned int (*action)(unsigned int data), unsigned int *y)
{
	Buffer+=offset;
	for(unsigned char i=0; i<size; i++)
	{
		*y = action(BuildDWORD(Buffer));
		Buffer+=SLOT_SIZE;
	}
}

//LS 16 bits
void GetLSWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned short int (*action)(unsigned short int data), unsigned short int *y)
{
	Buffer+=offset;
	for(unsigned char i=0; i<size; i++)
	{
		*y = action(BuildLSWORD(Buffer));
		Buffer+=SLOT_SIZE;
	}
}

//MS 16 bits
void GetMSWORD(unsigned short int offset, unsigned char size, unsigned char *Buffer, 
			 unsigned short int (*action)(unsigned short int data), unsigned short int *y)
{
	Buffer+=offset;
	for(unsigned char i=0; i<size; i++)
	{
		*y = action(BuildMSWORD(Buffer));
		Buffer+=SLOT_SIZE;
	}
}

//
unsigned int CopyData(unsigned int data)
{
	return data;
}

//LS 16 bits - mode 1
unsigned short int BuildLSWORD(unsigned char *Buffer)
{
	DATA_WORD data;

	data.byte0 = *(Buffer+0);
	data.byte1 = *(Buffer+1);

	return data.word;
}

//MS 16 bits - mode 2
unsigned short int BuildMSWORD(unsigned char *Buffer)
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

//
void T_intgraph(signed char *Buffer, Int_t *x, Int_t *y, unsigned char c){
	
	//signed int temp = 0;
	DATA_WORD		data;
	signed short	temp = 0;
	
	for(UInt_t i=0;i<EVENT_SIZE;i++){
		*x++ = i;
		
		//printf("0x%02X - 0x%02X\n", *(Buffer+1), *(Buffer+0));
		//_getch();

		data.byte0 = (unsigned char)*(Buffer+0+c);
		data.byte1 = (unsigned char)*(Buffer+1+c);

		//printf("0x%04X\n", data.wdata);
		//_getch();

		temp = data.word<<6;

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
