#include "defines.h"

//
typedef union _DATA_WORD
{
	unsigned short word;

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

//32 bits
void GetDWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned int data, unsigned short offset, const void *r), const void *y);
//LS 16 bits
void GetLSWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned short data, unsigned short offset, const void *r), const void *y);
//MS 16 bits
void GetMSWORD(unsigned short offset, unsigned short size, unsigned char *Buffer,
				void (*action)(unsigned short data, unsigned short offset, const void *r), const void *y);
//
signed short _10B_TO_SIGNED_(unsigned short data);

//*r is a pointer to an UNSIGNED INT.
void CopyData(unsigned int data, unsigned short offset, const void *r);
//*r is a pointer to a SIGNED INT.
void GraphData(unsigned short data, unsigned short offset, const void *r);

//
unsigned short BuildLSWORD(unsigned char *Buffer);
//
unsigned short BuildMSWORD(unsigned char *Buffer);
//
unsigned int BuildDWORD(unsigned char *Buffer);
