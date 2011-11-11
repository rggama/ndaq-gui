//=============================================================================
// File Name   : main.cpp
// Pseudo Name : NDAQ Event Catcher (NDEC)
// Author      : Rafael Gama
// Version     : 0.3b
// Copyright   : IF YOU COPY THIS, YOU'RE A HUGE VAGABUNDO!
// Description : Not today...
//=============================================================================
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
//#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define	_CRT_SECURE_NO_WARNINGS 1

#include "main.h"
//#include <conio.h>	//for getch() in a test procedure below.

//bool ThreadON1 = false;

Thread::Thread()
{
	MainF = new MainFrame(gClient->GetRoot(), 880, 380);
	//ThreadON1 = false;
	Threadstart();
}

Thread::~Thread()
{
	Threadstop();
}

void Thread::Func1()
{
	//printf("Is It!?\n");
	if(MainF->Update()){} else gSystem->Sleep(1);
}

void Thread::Thread1(void* arg)
{
  // thread function which calls user specified action Func0
  TThread::SetCancelOn();
  TThread::SetCancelDeferred();
  Thread* inst1 = (Thread*) arg;
  //MainFrame* mf = MainFrame();
  Int_t meid=TThread::SelfId(); // get pthread id
	 
  while(inst1->GetThreadRun1()){
    for(;;)
	{
       TThread::CancelPoint();
	   inst1->Func1();
	}
  }
}

//
//----- Thread Functions for Start and Stop
//
Int_t Thread::Threadstart()
{
  // start threads

  // Initialize thread1
  //if(mThread1) delete mThread1;
  //if (mThread1) TThread::Delete(mThread1); /*** INVESTIGAR ! ! ! ***/
  //if(!mThread1)
    //{
      mThreadrun1 = true;
      mThread1 = new TThread("mythread 1",(void(*)(void *))&Thread1,(void*) this);
      mThread1->Run();
    //}

  return 0;
}

Int_t Thread::Threadstop()
{
  // stop thread
  mThreadrun1=false;	// aborting flag
  gSystem->Sleep(10);   // wait a while for thread to halt
  TThread::Delete(mThread1);
  delete mThread1;
  mThread1=0;

  return 0;
}

int main(int argc, char **argv) {
	
	
	TApplication theApp("App", &argc, argv);

	if (gROOT->IsBatch()) {
		fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
		return 1;
	}

	new Thread();

	theApp.Run();

	/*
	signed char data[128];
	signed char *dataptr = &data[0];
	signed char *aptr;
	signed char *bptr;

	aptr = dataptr;

	for (unsigned char i=0; i<128; i++)
		*dataptr++ = i;
	
	bptr = dataptr;

	printf("delta: %d\n", bptr-aptr);

	//dataptr = &data[0];
	//dataptr-=128;
	dataptr-=(bptr-aptr);

	for (unsigned char i=0; i<128; i++)
		printf("data: %d\n", *dataptr++);

	getch();
	*/

	return 0;
}
