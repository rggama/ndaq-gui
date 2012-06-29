/**/
#define DEBUG				0
#define FDEBUG				1
#define SAVE3				1

//#define LINUX				1

#define CONFIG_CHAN			0xC0

#define	MAX_CHANNELS		8
#define	MAX_ADC_CHANNELS	8
#define BUFFER				65536

#define SLOT_SIZE			4								//Tamanho em bytes de 1 SLOT
#define HEADER_SIZE			1								//Tamanho em SLOTs de 1 HEADER
#define TIMESTAMP_SIZE		1								//Tamanho em SLOTs de TIMESTAMP
#define ADC_SAMPLES			128								//Quantidade de amostras por trigger para ADC
#define ADC_SIZE			ADC_SAMPLES * 1					//Tamanho em SLOTs de 2 canais de ADC
#define TDC_SIZE			1								//Tamanho em SLOTs de 2 canais de TDC
#define COUNTER_SIZE		2								//Tamanho em SLOTs de 2 canais de COUNTER


#define HEADER_OFFSET		0								//Offset no bloco de dados para HEADER (em SLOTs)
#define TIMESTAMP_OFFSET	1								//Offset no bloco de dados para TIMESTAMP (em SLOTs)
#define ADC_OFFSET			2								//Offset no bloco de dados para ADC (em SLOTs)
#define COUNTER_OFFSET		130								//Offset no bloco de dados para COUNTER (em SLOTs)

//Block Size per FIFO (in SLOTs)
#define FIFO_BS				(HEADER_SIZE+TIMESTAMP_SIZE+ADC_SIZE+COUNTER_SIZE)
//All FIFOs Block Size (in SLOTs)
#define FULL_BS				FIFO_BS * 4
//All FIFOs Block Size in bytes
#define FULL_BS_BYTES		FULL_BS * SLOT_SIZE

#define EVENT_SIZE			128								//Quantidade de samples por trigger
#define BLOCK_SIZE			65472 //2112 //65536
#define	MAX_EVENTS			(BLOCK_SIZE / EVENT_SIZE) 

#define _step_				2

#define CAL_SMP				40	//50

#define BASEOFS				5
#define BASEINT				10 //EVENT_SIZE/4

#define PK_START			5
#define PK_END				120

#define INT_START			32
#define INT_END				60

#define TIMEBIN				8.0f							//ns

#define c16E6				16<<20
#define c16E4				16<<12
#define c16E2				16<<4

#define _J_(x)				(((*((x)+1)<<2)+*(x))<<6)		//10 bits construction.
//#define _J_(x)			(((*((x)+1)<<4)+*(x))<<4)		//12 bits construction.

#define round(x)			((x)>=0?(long)((x)+0.5):(long)((x)-0.5))

#ifdef FDEBUG
	#define FILEDEBUG(s, p0, p1)	{FILE *debugf; debugf = fopen("debug.txt", "a+t"); fprintf(debugf, s, p0, p1); fclose(debugf);}
#else
	#define FILEDEBUG
#endif

/*****************************************************************************/

//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT
//#define	_CRT_SECURE_NO_WARNINGS

/**/
