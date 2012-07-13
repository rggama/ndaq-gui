#include "s_wave.h"
#include "defines.h"
#include "a_graph.h"
#include "calibration.h"
#include "string.h"
#include <conio.h>

FILE	*file;

/**************************************************************************************************************************/

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix)
{
	unsigned char btst = 0x01;
	
	for(unsigned char i=0;i<MAX_CHANNELS;i++){
	
		//if(*channel++ != 0){
		//if channel is enabled...
		if ((config & btst) == btst)
		{
			sprintf(namevector, "%s_%s%u.txt", filename, suffix, i+1);
			namevector+=(strlen(namevector)+1);
		}
		btst = btst<<1;
	}
}

/**************************************************************************************************************************/

//*r is a pointer to a UNSIGNED SHORT.
//*r will be incremented if a sample was sucessfully written. Otherwise, *r will be kept.
void SaveRawData(unsigned short data, unsigned short offset, const void *r)
{
	const unsigned short *rptr = (const unsigned short *) r;
	unsigned short *cptr = (unsigned short *) rptr;
	
	if (fprintf(file, "%d\t", _10B_TO_SIGNED_(data)) > 0) (*cptr)++;
}

//*r is a pointer to a SIGNED INT.
//*r is the Sumatorie.
void GetSum(unsigned short data, unsigned short offset, const void *r)
{
	const signed int *rptr = (const signed int *) r;
	signed int *vptr = (signed int *) rptr;

	(*vptr)+= _10B_TO_SIGNED_(data);
}

//*r is a pointer to a SIGNED SHORT.
//*r is the Negative Peak.
void GetNPeak(unsigned short data, unsigned short offset, const void *r)
{
	const signed short *rptr = (const signed short *) r;
	signed short *vptr = (signed short *) rptr;
	signed short sdata = 0;

	if ((sdata = _10B_TO_SIGNED_(data)) < *vptr) *vptr = sdata; 
}

//*r is a pointer to a SIGNED SHORT.
//*r is the Negative Peak.
void GetPPeak(unsigned short data, unsigned short offset, const void *r)
{
	const signed short *rptr = (const signed short *) r;
	signed short *vptr = (signed short *) rptr;
	signed short sdata = 0;

	if ((sdata = _10B_TO_SIGNED_(data)) > *vptr) *vptr = sdata; 
}

/**************************************************************************************************************************/

//Cada linha possui o numero de amostras em um evento ou waveform.
//Portanto, o numero de colunas é igual ao numero de amostras em uma waveform.
//O total de linhas, ou waveforms, dependerá do tamanho do bloco de dados recebido. 
unsigned short SaveWave(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer)
{
	unsigned short counter=0;
	unsigned short total=0;

	//Converting 'block_size' from byte to SLOT_SIZE.
	block_size = block_size/SLOT_SIZE;

	//Block counter - Each block contains 2 ADC channels. But we'll have to check if both are enabled 
	//for saving.
	for(unsigned char c=0;c<t_blocks;c++)
	{
		//Even indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Save ALL samples from even indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Even ADC indexes (0, 2, 4, 6 which corresponds to channels 1, 3, 5, 7) are on LSWords.
				//The function below will retrieve ADC_SIZE samples from the memory buffer. These samples 
				//will be written as a line on a ASCII file.
				GetLSWORD(ADC_OFFSET+block_index, ADC_SIZE, buffer, SaveRawData, &counter);
				fprintf(file, "\n");
			}

			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
	
			//'counter' is the written samples count. To get the written lines (waveforms) value,
			// we must divide 'counter' by ADC_SAMPLES (number of samples in a waveform).
			total += (counter/ADC_SAMPLES);
			counter = 0;
		}
		
		//Odd indexed ADC channel check.
		if (*chmap++ > 0)
		{	
			file = fopen(namevector, "a+t");
			//Save ALL samples from the save enabled ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Odd ADC indexes (1, 3, 5, 7 which corresponds to channels 2, 4, 6, 8) are on MSWords.
				//The function below will retrieve ADC_SIZE samples from the memory buffer. These samples 
				//will be written as a line on a ASCII file.
				GetMSWORD(ADC_OFFSET+block_index, ADC_SIZE, buffer, SaveRawData, &counter);
				fprintf(file, "\n");
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written samples count. To get the written lines (waveforms) value,
			// we must divide 'counter' by ADC_SAMPLES (number of samples in a waveform).
			total += (counter/ADC_SAMPLES);
			counter = 0;
		}
	}

	//returns the total written lines (waveforms).
	return total;
}

//Salva uma amostra de ADC, definida por CAL_SMP_OFFSET, contida em um bloco de dados.
unsigned short SaveCal(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer)
{
	unsigned short counter=0;
	unsigned short total=0;

	//Converting 'block_size' from byte to SLOT_SIZE.
	block_size = block_size/SLOT_SIZE;

	//Block counter - Each block contains 2 ADC channels. But we'll have to check if both are enabled 
	//for saving.
	for(unsigned char c=0;c<t_blocks;c++)
	{
		//Even indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Save ONE sample from even indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Even ADC indexes (0, 2, 4, 6 which corresponds to channels 1, 3, 5, 7) are on LSWords.
				//The function below will retrieve ONE sample from the memory buffer. These sample 
				//will be written as a line on a ASCII file.
				GetLSWORD(ADC_OFFSET+CAL_SMP_OFFSET+block_index, 1, buffer, SaveRawData, &counter);
				fprintf(file, "\n");
			}

			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
	
			//'counter' is the written samples count (should be 1 if everything went right). 
			total += counter;
			counter = 0;
		}
		
		//Odd indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");
			//Save ALL samples from the save enabled ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Odd ADC indexes (1, 3, 5, 7 which corresponds to channels 2, 4, 6, 8) are on MSWords.
				//The function below will retrieve ADC_SIZE samples from the memory buffer. These samples 
				//will be written as a line on a ASCII file.
				GetMSWORD(ADC_OFFSET+CAL_SMP_OFFSET+block_index, 1, buffer, SaveRawData, &counter);
				fprintf(file, "\n");
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written samples count (should be 1 if everything went right). 
			total += counter;
			counter = 0;
		}
	}

	//returns the total written samples.
	return total;
}

//
unsigned short SaveNTable(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer)
{
	unsigned short counter=0;
	unsigned short total=0;

	signed short peak = 32767; //Maximum for a signed short.
	signed int acc = 0;
	double baseline = 0;
	double amplitude = 0;

	//Converting 'block_size' from byte to SLOT_SIZE.
	block_size = block_size/SLOT_SIZE;

	//Block counter - Each block contains 2 ADC channels. But we'll have to check if both are enabled 
	//for saving.
	for(unsigned char c=0;c<t_blocks;c++)
	{
		//Even indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Calculating/Saving baseline/amplitude from even indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Even ADC indexes (0, 2, 4, 6 which corresponds to channels 1, 3, 5, 7) are on LSWords.
				
				//Peak:
				//1) We will find the waveform's NEGATIVE peak value starting at PK_OFFSET.
				GetLSWORD(ADC_OFFSET+PK_OFFSET+block_index, PK_INTERVAL, buffer, GetNPeak, &peak);
				//2) We have to consider ADC calibration for the final value.
				peak = peak*A+B;

				//Baseline:
				//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
				GetLSWORD(ADC_OFFSET+BASE_OFFSET+block_index, BASE_INTEGRAL, buffer, GetSum, &acc);
				//2) 'baseline' is the mean value of the above sumatorie.
				baseline = acc / BASE_INTEGRAL;
				//3) We have to consider ADC calibration for the final value.
				baseline = baseline*A+B;

				//Amplitude: It's the Peak - Baseline.
				amplitude = peak - (baseline);
				if (fprintf(file, "%0.2f\t%0.2f\n", baseline, amplitude) > 0) counter++;

				//reseting vars.
				peak = 32767;
				acc = 0;
			}

			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
	
			//'counter' is the written baseline/amplitude pair count.
			total += counter;
			counter = 0;
		}
		
		//Odd indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Calculating/Saving baseline/amplitude from odd indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Odd ADC indexes (1, 3, 5, 7 which corresponds to channels 2, 4, 6, 8) are on MSWords.

				//Peak:
				//1) We will find the waveform's NEGATIVE peak value starting at PK_OFFSET.
				GetMSWORD(ADC_OFFSET+PK_OFFSET+block_index, PK_INTERVAL, buffer, GetNPeak, &peak);
				//2) We have to consider ADC calibration for the final value.
				peak = peak*A+B;

				//Baseline:
				//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
				GetMSWORD(ADC_OFFSET+BASE_OFFSET+block_index, BASE_INTEGRAL, buffer, GetSum, &acc);
				//2) 'baseline' is the mean value of the above sumatorie.
				baseline = acc / BASE_INTEGRAL;
				//3) We have to consider ADC calibration for the final value.
				baseline = baseline*A+B;

				//Amplitude: It's the Peak - Baseline.
				amplitude = peak - (baseline);
				if (fprintf(file, "%0.2f\t%0.2f\n", baseline, amplitude) > 0) counter++;

				//reseting vars.
				peak = 32767;
				acc = 0;
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written baseline/amplitude pair count.
			total += counter;
			counter = 0;
		}
	}

	//returns the total written baseline/amplitude pair count (lines).
	return total;
}

//
unsigned short SavePTable(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer)
{
	unsigned short counter=0;
	unsigned short total=0;

	signed short peak = -32768; //Minimum for a signed short.
	signed int acc = 0;
	double baseline = 0;
	double amplitude = 0;

	//Converting 'block_size' from byte to SLOT_SIZE.
	block_size = block_size/SLOT_SIZE;

	//Block counter - Each block contains 2 ADC channels. But we'll have to check if both are enabled 
	//for saving.
	for(unsigned char c=0;c<t_blocks;c++)
	{
		//Even indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Calculating/Saving baseline/amplitude from even indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Even ADC indexes (0, 2, 4, 6 which corresponds to channels 1, 3, 5, 7) are on LSWords.
				
				//Peak:
				//1) We will find the waveform's NEGATIVE peak value starting at PK_OFFSET.
				GetLSWORD(ADC_OFFSET+PK_OFFSET+block_index, PK_INTERVAL, buffer, GetPPeak, &peak);
				//2) We have to consider ADC calibration for the final value.
				peak = peak*A+B;

				//Baseline:
				//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
				GetLSWORD(ADC_OFFSET+BASE_OFFSET+block_index, BASE_INTEGRAL, buffer, GetSum, &acc);
				//2) 'baseline' is the mean value of the above sumatorie.
				baseline = acc / BASE_INTEGRAL;
				//3) We have to consider ADC calibration for the final value.
				baseline = baseline*A+B;

				//Amplitude: It's the Peak - Baseline.
				amplitude = peak - (baseline);
				if (fprintf(file, "%0.2f\t%0.2f\n", baseline, amplitude) > 0) counter++;

				//reseting vars.
				peak = -32768;
				acc = 0;
			}

			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);
	
			//'counter' is the written baseline/amplitude pair count.
			total += counter;
			counter = 0;
		}
		
		//Odd indexed ADC channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Calculating/Saving baseline/amplitude from odd indexed ADC channels (contained into a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				//Odd ADC indexes (1, 3, 5, 7 which corresponds to channels 2, 4, 6, 8) are on MSWords.

				//Peak:
				//1) We will find the waveform's NEGATIVE peak value starting at PK_OFFSET.
				GetMSWORD(ADC_OFFSET+PK_OFFSET+block_index, PK_INTERVAL, buffer, GetPPeak, &peak);
				//2) We have to consider ADC calibration for the final value.
				peak = peak*A+B;

				//Baseline:
				//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
				GetMSWORD(ADC_OFFSET+BASE_OFFSET+block_index, BASE_INTEGRAL, buffer, GetSum, &acc);
				//2) 'baseline' is the mean value of the above sumatorie.
				baseline = acc / BASE_INTEGRAL;
				//3) We have to consider ADC calibration for the final value.
				baseline = baseline*A+B;

				//Amplitude: It's the Peak - Baseline.
				amplitude = peak - (baseline);
				if (fprintf(file, "%0.2f\t%0.2f\n", baseline, amplitude) > 0) counter++;

				//reseting vars.
				peak = -32768;
				acc = 0;
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written baseline/amplitude pair count.
			total += counter;
			counter = 0;
		}
	}

	//returns the total written baseline/amplitude pair count (lines).
	return total;
}

/**************************************************************************************************************************/

unsigned short SaveCounter(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer)
{
	unsigned short counter=0;
	unsigned short total=0;
	
	unsigned int timestamp=0;
	unsigned int cntr=0;

	//Converting 'block_size' from byte to SLOT_SIZE.
	block_size = block_size/SLOT_SIZE;

	//Block counter - Each block contains 2 Counter channels. But we'll have to check if both are enabled 
	//for saving.

	for (unsigned char c=0;c<t_blocks;c++)
	{
		//Even Counter channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Getting timestamp/counter from a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				GetDWORD(TIMESTAMP_OFFSET+block_index, TIMESTAMP_SIZE, buffer, CopyData, &timestamp);
				GetDWORD(ACOUNTER_OFFSET+block_index, COUNTER_SIZE, buffer, CopyData, &cntr);
				if (fprintf(file, "%08u\t%08u\n", timestamp, cntr) > 0) counter++;
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written timestamp/counter count.
			total += counter;
			counter = 0;
		}

		//Odd Counter channel check.
		if (*chmap++ > 0)
		{
			file = fopen(namevector, "a+t");

			//Getting timestamp/counter from a single FIFO block)...
			for(unsigned short block_index=(c*FIFO_BS); block_index<block_size; block_index+=(FIFO_BS*t_blocks))
			{
				GetDWORD(TIMESTAMP_OFFSET+block_index, TIMESTAMP_SIZE, buffer, CopyData, &timestamp);
				GetDWORD(BCOUNTER_OFFSET+block_index, COUNTER_SIZE, buffer, CopyData, &cntr);
				if (fprintf(file, "%08u\t%08u\n", timestamp, cntr) > 0) counter++;
			}
			fclose(file);
			//next filename.
			namevector+=(strlen(namevector)+1);

			//'counter' is the written timestamp/counter count.
			total += counter;
			counter = 0;
		}
	}

	//returns the total written timestamp/counter count (lines).
	return total;
}

/**************************************************************************************************************************/
