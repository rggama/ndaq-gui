// header files required by application

//#include <RQ_OBJECT.h>


class Settings {

//RQ_OBJECT("Settings")

private:

public:
   Settings();
   virtual ~Settings();

   void SetChanConfig(unsigned char nc_config);/* { c_config = nc_config; }*/
   unsigned char GetChanConfig(void);/* { return c_config; }*/
   void SetChanTotal(unsigned char nc_total);/* { c_total = nc_total; }*/
   unsigned char GetChanTotal(void);/* { return c_total; }*/
};


