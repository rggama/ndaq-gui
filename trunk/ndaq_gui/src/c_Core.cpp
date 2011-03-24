//=============================================================================
// File Name   : c_Core.cpp
// Pseudo Name : NDAQ Event Catcher (NDEC)
// Author      : Rafael Gama
// Version     : 0.2b
// Copyright   : IF YOU COPY THIS, YOU'RE A HUGE VAGABUNDO!
// Description : Not today...
//=============================================================================
#include "defines.h"
#include <stdio.h>

#include "c_Core.h"


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

	fmpd0->Write(0xAA, 0x55);		//Resets hardware
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

	Run = state;
	
	//Start
	if(state){
		fmpd0->Write(0xAA,0x55);

		//fmpd0->Write(0x90,0x00);		//'BCOUNT' Low
		//fmpd0->Write(0x91,0x1F);		//'BCOUNT' High

		//fmpd0->Write(0xB0,0x00);		//C8 'WMAX' Low
		//fmpd0->Write(0xB1,0x02);		//C8 'WMAX' High

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

		fmpd0->Write(0x80, adc_pwdn);	//ADCs Power down
		
		fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer
		//fmpd0->Write(0xAB,0x55);		//Reset 63488b counter
		fmpd0->Write(0x82,lc_config);	//Configure Channels Readout
		fmpd0->Write(0x81,0x08);		//Start Readout FSM
		fmpd0->Write(0xC0,0x01);
	//Stop
	}else{
		fmpd0->Write(0x81,0);			// Stop Readout FSM
		fmpd0->Write(0xC0,0);

		fmpd0->clearBufferRX();			//Clear SOFTWARE RX Buffer

		fmpd0->Write(0x80,0x0F);		//Power Down ALL ADCs
	}	
}

void Core::ToggleRun()
{
	Run = !Run;
	SetRun(Run);
}

unsigned int Core::Acq(unsigned char *Buffer)
{
	unsigned long BytesRead = 0;
	unsigned long Size = 0;
	unsigned long ReadSize = 0;

	Size=fmpd0->GetSize();
	
	if (DEBUG) printf("Buffer Size: %u\n", Size);

	if(Size > 4 /*BLOCK_SIZE-1*/){
			//fmpd0->Write(0xAB, 0x55);	//Resets TX INTERFACE's Byte Counter

			ReadSize = Size/4;
			ReadSize = ReadSize*4;

			fmpd0->Read(/*(unsigned char *)*/Buffer, BytesRead, ReadSize /*BLOCK_SIZE*/ /*Size*/);
			
			return (unsigned int)BytesRead; //1
	}

	return 0;
}

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
