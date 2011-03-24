#include <stdio.h>

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix);
void SaveWave(char *namevector, unsigned char t_channels, signed char *buffer);
void SaveCal(char *namevector, unsigned char t_channels, signed char *buffer);
void SaveTDC(char *namevector, unsigned int block_size, unsigned char *buffer);
signed char GetPHigher(signed char *buffer, unsigned int w);
signed char GetNHigher(signed char *buffer, unsigned int w);
//signed char GetLower(signed char *buffer);
void SaveColumn (unsigned char t_channels);