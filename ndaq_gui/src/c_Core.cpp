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

#define C				printf("WriteCore Error!\n"); _getch();
#define E				printf("WriteReg Error!\n");
#define R				if (DEBUG) printf("Response 0x%0.2X\n", r);

#define TestWCore(a, d)	{if(!WriteCore(a, d)) {C}}
#define TestWReg(a, d)	{if(!WriteReg(a, d)) {E}}

Core::Core()
{
	//Initialize MPD
	fmpd0 = new MPD(0); //Instanciando o objeto fmpd0 da classe MPD. Ou seja, tornar essa bendenga REAL ! 
	Run = false;
	lc_config = 0;
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

	fmpd0->SetLatency(16);

	CheckClear();
	WriteReg(0xAA, 0x55);		//Resets Vme FPGA
	WriteReg(0x80, 0x80);		//Grant we'll have command responses.
	WriteCore(0xAA,0x55);		//Resets Core FPGA
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
		TestWCore(0x7B, 0x01);				//Internal Trigger Slope: 0 -> rising, 1 -> falling	
	}
	//if slope is rising...	
	else
		TestWCore(0x7B, 0x00);
	
	//if slope is falling...
	if (slope == false)
	{	
		printf("\nLow byte: 0x%02X - High byte: 0x%02X\n", (unsigned char) th, ((th >> 8) & 0x03));
		TestWCore(0x79, (unsigned char) th);		//Th2: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x7A, temp);						//Th2: high byte
		//Hysteresis        
		th = th+1;
		TestWCore(0x77, (unsigned char) th);		//Th1: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x78, temp);						//Th1: high byte
	}
	//if slope is rising...
	else
	{
		printf("\nLow byte: 0x%02X - High byte: 0x%02X\n", (unsigned char) th, ((th >> 8) & 0x03));
		TestWCore(0x79, (unsigned char) th);		//Th2: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x7A, temp);						//Th2: high byte
		//Hysteresis        
		th = th-1;
		TestWCore(0x77, (unsigned char) th);		//Th1: low byte
		temp = (th >> 8) & 0x03; 
		TestWCore(0x78, temp);						//Th1: high byte
	}
	
	//if trigger is internal...
	if (mode == false)
	{
		TestWCore(0x91, 0x80);						//ACQ Register - Bit 7: 0 -> external trigger, 1 -> internal trigger
	}
	//if trigger is external...	
	else
		TestWCore(0x91, 0x00);
}	

void Core::SetRun(bool state)
{
	unsigned char b_btst = 0x01;
	unsigned char n_btst = 0x01;
	unsigned char adc_pwdn = 0x0F;

	
	
	//Start
	if(state){
		WriteReg(0x80, 0x80);				//Grant we'll have command responses.

		//TestWCore(0x00, 0x00);			//
		
		TestWCore(0x89, 0x01);				//ACQ Reset Assert
		Sleep(16);
		TestWCore(0x89, 0x00);				//ACQ Reset Deassert
		
		TestWCore(0x80, 0x01);				//DataBuilder Enable

		TestWReg(0x80, 0x00);				//From here we won't have command responses anymore.
		
		//***CheckClear();						//Ensure Receive Buffer is clear.

		WriteReg(0x82, 0x01);				//Readout Reset Assert
		//Sleep(16);
		WriteReg(0x82, 0x00);				//Readout Reset Deassert
		//WriteReg(0x80, 0x01);				//Vme Readout Enable.
		//WriteReg(0x81, lc_config);		//Vme Channel Selector.
		WriteReg(0x81, 0x0F);
		WriteReg(0x80, 0x01);				//Vme Readout Enable.

		Run = true;

	//Stop
	}else{
		CheckClear();						//Ensure Receive Buffer is clear.
		WriteReg(0x80, 0x80);				//Return grant to command responses.
		Sleep(120);
		//TestWCore(0x87, 0x00);			//ADC Power-down
		//TestWCore(0x91, 0x00);			//ACQ Disable
		TestWCore(0x80, 0x00);				//DataBuilder Disable
				
		TestWReg(0x81, 0x00);				//Vme Channel Selector - Disable ALL Channels.
		TestWReg(0x80, 0x80);				//Vme Readout Disable.

		//CheckClear();						//Ensure Receive Buffer is clear.
		//TestWReg(0xAA, 0x55);				//Resets Vme FPGA 
		//TestWReg(0x80, 0x80);				//Return grant to command responses.

		//VERIFICAR!
		//Sem isso, perde-se a sincronia com 8 canais habilitados
		//TestWCore(0xAA,0x55);				//Resets Core FPGA

		Run = false; //if it really stopped.
		
	}	
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
	
	if (1/*DEBUG*/) printf(" - Buffer Size: %u\n", Size);

	if(Size > BLOCK_SIZE-1){
			//fmpd0->Write(0xAB, 0x55);	//Resets TX INTERFACE's Byte Counter

			ReadSize = Size/BLOCK_SIZE;
			ReadSize = ReadSize*BLOCK_SIZE;

			fmpd0->Read(/*(unsigned char *)*/Buffer, BytesRead, ReadSize /*BLOCK_SIZE*/ /*Size*/);
		
		//if (BytesRead == ReadSize)
			return (unsigned int)BytesRead; //1
		//else
			//for(;;); //return 0;
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
	while((Size > 0) && (t < 20))	//~20ms of timeout;
	{
		fmpd0->clearBufferRX();	
		Size=fmpd0->GetSize();
		t++;
		Sleep(1);
	}
	printf("First Size: %u\n", Size);
}

//Write to Command Register - ***TEST ERROR INSIDE OR SOMETHING LIKE THAT (Already done?!)***
unsigned char Core::WriteReg(unsigned char addr, unsigned char data)
{
	unsigned char	temp = 0;
	DWORD		BytesRead = 0;
	DWORD		Size = 0;
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
	
	
	while((Size < 1) && (t < 20))	//~20ms of timeout;
	{
		Size=fmpd0->GetSize();
		t++;
		Sleep(1);
	}
	
	//if(Size > 0)
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

//Read from Command Register - ***CHANGE TO RETURN POINTER TO DATA*** (Really?)
unsigned char Core::ReadReg(unsigned char addr)
{
	unsigned char	temp = 0;
	DWORD		BytesRead = 0;
	DWORD		Size = 0;
	unsigned char	Buffer[1];
	unsigned char	t=0;

	fmpd0->clearBufferRX();

	fmpd0->WriteB(0x2A);			//read cmd
	temp = (addr & 0x0F);			//temp = ls nibble from addr.
	fmpd0->WriteB((temp | 0xA0));	//addr ph1 - ls nibble
	temp = (addr >> 4);				//temp = ms nibble from addr.
	fmpd0->WriteB((temp | 0x50));	//addr ph2 - ms nibble

	while((Size < 1) && (t < 20))	//~20ms of timeout;
	{
		Size=fmpd0->GetSize();
		//if (1) printf("Buffer Size: %u - try: %u\r", Size, t);
		t++;
		Sleep(1);
	}

	if(Size > 0)
	{
			fmpd0->Read(Buffer, BytesRead, 1);
			
			return Buffer[0];
	}

	return 0;
}

//
unsigned char Core::WriteSSPI(unsigned char data)
{
	unsigned char t=0;

	//while busy
	while (((ReadReg(0x71) & 0x01) == 0x01) && (t < 20)) t++;
	//if(!(WriteReg(0x70, data))) E;
	TestWReg(0x70, data);
	if (t==20) printf("SPI busy test TIMED OUT!\n");
	t=0;
	//while no data
	while (((ReadReg(0x71) & 0x02) == 0x00) && (t < 20)) t++;
	if (t==20) printf("SPI data test TIMED OUT!\n");
	return ReadReg(0x70);
}


unsigned char Core::WriteCore(unsigned char addr, unsigned char data)
{
	unsigned char	temp = 0;
	unsigned char	r = 0;

	r = WriteSSPI(0xAA); R;				//write cmd
	temp = (addr & 0x0F);				//temp = ls nibble from addr.
	r = WriteSSPI((temp | 0xA0)); R;	//addr ph1 - ls nibble
	temp = (addr >> 4);					//temp = ms nibble from addr.
	r = WriteSSPI((temp | 0x50)); R;	//addr ph2 - ms nibble
	temp = (data & 0x0F);				//temp = ls nibble from addr.
	r = WriteSSPI((temp | 0xA0)); R;	//data ph1 - ls nibble
	temp = (data >> 4);					//temp = ms nibble from addr.
	r = WriteSSPI((temp | 0x50)); R;	//data ph2 - ms nibble
	
	r = WriteSSPI(0xFF); R;				//getting response.
	
	if (r == 0xEB) return 1;
	
	return 0;
}


unsigned char Core::ReadCore(unsigned char addr)
{
	unsigned char	temp = 0;
	unsigned char	r = 0;


	r = WriteSSPI(0x2A); R;				//read cmd
	temp = (addr & 0x0F);				//temp = ls nibble from addr.
	r = WriteSSPI((temp | 0xA0)); R;	//addr ph1 - ls nibble
	temp = (addr >> 4);					//temp = ms nibble from addr.
	r = WriteSSPI((temp | 0x50)); R;	//addr ph2 - ms nibble

	r = WriteSSPI(0xFF); R;				//getting response and it is the register's value.

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
	
	r = WriteSSPI(counter);

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

	if (WriteReg(0x27, counter)) printf("Write OK!\n");
	r = ReadReg(0x27);
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

	if (WriteCore(0x27, counter)) printf("Write OK!\n");
	else {printf("Error!\n"); _getch();}
	r = ReadCore(0x27);
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
