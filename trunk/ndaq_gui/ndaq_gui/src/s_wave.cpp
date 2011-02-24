#include "s_wave.h"
#include "defines.h"
#include "string.h"
//for (/* TRIGGERS - EVENTS */)

//for (/* BUFFER */)
//fprintf(


//let's code ;D

//passo = EVENT_SIZE * TOTAL_CHANNELS
//start = (mapchannels-1) * EVENT_SIZE

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

void SaveWave(char *namevector, unsigned char t_channels, signed char *buffer){
	
	FILE *file;
	
	//if (t_channels == 0) return;

	for(unsigned char c=0;c<t_channels;c++){
	
		//printf("\n--start %s\n\n", namevector);
		file = fopen(namevector, "a+t");

	//if ((config & btst) == btst)
		//line
		for(unsigned int i=(c*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
			//column
			for(unsigned int j=i;j<(EVENT_SIZE+i);j++){
			
				//Save all columns for that line
				//printf("%u\t", j);
				fprintf(file, "%d\t", buffer[j]);
			}
			//printf("\n");
			fprintf(file, "\n");
		}
		//printf("\n--end %s\n\n", namevector);
		fclose(file);
		namevector+=(strlen(namevector)+1);
	}
}

void SaveCal(char *namevector, unsigned char t_channels, signed char *buffer){
	
	FILE *file;
	
	//if (t_channels == 0) return;

	for(unsigned char c=0;c<t_channels;c++){
	
		//printf("\n--start %s\n\n", namevector);
		//file = fopen(namevector, "a+t");

	//if ((config & btst) == btst)
		//line
		for(unsigned int i=(c*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
			//column
			//for(unsigned int j=i;j<(EVENT_SIZE+i);j++){
			
				//Save all columns for that line
				//printf("%u\t", j);
				//fprintf(file, "%d\t", GetHigher(buffer, i));
				GetNHigher(buffer, i);

			//}
			//printf("\n");
			//fprintf(file, "\n");
		}
		//printf("\n--end %s\n\n", namevector);
		//fclose(file);
		namevector+=(strlen(namevector)+1);
	}
}

signed char *GetPPeak(signed char *buffer, signed char *addr, unsigned int size)
{
	signed char p_peak = -128; //Minimum for a signed char.
	signed char *p_peakptr = NULL;
	
	
	buffer = addr;

	for(unsigned int i=0; i<size; i++)	
		
		if (*buffer++ > p_peak){
			p_peak = *(buffer-1);
			p_peakptr = (buffer-1);
		}

	return p_peakptr;
}

signed char *GetNPeak(signed char *buffer, signed char *addr, unsigned int size)
{
	signed char n_peak = 127; //Maximum for a signed char.
	signed char *n_peakptr = NULL;
	
	
	buffer = addr;

	for(unsigned int i=0; i<size; i++)
		
		if (*buffer++ < n_peak){
			n_peak = *(buffer-1);
			n_peakptr = (buffer-1);
		}

	return n_peakptr;
	
}

signed char GetPHigher(signed char *buffer, unsigned int w){
	
	signed char p_peak = 0;
	signed char n_peak = 0;
	signed char f = 0;
	unsigned int filter_s = 3;
	signed int acc = 0;

	signed char *startptr = &buffer[w];
	signed char *ppeakptr = NULL;

	if ((ppeakptr = GetPPeak(buffer, startptr, EVENT_SIZE)) != NULL)
		p_peak = *(buffer = ppeakptr);
	else
		return 0;
	//p_peak = *(buffer = ppeakptr = GetPPeak(buffer, startptr, EVENT_SIZE));

	if ((buffer = GetNPeak(buffer, buffer, buffer-startptr)) != NULL)
		n_peak = *buffer;
	else
		return 0;
	//n_peak = *(buffer = GetNPeak(buffer, buffer, buffer-startptr));

	//pulse is between 'ppeak' pointer and 'npeak' pointer whose is already stored at 'buffer' pointer.
	
	//printf("delta: %d\n", buffer-ppeakptr);
	//for (unsigned int i=0; i<(buffer-ppeakptr); i++)
	//	printf("count: %u", i);

	/*
	** Integrates sample[n+i] for filter_s iterations then
	** divides the result by the numer of iterations and
	** compares it to sample[n+i]. If yes, result is sample[n].
	** Otherwise, result is zero. Where 'i' starts at the positive
	** voltape peak plus one. Repeats the cycle for sample[n+i] 
	** until 'i' reaches the negative voltage peak index minus 
	** filter size. (filter_s).
	**
	*/
	signed int size = (buffer-ppeakptr)-filter_s;

	buffer=ppeakptr+1; //let's start at the pointer right after 'ppeak' pointer.
	
	printf("size: %d\n", size);

	
	if (size > 0) {	//do not let a negative size into for loop.

		for(unsigned int i=0; i<size; i++){
		
			acc = 0;

			for(unsigned int j=0; j<filter_s; j++)
				acc+=*buffer++;
		
			buffer-=filter_s;
		
			//printf("Idx: %d - AdF: %d - Buf: %d\n", i, (acc/filter_s), *buffer);

			if ((acc/filter_s) == *buffer++)
				f = *(buffer-1);
		}
	}
	
	printf("PPk: %d\n", p_peak);
	printf("NPk: %d\n", n_peak);
	if (f != 0)
		printf("Max: %d\n", f);
	
	return f;
}

signed char GetNHigher(signed char *buffer, unsigned int w){
	
	signed char p_peak = 0;
	signed char n_peak = 0;
	signed char f = 0;
	unsigned int filter_s = 3;
	signed int acc = 0;

	signed char *startptr = &buffer[w];
	signed char *npeakptr = NULL;

	if ((npeakptr = GetNPeak(buffer, startptr, EVENT_SIZE)) != NULL)
		n_peak = *(buffer = npeakptr);
	else
		return 0;

	if ((buffer = GetPPeak(buffer, buffer, buffer-startptr)) != NULL)
		p_peak = *buffer;
	else
		return 0;

	//pulse is between 'npeak' pointer and 'ppeak' pointer whose is already stored at 'buffer' pointer.
	
	//printf("delta: %d\n", buffer-ppeakptr);
	//for (unsigned int i=0; i<(buffer-ppeakptr); i++)
	//	printf("count: %u", i);

	/*
	** Integrates sample[n+i] for filter_s iterations then
	** divides the result by the numer of iterations and
	** compares it to sample[n+i]. If yes, result is sample[n].
	** Otherwise, result is zero. Where 'i' starts at the positive
	** voltape peak plus one. Repeats the cycle for sample[n+i] 
	** until 'i' reaches the negative voltage peak index minus 
	** filter size. (filter_s).
	**
	*/
	signed int size = (buffer-npeakptr)-filter_s;

	buffer=npeakptr+1; //let's start at the pointer right after 'npeak' pointer.
	
	printf("size: %d\n", size);

	
	if (size > 0) {	//do not let a negative size into for loop.

		for(unsigned int i=0; i<size; i++){
		
			acc = 0;

			for(unsigned int j=0; j<filter_s; j++)
				acc-=*buffer++;
					
			buffer-=filter_s;
		
			//printf("Idx: %d - AdF: %d - Buf: %d\n", i, (acc/filter_s), *buffer);

			if ((acc/filter_s) == -(*buffer++))
				f = *(buffer-1);
		}
	}
	
	printf("PPk: %d\n", p_peak);
	printf("NPk: %d\n", n_peak);
	if (f != 0)
		printf("Max: %d\n", f);
	
	return f;
}

//seems useless :(
void SaveColumn (unsigned char t_channels){

	//line
	for(unsigned int i=0;i<EVENT_SIZE;i++){
		//column
		for(unsigned int j=(0*EVENT_SIZE)+i;j<BLOCK_SIZE;j+=(EVENT_SIZE*t_channels)){
		
			//Save those indexes, as a line. You've just saved all the first samples of all events(Waveforms)
			printf("%u\t", j);
		}
		printf("\n");
	}
}

/*
	unsigned int eq = 0;
	unsigned int gt = 0;
	unsigned int lt = 0;

	for(unsigned int i=0; i<EVENT_SIZE; i++)
	
		if(*++buffer == *(buffer-1))
			eq++;
		else if (*buffer > *(buffer-1))
			gt++;
		else if (*buffer < *(buffer-1))
			lt++;

	printf("eq: %d\n", eq);
	printf("gt: %d\n", gt);
	printf("lt: %d\n", lt);
*/

	/*
	** Integrates everything then divide by iterations.
	** Noise will be clearly noticed on result.
	**

	filter_s = 0;
	for(unsigned int i=(riseidx+1); i<fallidx; i++){
		acc+=*buffer++;
		filter_s++;
		printf("Idx: %d - Acc: %d - Buf: %d\n", i, acc, *buffer);

	}

	f = (acc/filter_s);
	printf("AdF: %d\n", f);
	*/
