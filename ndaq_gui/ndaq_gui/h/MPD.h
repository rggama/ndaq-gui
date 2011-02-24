#ifdef __CINT__
class pthread_cond_t;
class pthread_mutex_t;
#endif

#include <windows.h>
//#include "TObjString.h"
//#include "TList.h"
//#include "WinTypes.h"
//#include <TSystem.h>

class MPD
{
private:
	bool connected;
	int numDev;
public:
	MPD(int num);
	virtual ~MPD();
	int connect(bool con);
	int SetLatency(unsigned char latency);
	int SetBlock(unsigned int block);
	int SetFlowControl(void);
	int Write(int addr, unsigned char data);
	int Read(unsigned char *data,DWORD &BytesR, DWORD size);
	void reset();
	bool isConnected();
	int GetNumDev();
	int GetSize();
	int clearBufferRX();
};
