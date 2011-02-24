#include <stdio.h>

void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix);
void SaveWave(char *namevector, unsigned char t_channels, signed char *buffer);
void SaveColumn (unsigned char t_channels);