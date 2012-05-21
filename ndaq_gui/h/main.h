// header files required by the application.
#include <TROOT.h>
#include <TApplication.h>
#include <TThread.h>

#include "f_MainFrame.h"

class Thread {

private:
	TThread *mThread1;
	MainFrame *MainF;

protected:
	bool mThreadrun1;

public:
	Thread();
	virtual ~Thread();

	virtual void Func1();
	static void Thread1(void* arg);
	Int_t Threadstart();
	Int_t Threadstop();
	Bool_t GetThreadRun1() 
    	{return mThreadrun1;}

	//ClassDef(Thread,0);
};
