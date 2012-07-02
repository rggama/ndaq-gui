//=============================================================================
// File Name   : c_Core.cpp
// Pseudo Name : NDAQ Event Catcher (NDEC)
// Author      : Rafael Gama
// Version     : svn controlled.
// Copyright   : IF YOU COPY THIS, YOU'RE A HUGE VAGABUNDO!
// Description : Not today...
//=============================================================================
#include "defines.h"
#include <stdio.h>

#ifndef LINUX
	#include <conio.h>
#else
	#include <unistd.h>
	#include <iostream>
	#define _getch()	std::cin.get();
	#define	Sleep(x)	usleep((x*1000));
#endif

#include "c_Core.h"

#define C(n, id)			{printf("WriteCore Error at 0x%02X!\n", n); FILEDEBUG("WriteCore Error - 0x%02X - id: %u!\n", n, id);}
#define E(n, id)			{printf("WriteReg Error at 0x%02X!\n", n); FILEDEBUG("WriteReg Error - 0x%02X - id: %u!\n", n, id);} 
#define R					if (DEBUG) printf("Response 0x%02X\n", r);

#define TestWCore(a, d, id)	{if(!WriteCore(a, d, id)) {C(a, id)}}
#define TestWReg(a, d, id)	{if(!WriteReg_T(a, d, id)) {E(a, id)}}

Core::Core()
{
	//Initialize MPD
	fmpd0 = new MPD(0); //Instanciando o objeto fmpd0 da classe MPD. Ou seja, tornar essa bendenga REAL ! 
	Run = false;
	lc_config = 0;
	acq_config = 0;
}

Core::~Core()
{
	delete fmpd0;
}

int Core::SetConnect(bool state)
{
	if(fmpd0->isConnected())
		return -1;
	else{
	  	fmpd0->connect(true);
		
		if(fmpd0->isConnected()){
			return 1;
		}else{
			return 0;
		}	
	}
}

void Core::Initialize()
{
	fmpd0->SetFlowControl();

	fmpd0->SetBlock(65536);
	//fmpd0->SetBlock(64);

	fmpd0->SetLatency(1);

	CheckClear();
	WriteReg(0xAA, 0x55, 20);		//Resets Vme FPGA
	WriteReg(0x80, 0x80, 21);		//Grant we'll have command responses.
	WriteCore(0xAA,0x55, 22);		//Resets Core FPGA
}

void Core::SetChannels(unsigned char c_config)
{
	lc_config = c_config;
}

void Core::SetTrigger(bool mode, bool slope, signed short th)
{
	unsigned char temp=0;

	//if slope is falling...
	if (slope == false)
	{
		TestWCore(0x7B, 0x01, 30);				//Internal Trigger Slope: 0 -> rising, 1 -> falling	
	}
	//if slope is rising...	
	else
		TestWCore(0x7B, 0x00, 31);
	
	//if slope is falling...
	if (slope == false)
	{	
		printf("\nLow byte: 0x%02X - High byte: 0x%02X\n", (unsigned char) th, ((th >> 8) & 0x03));
		TestWCore(0x79, (unsigned char) th, 32);		//Th2: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x7A, temp, 33);						//Th2: high byte
		//Hysteresis        
		th = th+1;
		TestWCore(0x77, (unsigned char) th, 34);		//Th1: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x78, temp, 35);						//Th1: high byte
	}
	//if slope is rising...
	else
	{
		printf("\nLow byte: 0x%02X - High byte: 0x%02X\n", (unsigned char) th, ((th >> 8) & 0x03));
		TestWCore(0x79, (unsigned char) th, 36);		//Th2: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x7A, temp, 37);						//Th2: high byte
		//Hysteresis        
		th = th-1;
		TestWCore(0x77, (unsigned char) th, 38);		//Th1: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x78, temp, 39);						//Th1: high byte
	}
	
	//if trigger is internal...
	if (mode == false)
	{
		acq_config = 0x80;
		TestWCore(0x91, acq_config, 40);						//ACQ Register - Bit 7: 0 -> external trigger, 1 -> internal trigger
	}
	//if trigger is external...	
	else
	{
		acq_config = 0x00;
		TestWCore(0x91, acq_config, 41);
	}
}	

void Core::SetRun(bool state)
{
	unsigned char b_btst = 0x01;
	unsigned char n_btst = 0x01;
	unsigned char adc_pwdn = 0x0F;

	
	
	//Start
	if(state){
		WriteReg(0x80, 0x80, 0);					//Grant we'll have command responses.

		//TestWCore(0x87, 0x00);					//ADC Power-up - Already Powered.

		TestWReg(0x81, 0x00, 1);					//Vme DataBuilder config - Disable ALL Slots.

		TestWCore(0x80, 0x00, 18);					//Core DataBuilder config - Disable ALL Slots.

		TestWCore(0x91, (acq_config | 0x00), 2);	//Disable all ACQ enables.

		TestWCore(0x81, 0x7F, 3);					//127(+1) samples per trigger (config).
		
		TestWCore(0x89, 0x01, 4);					//ACQ Reset Assert.
		Sleep(32);
		TestWCore(0x89, 0x00, 5);					//ACQ Reset Deassert.
		Sleep(32);

		TestWCore(0x91, (acq_config | 0x1F), 6);	//Enables desired ACQ enables.

		TestWCore(0x80, 0x6E, 7);					//Core DataBuilder Configuration.
		Sleep(32);

		TestWCore(0x80, 0x6F, 8);					//DataBuilder Enable.

		/********************/

		TestWReg(0x83, 0x83, 9);					//Vme DataBuilder Block Size in WORDS less one (config).

		WriteReg(0x80, 0x00, 10);					//From here we won't have command responses anymore.
		
		CheckClear();								//Ensure Receive Buffer is clear.

		WriteReg(0x82, 0x01, 11);					//Readout Reset Assert.
		Sleep(32);
		WriteReg(0x82, 0x00, 12);					//Readout Reset Deassert.


		WriteReg(0x81, 0x0F, 13);					//Vme DataBuilder config.
		Sleep(32);
		WriteReg(0x80, 0x01, 14);					//Vme Readout Enable (DataBuilder enable).

		Run = true;

	//Stop
	}else{
		WriteReg(0x80, 0x80, 15);					//Return grant to command responses.
		Sleep(360);
		CheckClear();								//Ensure Receive Buffer is clear.
		
		TestWCore(0x91, 0x00, 16);					//Disable all ACQ enables.

		TestWCore(0x80, 0x00, 19);					//Disable all Core DataBuilder enables.
		
		//TestWCore(0x87, 0x00);					//ADC Power-down.
				
		TestWReg(0x81, 0x00, 17);					//Vme DataBuilder config - Disable ALL Slots.
		//TestWReg(0x80, 0x80);						//Vme Readout Disable (DataBuilder disable) - Already issued!
		
		Run = false;								//If it really stopped.		
	}	
}

void Core::Test()
{
	//
}

void Core::ToggleRun()
{
	//Do not change 'Run'. Let 'SetRun' do it based on hardware check.
	if (Run)
		SetRun(false);
		//Run = false;
	else
		SetRun(true);
		//Run = true;
}

//READ ON DEMAND :D
unsigned int Core::Acq(unsigned char *Buffer)
{
	DWORD BytesRead = 0;
	DWORD Size = 0;
	DWORD ReadSize = 0;

	Size=fmpd0->GetSize();
	
	if (DEBUG) printf(" - Buffer Size: %u\n", Size);

	if(Size > BLOCK_SIZE-1)
	{
		ReadSize = Size/BLOCK_SIZE;
		ReadSize = ReadSize*BLOCK_SIZE;

		fmpd0->Read(Buffer, BytesRead, ReadSize);

		return (unsigned int)BytesRead;
	}

	return 0;
}

void Core::CheckClear(void)
{
	DWORD		Size = 0;
	unsigned char	t=0;

	fmpd0->clearBufferRX();
	Size=fmpd0->GetSize();
	
	//MUST really flush the read buffer;
	while((Size > 0) && (t < 500))	//~500ms of timeout;
	{
		fmpd0->clearBufferRX();	
		Size=fmpd0->GetSize();
		t++;
		Sleep(1);
	}
	
	if (Size > 0) FILEDEBUG("Check Clear Error - %u!\n", Size, 0);

	printf("First Size: %u\n", Size);
}

//Write to Command Register and TEST response.
unsigned char Core::WriteReg_T(unsigned char addr, unsigned char data, unsigned char id)
{
	unsigned char	temp = 0;
	DWORD			BytesRead = 0;
	DWORD			Size = 0;
	unsigned char	Buffer[1];
	unsigned char	t=0;
	
	fmpd0->clearBufferRX();

	fmpd0->WriteB(0xAA);			//write cmd
	temp = (addr & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//addr ph1 - ls nibble
	temp = (addr >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//addr ph2 - ms nibble
	temp = (data & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//data ph1 - ls nibble
	temp = (data >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//data ph2 - ms nibble
	
	
	while((Size < 1) && (t < 500))	//~500ms of timeout;
	{
		Size=fmpd0->GetSize();
		t++;
		Sleep(1);
	}
	
	if(Size > 1) FILEDEBUG("WriteReg Response Error - 0x%02X - id: %u!\n", addr, id);

	if (Size == 1) //Size > 1 is not a command reponse. Maybe ACQ'ed data?
	{
			fmpd0->Read(Buffer, BytesRead, 1);
			
			if (Buffer[0] == 0xEB)
				return 1;
			else
				return 0;
	}

	return 0;
}

//Just Write to Command Register.
unsigned char Core::WriteReg(unsigned char addr, unsigned char data, unsigned char id)
{
	unsigned char	temp = 0;
	DWORD			BytesRead = 0;
	DWORD			Size = 0;
	unsigned char	t=0;
	
	fmpd0->clearBufferRX();

	fmpd0->WriteB(0xAA);			//write cmd
	temp = (addr & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//addr ph1 - ls nibble
	temp = (addr >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//addr ph2 - ms nibble
	temp = (data & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//data ph1 - ls nibble
	temp = (data >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//data ph2 - ms nibble
	
	return 1;
}

//Read from Command Register.
unsigned char Core::ReadReg(unsigned char addr, unsigned char id)
{
	unsigned char	temp = 0;
	DWORD			BytesRead = 0;
	DWORD			Size = 0;
	unsigned char	Buffer[1];
	unsigned char	t=0;

	fmpd0->clearBufferRX();

	fmpd0->WriteB(0x2A);			//read cmd
	temp = (addr & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//addr ph1 - ls nibble
	temp = (addr >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//addr ph2 - ms nibble

	while((Size < 1) && (t < 500))	//~500ms of timeout;
	{
		Size=fmpd0->GetSize();
		//if (1) printf("Buffer Size: %u - try: %u\r", Size, t);
		t++;
		Sleep(1);
	}

	if(Size > 1) FILEDEBUG("ReadReg Response Error - 0x%02X - id: %u!\n", addr, id);

	if (Size == 1) //Size > 1 is not a register value. Maybe ACQ'ed data?
	{
			fmpd0->Read(Buffer, BytesRead, 1);
			
			return Buffer[0];
	}
	
	FILEDEBUG("ReadReg Error - 0x%02X - id: %u!\n", addr, id);

	return 0;
}

//
unsigned char Core::WriteSSPI(unsigned char data, unsigned char id)
{
	unsigned char t=0;

	//while busy
	while (((ReadReg(0x71, id) & 0x01) == 0x01) && (t < 500)) t++;
	TestWReg(0x70, data, id);
	if (t==20){printf("SPI busy test TIMED OUT!\n"); FILEDEBUG("SPI busy test TIMED OUT - %u - id: %u!\n", 0, id);}
	t=0;
	//while no data
	while (((ReadReg(0x71, id) & 0x02) == 0x00) && (t < 500)) t++;
	if (t==20){printf("SPI data test TIMED OUT!\n"); FILEDEBUG("SPI data test TIMED OUT - %u - id: %u!\n", 0, id);}
	return ReadReg(0x70, id);
}

//
unsigned char Core::WriteCore(unsigned char addr, unsigned char data, unsigned char id)
{
	unsigned char	temp = 0;
	unsigned char	r = 0;

	r = WriteSSPI(0xAA, id); R;				//write cmd
	r = WriteSSPI(addr, id); R;				//addr
	r = WriteSSPI(data, id); R;				//data
	
	r = WriteSSPI(0xFF, id); R;				//getting response.
	
	if (r == 0xEB) return 1;
	
	return 0;
}

//unsigned char Core::WriteCore(unsigned char addr, unsigned char data, unsigned char id)
//{
//	unsigned char	temp = 0;
//	unsigned char	r = 0;
//
//	r = WriteSSPI(0xAA, id); R;				//write cmd
//	temp = (addr & 0x0F);					//temp = ls nibble from addr.
//	r = WriteSSPI((temp | 0xA0), id); R;	//addr ph1 - ls nibble
//	temp = (addr >> 4);						//temp = ms nibble from addr.
//	r = WriteSSPI((temp | 0x50), id); R;	//addr ph2 - ms nibble
//	temp = (data & 0x0F);					//temp = ls nibble from addr.
//	r = WriteSSPI((temp | 0xA0), id); R;	//data ph1 - ls nibble
//	temp = (data >> 4);						//temp = ms nibble from addr.
//	r = WriteSSPI((temp | 0x50), id); R;	//data ph2 - ms nibble
//	
//	r = WriteSSPI(0xFF, id); R;				//getting response.
//	
//	if (r == 0xEB) return 1;
//	
//	return 0;
//}

//
unsigned char Core::ReadCore(unsigned char addr, unsigned char id)
{
	unsigned char	temp = 0;
	unsigned char	r = 0;


	r = WriteSSPI(0x2A, id); R;				//read cmd
	r = WriteSSPI(addr, id); R;				//addr ph1 - ls nibble

	r = WriteSSPI(0xFF, id); R;				//getting response and it is the register's value.

	return r;
}

//
//unsigned char Core::ReadCore(unsigned char addr, unsigned char id)
//{
//	unsigned char	temp = 0;
//	unsigned char	r = 0;
//
//
//	r = WriteSSPI(0x2A, id); R;				//read cmd
//	temp = (addr & 0x0F);					//temp = ls nibble from addr.
//	r = WriteSSPI((temp | 0xA0), id); R;	//addr ph1 - ls nibble
//	temp = (addr >> 4);						//temp = ms nibble from addr.
//	r = WriteSSPI((temp | 0x50), id); R;	//addr ph2 - ms nibble
//
//	r = WriteSSPI(0xFF, id); R;				//getting response and it is the register's value.
//
//	return r;
//}

/*****************************************************************************/
/***************************** Test Functions ********************************/
/*****************************************************************************/

//SPI Loopback test. Core FPGA must be 'loopback enabled'.
unsigned char counter = 0;
void Core::Loopback(void)
{
	
	unsigned char r		= 0;
	//unsigned int sum	= 0; 
	
	r = WriteSSPI(counter, 0);

	printf("\n");
	printf("%u: Loopback: %u, %u\n", counter, r, 0/*Buffer[1]*/);
	
	//sum = counter + r;

	if((r != (counter-1)) && (r != 0xFF))
	{
		printf("Error!\n");
		_getch();
	}
	counter++;
}

void Core::TestVmeRW(void)
{
	unsigned char r=0;

	if (WriteReg(0x27, counter, 80)) printf("Write OK!\n");
	r = ReadReg(0x27, 81);
	printf("Read returns 0x%0.2X\n", r);
	if (r != counter)
	{
		printf("Error!\n");
		_getch();
	}
	counter++;
}

void Core::TestCoreRW(void)
{
	unsigned char r=0;

	if (WriteCore(0x27, counter, 82)) printf("Write OK!\n");
	else {printf("Error!\n"); _getch();}
	r = ReadCore(0x27, 83);
	printf("Read returns 0x%0.2X\n", r);
	if (r != counter)
	{
		printf("Error!\n");
		_getch();
	}
	counter++;
}

/*****************************************************************************/
/*****************************************************************************/


unsigned char Core::MapChannels(unsigned char config, unsigned char *channel){
	
	unsigned char pos=0;

		//Based on the Priority Encoded Circular Arbiter FSM
		for(unsigned char i=0;i<8;i++) *channel++=0;
		channel-=8;

		if ((config & 0x01) == 0x01) goto pos1; else
		if ((config & 0x02) == 0x02) goto pos2; else
		if ((config & 0x04) == 0x04) goto pos3; else
		if ((config & 0x08) == 0x08) goto pos4; else
		if ((config & 0x10) == 0x10) goto pos5; else
		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return 0;

	pos1:
		channel[0] = ++pos;

		if ((config & 0x02) == 0x02) goto pos2; else
		if ((config & 0x04) == 0x04) goto pos3; else
		if ((config & 0x08) == 0x08) goto pos4; else
		if ((config & 0x10) == 0x10) goto pos5; else
		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos2:
		channel[1] = ++pos;

		if ((config & 0x04) == 0x04) goto pos3; else
		if ((config & 0x08) == 0x08) goto pos4; else
		if ((config & 0x10) == 0x10) goto pos5; else
		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos3:
		channel[2] = ++pos;

		if ((config & 0x08) == 0x08) goto pos4; else
		if ((config & 0x10) == 0x10) goto pos5; else
		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos4:
		channel[3] = ++pos;

		if ((config & 0x10) == 0x10) goto pos5; else
		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos5:
		channel[4] = ++pos;

		if ((config & 0x20) == 0x20) goto pos6; else
		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos6:
		channel[5] = ++pos;

		if ((config & 0x40) == 0x40) goto pos7; else
		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos7:
		channel[6] = ++pos;

		if ((config & 0x80) == 0x80) goto pos8; else
		return pos;

	pos8:
		channel[7] = ++pos;
	
		return pos;
}
