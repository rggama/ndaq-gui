#include "s_wave.h"
#include "defines.h"
#include "a_graph.h"
#include "calibration.h"
#include "string.h"
#include <conio.h>

FILE	*file;

/**************************************************************************************************************************/

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix){
	unsigned char btst = 0x01;
	
	for(unsigned char i=0;i<MAX_CHANNELS;i++){
	
		//if(*channel++ != 0){
		if ((config & btst) == btst){

			sprintf(namevector, "%s_%s%u.txt", filename, suffix, i+1);
			//namevector+=13;
			//printf("Strlen: %u\n", strlen(namevector));
			namevector+=(strlen(namevector)+1);
		}
		btst = btst<<1;
	}
}

/**************************************************************************************************************************/

signed int GetSample(signed char *buffer, signed char *addr)
{
	signed int temp = 0;


	temp=_J_(buffer = addr);
	
	return temp>>6;
}

//*r is a pointer to a SIGNED SHORT.
void SaveRawData(unsigned short data, unsigned short offset, const void *r)
{
	signed short temp = 0;
	//const signed short *rptr = (const signed short *) r;

	temp = (signed short)(data<<6);
	
	//printf("sample: %u\n", offset);
	fprintf(file, "%d\t", temp>>6);
}

/**************************************************************************************************************************/

//cada linha é um evento ou waveform
//colunas = EVENT_SIZE
//linhas = quantidade de eventos ou waveforms
void SaveWave(unsigned char t_blocks, unsigned char config, char *namevector, unsigned short block_size, unsigned char *buffer){
	
	unsigned char btst = 0x01;
	//signed short temp[ADC_SIZE];
	
	//Block counter - Each block contains 2 ADC channels. But we'll have to check if both are enabled 
	//for saving.
	for(unsigned char c=0;c<t_blocks;c++)
	{
		//Even indexed ADC channel check.
		if ((config & btst) == btst)
		{
			file = fopen(namevector, "a+t");

			//Save ALL samples from even indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<(block_size/SLOT_SIZE); block_index+=(FIFO_BS*t_blocks))
			{
				//Even ADC indexes (0, 2, 4, 6 which corresponds to channels 1, 3, 5, 7) are on LSWords.
				//The function below will retrieve ADC_SIZE samples from the memory buffer. These samples 
				//will be written as a line on a ASCII file.
				GetLSWORD(ADC_OFFSET+block_index, ADC_SIZE, buffer, SaveRawData, NULL);
				fprintf(file, "\n");
			}

			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
		}
		btst = btst<<1;
		
		//Odd indexed ADC channel check.
		if ((config & btst) == btst)
		{
			file = fopen(namevector, "a+t");
			//Save ALL samples from the save enabled ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<(block_size/SLOT_SIZE); block_index+=(FIFO_BS*t_blocks))
			{
				//Odd ADC indexes (1, 3, 5, 7 which corresponds to channels 2, 4, 6, 8) are on MSWords.
				//The function below will retrieve ADC_SIZE samples from the memory buffer. These samples 
				//will be written as a line on a ASCII file.
				GetMSWORD(ADC_OFFSET+block_index, ADC_SIZE, buffer, SaveRawData, NULL);
				fprintf(file, "\n");
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
		}
		btst = btst<<1;

		//printf("block index %u - channel: %u\n", block_index, c);
		//_getch();
	}
}

unsigned int SaveCal(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer){
	
	unsigned int counter = 0;

	for(unsigned char c=0;c<t_channels;c++){
	

		file = fopen(namevector, "a+t");

		//line
		for(unsigned int i=(c*(EVENT_SIZE*_step_));i<block_size;i+=((EVENT_SIZE*_step_)*t_channels)){
			
			fprintf(file, "%d\n", GetSample(buffer, &buffer[i+CAL_SMP*_step_]));
			//printf("Raw: %d\r", GetSample(buffer, &buffer[i+CAL_SMP*_step_]));
			counter++;
		}
		fclose(file);
		namevector+=(strlen(namevector)+1);
	}
	
	return counter;
}

unsigned int SavePTable(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer){
	
	unsigned int counter = 0;
	double base = 0;
	double peak = 0;
	double intg = 0;
	
	for(unsigned char c=0;c<t_channels;c++){
	
		file = fopen(namevector, "a+t");
	
		//line
		for(unsigned int i=(c*(EVENT_SIZE*_step_));i<block_size;i+=((EVENT_SIZE*_step_)*t_channels)){
		
			peak = GetPPeak(buffer, i, PK_START, PK_END)*A+B;
			base = GetBaseline(buffer, i)*A+B;
			//intg = GetPInt(buffer, base, i, INT_START, INT_END)*A+B;

			fprintf(file, "%0.2f\t%0.2f\n", base, (peak - (base)));
			//printf("Base, Amp: %0.2f\t%0.2f\r", base, (peak - (base)));
			counter++;

		}
		fclose(file);
		namevector+=(strlen(namevector)+1);
	}
	
	return counter;
}

unsigned int SaveNTable(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer){
	
	unsigned int counter = 0;
	double base = 0;
	double peak = 0;
	double intg = 0;
	
	for(unsigned char c=0;c<t_channels;c++){
	
		file = fopen(namevector, "a+t");
	
		//line
		for(unsigned int i=(c*(EVENT_SIZE*_step_));i<block_size;i+=((EVENT_SIZE*_step_)*t_channels)){
		
			peak = GetNPeak(buffer, i, PK_START, PK_END)*A+B;
			base = GetBaseline(buffer, i)*A+B;
			//intg = GetPInt(buffer, base, i, INT_START, INT_END)*A+B;

			fprintf(file, "%0.2f\t%0.2f\n", base, (peak - (base)));
			//printf("Base, Amp: %0.2f\t%0.2f\r", base, (peak - (base)));
			counter++;

		}
		fclose(file);
		namevector+=(strlen(namevector)+1);
	}
	
	return counter;
}

/**************************************************************************************************************************/

double GetBaseline(signed char *buffer, unsigned int w)
{
	double acc=0;


	for(unsigned int i=0; i<(BASEINT*_step_); i+=_step_)		
		acc+=GetSample(buffer, &buffer[w+i+(BASEOFS*2)]);

	//printf("b: %0.5f\r", acc/BASEINT);
	return acc/BASEINT;
}

/**************************************************************************************************************************/

signed int GetPPeak(signed char *buffer, unsigned int addr, unsigned char start, unsigned char end)
{
	signed int p_peak = -32768; //Minimum for a signed int.
	signed int temp = 0;

	for(unsigned char i=0; i<((end*_step_)-(start*_step_)); i+=_step_){	
		
		temp = GetSample(buffer, &buffer[addr+(start*_step_)+i]);

		if (temp > p_peak)
			p_peak = temp;
	}

	//printf("p: %0.5d\r", p_peak);
	return p_peak;
}

signed int GetNPeak(signed char *buffer, unsigned int addr, unsigned char start, unsigned char end)
{
	signed int n_peak = 32767; //Maximum for a signed int.
	signed int temp = 0;

	for(unsigned char i=0; i<((end*_step_)-(start*_step_)); i+=_step_){	
		
		temp = GetSample(buffer, &buffer[addr+(start*_step_)+i]);

		if (temp < n_peak)
			n_peak = temp;
	}

	//printf("p: %0.5d\r", p_peak);
	return n_peak;
}

double GetPInt(signed char *buffer, double base, unsigned int addr, unsigned char start, unsigned char end)
{
	double acc = 0;


	for(unsigned char i=0; i<((end*_step_)-(start*_step_)); i+=_step_){	
		
		acc+= (GetSample(buffer, &buffer[addr+(start*_step_)+i]) - base);
	}

	printf("p: %0.7f\r", acc);

	return acc;
}


//signed char *GetNPeak(signed char *buffer, signed char *addr, unsigned int size)
//{
//	signed char n_peak = 127; //Maximum for a signed char.
//	signed char *n_peakptr = NULL;
//	
//	buffer = addr;
//
//	for(unsigned int i=0; i<size; i++)
//		
//		if (*buffer++ < n_peak){
//			n_peak = *(buffer-1);
//			n_peakptr = (buffer-1);
//		}
//
//	return n_peakptr;
//	
//}

/**************************************************************************************************************************/

/**************************************************************************************************************************/

//seems useless :(
//cada coluna é um evento
//colunas = quantidade de eventos ou waveforms
//linhas = EVENT_SIZE
//void SaveColumn(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer){
//
//	FILE *file;
//
//	printf("block_size: %u\n", block_size);
//
//	for(unsigned char c=0;c<t_channels;c++){
//	
//		//printf("\n--start %s\n\n", namevector);
//		file = fopen(namevector, "a+t");
//		
//		//line
//		for(unsigned int i=0;i<EVENT_SIZE;i++){
//			//column
//			for(unsigned int j=(c*EVENT_SIZE)+i;j<block_size;j+=(EVENT_SIZE*t_channels)){
//		
//				//Save those indexes, as a line. You've just saved all the first samples of all events(Waveforms)
//				//printf("%u\t", j);
//				fprintf(file, "%d\t", buffer[j]);
//			}
//			//printf("\n");
//			fprintf(file, "\n");
//		}
//		//printf("\n--end %s\n\n", namevector);
//		fclose(file);
//		namevector+=(strlen(namevector)+1);
//	}
//}
