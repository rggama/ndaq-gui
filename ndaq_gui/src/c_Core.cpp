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
#include <conio.h>

#include "c_Core.h"

#define E	printf("WriteReg Error!\n")
#define R	if (DEBUG) printf("Response 0x%0.2X\n", r)

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

	fmpd0->SetLatency(16);

	CheckClear();
	WriteReg(0xAA, 0x55);		//Resets Vme FPGA
	//Colocar como valor padrao no FPGA
	WriteReg(0x80, 0x80);		//Grant we'll have command responses.
	WriteCore(0xAA,0x55);		//Resets Core FPGA
}

void Core::SetChannels(unsigned char c_config)
{
	lc_config = c_config;
}

void Core::SetRun(bool state)
{
	unsigned char b_btst = 0x01;
	unsigned char n_btst = 0x01;
	unsigned char adc_pwdn = 0x0F;

	
	
	//Start
	if(state){
		WriteReg(0x80, 0x80);				//Grant we'll have command responses.
		Sleep(50);

		//WriteReg(0xAA, 0x55);
		
		//fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer

		//WriteCore(0x87, 0x01);			//ADC Power-up
		WriteCore(0x77, 0x55);				//Th
		
		//WriteCore(0x89, 0x01);			//FIFO Reset Assert
		//WriteCore(0x89, 0x00);			//FIFO Reset Deassert
		//Sleep(20);
		//WriteCore(0x91, 0x01);			//ACQ Enable

		WriteReg(0x80, 0x00);				//From here we won't have command responses anymore.
		CheckClear();						//Ensure Receive Buffer is clear.

		WriteReg(0x80, 0x01);				//Vme Readout Enable.
		WriteReg(0x81, lc_config);			//Vme Channel Selector.
				
		Run = true;

	//Stop
	}else{
		//WriteCore(0x87, 0x00);			//ADC Power-down
		//WriteCore(0x91, 0x00);			//ACQ Disable
		
		Run = false;
		Sleep(50);

		CheckClear();						//Ensure Receive Buffer is clear.
		WriteReg(0xAA, 0x55);				//Resets Vme FPGA
		WriteReg(0x80, 0x80);				//Return grant to command responses.
		WriteCore(0xAA,0x55);				//Resets Core FPGA

		//*Run = false; //if it really stopped.
	}	
}

/*
void Core::SetRun(bool state)
{
	unsigned char b_btst = 0x01;
	unsigned char n_btst = 0x01;
	unsigned char adc_pwdn = 0x0F;

	
	
	//Start
	if(state){
		WriteCore(0xAA, 0x55);
		WriteReg(0xAA, 0x55);

		for(unsigned char i=1;i<9;i++)
		{
			if ((lc_config & b_btst) == b_btst)
			{
				//0: enable, 1: disable.
				adc_pwdn = adc_pwdn & ((unsigned char)~n_btst); //enabling.
			}
			b_btst = b_btst<<1; //sweeping 8 bits var.
			if ((i%2) == 0) n_btst = n_btst<<1; //sweeping 4 bits var.
		}

		////fmpd0->Write(0x80, adc_pwdn);	//ADCs Power down
		
		fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer

		WriteCore(0x80,0x01);
		WriteReg(0x80, 0x01);
		//fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer
		Run = true;

		 //if everything went well.
	//Stop
	}else{
		////fmpd0->Write(0x81,0);			// Stop Readout FSM

		////fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer

		////fmpd0->Write(0x80,0x0F);		//Power Down ALL ADCs
		StopReadout();
		WriteCore(0x80,0x00);
		Run = false; //if it really stopped.
	}	
}
*/

void Core::ToggleRun()
{
	//Do not change 'Run'. Let 'SetRun' do it based on hardware check.
	if (Run)
		SetRun(false);
	else
		SetRun(true);
}

//READ ON DEMAND :D
unsigned int Core::Acq(unsigned char *Buffer)
{
	unsigned long BytesRead = 0;
	unsigned long Size = 0;
	unsigned long ReadSize = 0;

	Size=fmpd0->GetSize();
	
	if (DEBUG) printf(" - Buffer Size: %u\n", Size);

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
	unsigned long	Size = 0;
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

//Write to Command Register - ***TEST ERROR INSIDE OR SOMETHING LIKE THAT***
unsigned char Core::WriteReg(unsigned char addr, unsigned char data)
{
	unsigned char	temp = 0;
	unsigned long	BytesRead = 0;
	unsigned long	Size = 0;
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
		//if (1) printf("Buffer Size: %u - try: %u\r", Size, t);
		t++;
		Sleep(1);
	}
	
	if(Size > 0)
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
	unsigned long	BytesRead = 0;
	unsigned long	Size = 0;
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
	if(!(WriteReg(0x70, data))) E;
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

	//unsigned long	BytesRead = 0;
	//unsigned long	Size = 0;
	//unsigned char	Buffer[1];
	//unsigned char	t=0;

	//fmpd0->clearBufferRX();

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
