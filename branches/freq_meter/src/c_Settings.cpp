#include "c_Settings.h"

//It should be a file database not global vars :)
unsigned char		c_config;
unsigned char		c_total;

Settings::Settings()
{
	//Future: Read Settings File.
	c_config = 1;
	c_total = 1;
}

Settings::~Settings()
{
	delete this;
}

void Settings::SetChanConfig(unsigned char nc_config) { c_config = nc_config; }
unsigned char Settings::GetChanConfig(void) { return c_config; }
void Settings::SetChanTotal(unsigned char nc_total) { c_total = nc_total; }
unsigned char Settings::GetChanTotal(void) { return c_total; }

