#include "c_Settings.h"

//It should be a file database not global vars :)
unsigned char		c_config;
unsigned char		c_total;

//It is ok to be a global :)
bool				GraphsOpen;

Settings::Settings()
{
	//Future: Read Settings File.
	c_config = 1;
	c_total = 1;
	GraphsOpen = false;
}

Settings::~Settings()
{
	delete this;
}

void Settings::SetChanConfig(unsigned char nc_config) { c_config = nc_config; }
unsigned char Settings::GetChanConfig(void) { return c_config; }
void Settings::SetChanTotal(unsigned char nc_total) { c_total = nc_total; }
unsigned char Settings::GetChanTotal(void) { return c_total; }

bool Settings::GetGraphsOpen(void) { return GraphsOpen; }
void Settings::SetGraphsOpen(bool state) { GraphsOpen = state; }


