/**/
#define DEBUG			0

//#define LINUX			1

#define CONFIG_CHAN		0xC0

#define	MAX_CHANNELS	8
#define BUFFER			65536
#define HEADER_SIZE		0
#define EVENT_SIZE		128							//Quantidade de samples por trigger
//#define GROUP_SIZE		0
#define BLOCK_SIZE		48//62400 //63488 //256 //63488 //256 /*7936 - 63488*/
#define	MAX_EVENTS		(BLOCK_SIZE / EVENT_SIZE) 

#define _step_			2

#define CAL_SMP			40	//50

#define BASEOFS			5
#define BASEINT			10 //EVENT_SIZE/4

//#define PK_START		46
//#define PK_END		70
#define PK_START		5
#define PK_END			120

#define INT_START		32
#define INT_END			60

#define TIMEBIN			8.0f						//ns

//NDAQ-01-2010-ADC1 - 1 Vpp
//#define A				1.2196f
//#define B				-4.73032f

//NDAQ-01-2010-ADC1 - 2 Vpp
//#define A				2.53037f
//#define B				-5.43545f

//NDAQ-20-2011-ADC1 - 2 Vpp
#define A	2.59134f
#define B	-4.08396f

#define c16E6			16<<20
#define c16E4			16<<12
#define c16E2			16<<4

#define _J_(x)			(((*((x)+1)<<2)+*(x))<<6)	//10 bits construction.
//#define _J_(x)			(((*((x)+1)<<4)+*(x))<<4)	//12 bits construction.

/**/
