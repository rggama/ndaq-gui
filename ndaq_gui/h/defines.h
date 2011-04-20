/**/
#define DEBUG			0

#define CONFIG_CHAN		0xC0

#define	MAX_CHANNELS	8
#define BUFFER			65536
#define HEADER_SIZE		0
#define EVENT_SIZE		128							//Quantidade de samples por trigger
//#define GROUP_SIZE		0
#define BLOCK_SIZE		63488 /*7936 - 63488*/
#define	MAX_EVENTS		(BLOCK_SIZE / EVENT_SIZE) 

#define BASEINT			EVENT_SIZE/4

#define TIMEBIN			8.0f						//ns

#define A				1.0f
#define B				0.0f

#define c16E6			16<<20
#define c16E4			16<<12
#define c16E2			16<<4
/**/
