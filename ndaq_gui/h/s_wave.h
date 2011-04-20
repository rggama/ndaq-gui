#include <stdio.h>

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix);

void SaveWave(char *namevector, unsigned char t_channels, signed char *buffer);
unsigned int SaveCal(char *namevector, unsigned char t_channels, signed char *buffer);
unsigned int SaveTable(char *namevector, unsigned char t_channels, signed char *buffer);

double GetBaseline(signed char *buffer, unsigned int w);
double GetPAmplitude(signed char *buffer, unsigned int w);
double GetNAmplitude(signed char *buffer, unsigned int w);

signed char *GetPPeak(signed char *buffer, signed char *addr, unsigned int size);
signed char *GetNPeak(signed char *buffer, signed char *addr, unsigned int size);

signed char GetPHigher(signed char *buffer, unsigned int w);
signed char GetNHigher(signed char *buffer, unsigned int w);

void SaveColumn (unsigned char t_channels);