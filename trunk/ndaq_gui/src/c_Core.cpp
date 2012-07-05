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
	fmpd0 = new MPD(0);
	Run = false;
	lc_config = 0;
	acq_config = 0;
	t_blocks = 0;
	block_size = 65535;
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
	WriteReg(0xAA, 0x55, 90);		//Resets Vme FPGA
	WriteReg(0x80, 0x80, 91);		//Grant we'll have command responses.
	WriteCore(0xAA,0x55, 92);		//Resets Core FPGA
}

void Core::SetChannels(unsigned char c_config)
{
	lc_config = c_config;
}

void Core::SetTrigger(bool mode, bool slope, signed short th, unsigned char isel)
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
		TestWCore(0x71, acq_config, 40);				//Bit 7: 0 -> external trigger, 1 -> internal trigger
		TestWCore(0x70, isel, 41);						//Trigger Output Selector
	}
	//if trigger is external...	
	else
	{
		acq_config = 0x00;
		TestWCore(0x71, acq_config, 42);
	}
}	

void Core::MakeConfig(unsigned char cword, NDAQ_CONFIG *config)
{
	//ACQ Configuration (config->ac).
	// B7  B6  B5  B4  B3  B2  B1  B0
	//[ N | N | N | A | I | E | C | T ]
	//-N : Not used.
	//-A : ADC				-> (1)Enable, (0)Disable.
	//-I : Internal Trigger	-> (1)Enable, (0)Disable.
	//-E : External Trigger	-> (1)Enable, (0)Disable.
	//-C : Counters			-> (1)Enable, (0)Disable.
	//-T : Timebase			-> (1)Enable, (0)Disable.

	//Core Databuilder FIFO Block Configuration (per fifo) (config->bc1,bc2,bc3,bc4).
	// B7  B6  B5  B4  B3  B2  B1  B0
	//[ N | N | c | C | t | A | T | H ]
	//-N : Not used.
	//-c : Odd Counter		-> (1)Enable, (0)Disable.
	//-C : Even Counter		-> (1)Enable, (0)Disable.
	//-t : TDC Odd+Even		-> (1)Enable, (0)Disable.
	//-A : ADC Odd+Even		-> (1)Enable, (0)Disable.
	//-T : Timebase			-> (1)Enable, (0)Disable.
	//-H : Header			-> (1)Enable, (0)Disable.
	
	//VME Databuilder FIFO Transfer Configuration (config->vc).
	// B7  B6  B5  B4  B3  B2  B1  B0
	//[ N | N | N | N | 4 | 3 | 2 | 1 ]
	//-N : Not used.
	//-4 : FIFO 4 Transfer	-> (1)Enable, (0)Disable.
	//-3 : FIFO 3 Transfer	-> (1)Enable, (0)Disable.
	//-2 : FIFO 2 Transfer	-> (1)Enable, (0)Disable.
	//-1 : FIFO 1 Transfer	-> (1)Enable, (0)Disable.

	//reseting total blocks count
	t_blocks = 0;

	//Fixed by now.
	config->ac = 0x1F;

	//if channel 1 or channel 2 is enabled
	if (((cword & 0x01) == 0x01) || ((cword & 0x02) == 0x02))
	{		
		//enable everything in a block for FIFO 1.
		config->bc1 = 0x37;
		//Turn ON FIFO 1 transfer on VME's databuilder.
		config->vc = 0x01;
		t_blocks++;
	}
	else
	{
		//enable none for FIFO 1.
		config->bc1 = 0x00;
		//Do NOT enable FIFO 1 transfer on VME's databuilder.
		config->vc = 0x00;
	}

	//if channel 3 or channel 4 is enabled
	if (((cword & 0x04) == 0x04) || ((cword & 0x08) == 0x08))
	{
		//enable everything in a block for FIFO 2.
		config->bc2 = 0x37;
		//Turn ON FIFO 2 transfer on VME's databuilder.
		config->vc |= 0x02;
		t_blocks++;
	}
	else
	{	
		//enable none for FIFO 2.
		config->bc2 = 0x00;
		//Do NOT enable FIFO 2 transfer on VME's databuilder.
		config->vc |= 0x00;
	}

	//if channel 5 or channel 6 is enabled
	if (((cword & 0x10) == 0x10) || ((cword & 0x20) == 0x20))
	{	
		//enable everything in a block for FIFO 3.
		config->bc3 = 0x37;
		//Turn ON FIFO 3 transfer on VME's databuilder.
		config->vc |= 0x04;
		t_blocks++;
	}
	else
	{
		//enable none for FIFO 3.
		config->bc3 = 0x00;
		//Do NOT enable FIFO 3 transfer on VME's databuilder.
		config->vc |= 0x00;
	}

	//if channel 7 or channel 8 is enabled
	if (((cword & 0x40) == 0x40) || ((cword & 0x80) == 0x80))
	{
		//enable everything in a block for FIFO 4.
		config->bc4 = 0x37;
		//Turn ON FIFO 4 transfer on VME's databuilder.
		config->vc |= 0x08;
		t_blocks++;
	}
	else
	{
		//enable none for FIFO 4.
		config->bc4 = 0x00;
		//Do NOT enable FIFO 4 transfer on VME's databuilder.
		config->vc |= 0x00;
	}

	//VME Databuilder WORDS per FIFO (in SLOT Size).
	//Fixed by now.
	config->vs = 0x83; //Its the size (in SLOTs) of 'everything enabled' for each FIFO block;

	#ifdef MAX_TP
		block_size = BUFFER/(t_blocks*FIFO_BS*SLOT_SIZE);
		block_size = block_size*(t_blocks*FIFO_BS*SLOT_SIZE);
	#else
		block_size = t_blocks*FIFO_BS*SLOT_SIZE;		//*SLOT_SIZE because it is in bytes.
	#endif

	//ZERO t_blocks WORKAROUND! 
	if (t_blocks == 0 ) t_blocks = 1;

	printf("Defined Block_Size: %u\n", block_size);
}

void Core::Config(NDAQ_CONFIG config)
{
	WriteReg(0x80, 0x80, 80);						//Grant we'll have command responses.

	TestWCore(0x80, config.ac, 81);					//ACQ Configuration - Enabling Desired Peripherals.
	TestWCore(0x41, config.bc1, 82);				//Core DataBuilder Configuration - FIFO 1.
	TestWCore(0x42, config.bc2, 83);				//Core DataBuilder Configuration - FIFO 2.
	TestWCore(0x43, config.bc3, 84);				//Core DataBuilder Configuration - FIFO 3.
	TestWCore(0x44, config.bc4, 85);				//Core DataBuilder Configuration - FIFO 4.

	TestWReg(0x83, config.vs, 86);					//Vme DataBuilder Block Size Configuration in WORDS less one (config).
	TestWReg(0x81, config.vc, 87);					//Vme DataBuilder Configuration.
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
		
		TestWCore(0x81, 0x7F, 1);					//127(+1) samples per trigger (config).
		
		TestWCore(0x89, 0x01, 2);					//ACQ Reset Assert.
		Sleep(32);
		TestWCore(0x89, 0x00, 3);					//ACQ Reset Deassert.
		Sleep(32);

		TestWCore(0x40, 0x01, 4);					//Core DataBuilder Enable.

		/********************/

		WriteReg(0x80, 0x00, 5);					//From here we won't have command responses anymore.
		
		CheckClear();								//Ensure Receive Buffer is clear.

		WriteReg(0x82, 0x01, 6);					//Readout Reset Assert.
		Sleep(32);
		WriteReg(0x82, 0x00, 7);					//Readout Reset Deassert.
		Sleep(32);

		WriteReg(0x80, 0x01, 8);					//Vme Readout Enable (DataBuilder enable).

		Run = true;

	//Stop
	}else{
		WriteReg(0x80, 0x80, 9);					//Return grant to command responses.
		Sleep(360);
		CheckClear();								//Ensure Receive Buffer is clear.
		
		TestWCore(0x40, 0x00, 10);					//Core DataBuilder Disable.
		
		//TestWCore(0x87, 0x00);					//ADC Power-down.
				
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

	if(Size > (DWORD)block_size-1)
	{
		ReadSize = Size/block_size;
		ReadSize = ReadSize*block_size;

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

unsigned char Core::MapChannels(unsigned char config)
{
	unsigned char btst = 0x01;
	unsigned char t_channels = 0;
	unsigned char *chmap = &chmap_vector[0];
	unsigned char *chadd = &chadd_vector[0];

	for(unsigned char i=0;i<MAX_CHANNELS;i++){
		//if channel is enabled...
		if ((config & btst) == btst)
		{
			*chmap++ = i+1;
			t_channels++;
		}
		else
		{
			*chmap++ = 0;
		}
		//If a block is fully inactive, skip it.
		if ((i>0) && (i%2!=0) && (*(chmap-1) == 0) && (*(chmap-2) == 0)) chmap-=2;
		//Incremet bit test.
		btst = btst<<1;
	}
	
	chmap = &chmap_vector[0];
	
	
	for (unsigned char c=0;c<t_blocks;c++)
	{
		printf("Mapped Channels: %u->%u\n", c, *chmap++);	
		printf("Mapped Channels: %u->%u\n", c, *chmap++);	
	}
	

	return t_channels;
}

