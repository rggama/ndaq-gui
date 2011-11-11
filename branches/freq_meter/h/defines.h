/**/
#define DEBUG			0

#define CONFIG_CHAN		0xC0

#define	MAX_CHANNELS	8
#define BUFFER			65536
#define HEADER_SIZE		0
#define EVENT_SIZE		128							//Quantidade de samples por trigger
//#define GROUP_SIZE		0
#define BLOCK_SIZE		8 /*7936 - 63488*/
#define	MAX_EVENTS		(BLOCK_SIZE / EVENT_SIZE) 

#define _step_			2

#define CAL_SMP			50

#define BASEOFS			5
#define BASEINT			20 //EVENT_SIZE/4

//#define PK_START		46
//#define PK_END		70
#define PK_START		60
#define PK_END			100

#define INT_START		32
#define INT_END			60

#define TIMEBIN			8.0f						//ns

//NDAQ-01-2010-ADC1 - 1 Vpp
//#define A				1.2196f
//#define B				-4.73032f

//NDAQ-01-2010-ADC1 - 2 Vpp
#define A				2.53037f
#define B				-5.43545f

#define c16E6			16<<20
#define c16E4			16<<12
#define c16E2			16<<4

#define _J_(x)			(((*((x)+1)<<2)+*(x))<<6)
/**/
