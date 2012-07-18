// header files required by the application
//#include <stdlib.h>
//#include <stdio.h>
//#include <time.h>
//#include <iostream>
//using namespace std;	

#include "MPD.h"
#include "defines.h"

struct NDAQ_CONFIG
{
	unsigned char ac;
	unsigned char bc1;
	unsigned char bc2;
	unsigned char bc3;
	unsigned char bc4;
	unsigned char vs;
	unsigned char vc;
};


class Core {

private:
	MPD *fmpd0;

	unsigned char lc_config;
	unsigned char acq_config;
	unsigned char t_blocks;
	unsigned char t_channels;
	unsigned short block_size;
	unsigned char chmap_vector[MAX_CHANNELS];
	unsigned char chadd_vector[MAX_CHANNELS];

protected:
	bool Run;

public:

	Core();
	virtual ~Core();
	
	int SetConnect(bool state);
	void Initialize();
	void SetChannels(unsigned char c_config);
	void SetTrigger(bool mode, bool slope, signed short th, unsigned char isel);	
	void MakeConfig(unsigned char cword, NDAQ_CONFIG *config);
	void Config(NDAQ_CONFIG config);
	void SetRun(bool state);
	bool GetRun() {return Run;}
	unsigned char GetTBlocks() {return t_blocks;}
	unsigned char GetTChannels() {return t_channels;}
	unsigned short GetBS() {return block_size;}
	void ToggleRun();
	unsigned int Acq(unsigned char *Buffer);
	unsigned char MapChannels(unsigned char config);
	unsigned char *GetChMap() {return &chmap_vector[0];}
	unsigned char WriteReg_T(unsigned char addr, unsigned char data, unsigned char id);
	unsigned char WriteReg(unsigned char addr, unsigned char data, unsigned char id);
	unsigned char ReadReg(unsigned char addr, unsigned char id);
	void CheckClear(void);
	void Loopback(void);
	void TestVmeRW(void);
	void TestCoreRW(void);
	unsigned char WriteSSPI(unsigned char data, unsigned char id);
	unsigned char WriteCore(unsigned char addr, unsigned char data, unsigned char id);
	unsigned char ReadCore(unsigned char addr, unsigned char id);
	void Test();

	//ClassDef(Core,0);
};
