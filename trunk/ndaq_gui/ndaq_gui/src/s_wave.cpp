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
	/*
	//2
	for(unsigned int i=(1*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//3
	for(unsigned int i=(2*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//4
	for(unsigned int i=(3*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//5
	for(unsigned int i=(4*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//6
	for(unsigned int i=(5*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//7
	for(unsigned int i=(6*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	//8
	for(unsigned int i=(7*EVENT_SIZE);i<BLOCK_SIZE;i+=(EVENT_SIZE*t_channels)){
		
		//memcpy
		printf("Start: %u\n", i);

	}
	*/
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