/**/

//#define LINUX				1
//#define MAX_TP			1								//Maximum Throughput

#define DEBUG				0
#define FDEBUG				1
//#define SAVE3				0

#define	MAX_CHANNELS		8
#define BUFFER				65536

#define SLOT_SIZE			4								//Tamanho em bytes de 1 SLOT
#define HEADER_SIZE			1								//Tamanho em SLOTs de 1 HEADER
#define TIMESTAMP_SIZE		1								//Tamanho em SLOTs de TIMESTAMP
#define ADC_SAMPLES			128								//Quantidade de amostras por trigger para ADC
#define ADC_SIZE			ADC_SAMPLES * 1					//Tamanho em SLOTs de 2 canais de ADC
#define TDC_SIZE			1								//Tamanho em SLOTs de 1 canal de TDC
#define COUNTER_SIZE		1								//Tamanho em SLOTs de 1 canal de COUNTER

#define HEADER_OFFSET		0								//Offset no bloco de dados para HEADER (em SLOTs)
#define TIMESTAMP_OFFSET	1								//Offset no bloco de dados para TIMESTAMP (em SLOTs)
#define ADC_OFFSET			2								//Offset no bloco de dados para ADC (em SLOTs)
#define ATDC_OFFSET			130								//Offset no bloco de dados para TDC A(em SLOTs)
#define BTDC_OFFSET			131								//Offset no bloco de dados para TDC B(em SLOTs)
#define ACOUNTER_OFFSET		132								//Offset no bloco de dados para COUNTER A(em SLOTs)
#define BCOUNTER_OFFSET		133								//Offset no bloco de dados para COUNTER B(em SLOTs)

//Block Size per FIFO (in SLOTs)
#define FIFO_BS				(HEADER_SIZE+TIMESTAMP_SIZE+ADC_SIZE+(TDC_SIZE*2)+(COUNTER_SIZE*2))

#define CAL_SMP_OFFSET		50	

#define BASE_OFFSET			5
#define BASE_INTEGRAL		10

#define PK_OFFSET			20
#define PK_INTERVAL			100

#define TIMEBIN				8.0f							//ns

#define c16E6				16<<20
#define c16E4				16<<12
#define c16E2				16<<4

//#define _J_(x)			(((*((x)+1)<<2)+*(x))<<6)		//10 bits construction.
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
