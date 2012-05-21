// header files required by the application
//#include <stdlib.h>
//#include <stdio.h>
//#include <time.h>
//#include <iostream>
//using namespace std;	

#include "MPD.h"

class Core {

private:
	MPD *fmpd0;

	unsigned char lc_config;
	

protected:
	bool Run;

public:
	Core();
	virtual ~Core();
	
	int SetConnect(bool state);
	void Initialize();
	void SetChannels(unsigned char c_config);
	void SetTrigger(bool mode, bool slope, signed short th);	
	void SetRun(bool state);
	bool GetRun() {return Run;}
	void ToggleRun();
	unsigned int Acq(unsigned char *Buffer);
	unsigned char MapChannels(unsigned char config, unsigned char *channel);
	unsigned char WriteReg(unsigned char addr, unsigned char data);
	unsigned char ReadReg(unsigned char addr);
	void CheckClear(void);
	void Loopback(void);
	void TestVmeRW(void);
	void TestCoreRW(void);
	unsigned char WriteSSPI(unsigned char data);
	unsigned char WriteCore(unsigned char addr, unsigned char data);
	unsigned char ReadCore(unsigned char addr);


	//ClassDef(Core,0);
};
