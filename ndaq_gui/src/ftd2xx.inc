// File named ftd2xx.cxx
//#ifdef __CINT__
//class pthread_cond_t;
//class pthread_mutex_t;
//#endif

//#include "ftd2xx.h"
//#include <iostream>
//using namespace std;

#define MAX_DEVICES 4

//Global Handler
FT_HANDLE	ftHandle[MAX_DEVICES];

int DeviceInfoList()
{
	FT_STATUS ftStatus; 
	FT_DEVICE_LIST_INFO_NODE *devInfo; 
	DWORD numDevs; 
	int id;
	 
	// create the device information list 
	ftStatus = FT_CreateDeviceInfoList(&numDevs); 
	 
	if (ftStatus == FT_OK) { 
	  printf("Number of devices is %d\n",numDevs); 
	} 
	 
	if (numDevs > 0) { 
	  // allocate storage for list based on numDevs 
	  devInfo = 
	(FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);  
	  // get the device information list 
	  ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);  
	  if (ftStatus == FT_OK) { 
		for (unsigned int i = 0; i < numDevs; i++) { 
		  printf("Dev %d:\n",i);  
		  printf("  Flags=0x%x\n",devInfo[i].Flags);  
		  printf("  Type=0x%x\n",devInfo[i].Type);  
		  printf("  ID=0x%x\n",devInfo[i].ID);  
		  printf("  LocId=0x%x\n",devInfo[i].LocId);  
		  printf("  SerialNumber=%s\n",devInfo[i].SerialNumber);  
		  printf("  Description=%s\n",devInfo[i].Description);  
		  printf("  ftHandle=0x%x\n",devInfo[i].ftHandle);
		  
		  if (strcmp(devInfo[i].Description, "NDAQ - Standalone Mode") == 0)
		  {
			printf("\nHello NDAQ!\n\n");
			id = i;
		  }
		  
		} 
	  }
	  else return -1;
	}
	else return -1;
	
	return id; 
}

//código modificado para se conectar a mais de um MPD
int OpenFtd(int i)
{
	DWORD dwRxBytes=0;
    DWORD dwTxBytes=0;
    DWORD dwEventDWord=0;
    
	FT_STATUS	ftStatus;
	//ftStatus = FT_GetStatus(ftHandle[i], &dwRxBytes, &dwTxBytes, &dwEventDWord);

	if((ftStatus = FT_Open(i,&ftHandle[i])) != FT_OK)
		return 0;
	
	return 1;
}

//código para fechar a conexão com um MPD
int CloseFtd(int i)
{
	FT_STATUS	ftStatus;
	
	if((ftStatus = FT_Close(ftHandle[i])) != FT_OK)
		return 0;

	return 1;
}

int WriteFtd(int i, unsigned char data)
{
	FT_STATUS	ftStatus;
	DWORD		buffersize = 1;
	DWORD		dwBytesWritten;

	if((ftStatus = FT_Write(ftHandle[i], &data, buffersize, &dwBytesWritten)) != FT_OK)
		return 0;
	
	return (int)dwBytesWritten;
}

int GetSIZE(int i)
{
	DWORD		dwRxSize = 0;
	FT_STATUS	ftStatus;
	
	ftStatus = FT_GetQueueStatus(ftHandle[i], &dwRxSize);
	
	if (ftStatus != FT_OK)
		return 0;
	
	return (int)dwRxSize;
}


void ReadFtd(int i, unsigned char *data, DWORD &dwBytesRead, DWORD dwRxSize)
{
	FT_STATUS ftStatus;

	ftStatus = FT_Read(ftHandle[i], data, dwRxSize, &dwBytesRead);
}

int clearRX(int i)
{	
	FT_STATUS ftStatus = FT_Purge(ftHandle[i],FT_PURGE_RX);
	if (ftStatus != FT_OK)  
		return 0;
	
	return 1;
}

int clearTX(int i)
{	
	FT_STATUS ftStatus = FT_Purge(ftHandle[i],FT_PURGE_TX);
	if (ftStatus != FT_OK)  
		return 0;
	
	return 1;
}

int ResetDevice(int i)
{	
	FT_STATUS ftStatus = FT_ResetDevice(ftHandle[i]);
	if (ftStatus != FT_OK)  
		return 0;
	
	return 1;
}

int SetLatencyFtd(int i, unsigned char latency)
{
	FT_STATUS ftStatus = FT_SetLatencyTimer(ftHandle[i], latency);
	if (ftStatus != FT_OK)  
		return 0;

	return 1;
}

int SetBlockFtd(int i,unsigned int block)
{
	FT_STATUS ftStatus = FT_SetUSBParameters(ftHandle[i], block, 0);
	if (ftStatus != FT_OK)  
		return 0;

	return 1;
}

int SetFlowControlFtd(int i, USHORT FlowControl)
{
	FT_STATUS ftStatus = FT_SetFlowControl(ftHandle[i], FlowControl, 0, 0);
	if (ftStatus != FT_OK)  
		return 0;

	return 1;
}

int GetNDevices()
{
	DWORD iNumDevs = 0;
	
	FT_STATUS ftStatus = FT_CreateDeviceInfoList(&iNumDevs);
	if (ftStatus != FT_OK)  
		return 0;

	return (int)iNumDevs;
}

#ifdef __MAKECINT__
#pragma link off class _FTCOMSTAT-;
#pragma link off class _FTDCB-;
#endif
