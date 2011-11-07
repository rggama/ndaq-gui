#include <stdio.h>

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix);

signed int GetSample(signed char *buffer, signed char *addr);

void SaveWave(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer);
unsigned int SaveCal(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer);
unsigned int SavePTable(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer);
unsigned int SaveNTable(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer);

double GetBaseline(signed char *buffer, unsigned int w);
//double GetPAmplitude(signed char *buffer, unsigned int w, double *basetmp);
//double GetNAmplitude(signed char *buffer, unsigned int w, double *basetmp);

signed int GetPPeak(signed char *buffer, unsigned int addr, unsigned char start, unsigned char end);
signed int GetNPeak(signed char *buffer, unsigned int addr, unsigned char start, unsigned char end);
//signed char *GetPPeak(signed char *buffer, signed char *addr, unsigned int size);
//signed char *GetNPeak(signed char *buffer, signed char *addr, unsigned int size);

double GetPInt(signed char *buffer, double base, unsigned int addr, unsigned char start, unsigned char end);

//void SaveColumn(char *namevector, unsigned char t_channels, unsigned int block_size, signed char *buffer);
//signed char GetPHigher(signed char *buffer, unsigned int w);
//signed char GetNHigher(signed char *buffer, unsigned int w);
