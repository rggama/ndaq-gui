#include <stdio.h>

//
void SetFilename(unsigned char config, char *namevector, char *filename, char *suffix);

//*r is a pointer to a UNSIGNED SHORT.
//*r will be incremented if a sample was sucessfully written. Otherwise, *r will be kept.
void SaveRawData(unsigned short data, unsigned short offset, const void *r);
//*r is a pointer to a SIGNED INT.
//*r is the Sumatorie.
void GetSum(unsigned short data, unsigned short offset, const void *r);
//*r is a pointer to a SIGNED SHORT.
//*r is the Negative Peak.
void GetNPeak(unsigned short data, unsigned short offset, const void *r);
//*r is a pointer to a SIGNED SHORT.
//*r is the Negative Peak.
void GetPPeak(unsigned short data, unsigned short offset, const void *r);

//
unsigned short SaveWave(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer);
//
unsigned short SaveCal(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer);
//
unsigned short SaveNTable(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer);
//
unsigned short SavePTable(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer);

//
unsigned short SaveCounter(unsigned char t_blocks, unsigned char *chmap, char *namevector, unsigned short block_size, unsigned char *buffer);
