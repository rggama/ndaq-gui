//=============================================================================
// File Name   : f_MainFrame.cpp
// Pseudo Name : NDAQ Event Catcher (NDEC)
// Author      : Rafael Gama
// Version     : svn controlled.
// Copyright   : IF YOU COPY THIS, YOU'RE A HUGE VAGABUNDO!
// Description : Not today...
//=============================================================================
#include "defines.h"
#include <conio.h>

#include "f_MainFrame.h"
//#include "main.h"

#include "a_graph.h"
#include "a_laudo.h"
#include "s_wave.h"
#include "calibration.h"

;char filename[] = "Filename";
char suffix[] = "Ch";
char namevector[280];

//

// *** HAD TO MAKE THESE VARS GLOBALS, IN ORDER TO THE SIGNAL TRIGGERED METHOD SettingsUpdate()
// *** WORK PROPERLY. WHEN THESE VARS ARE DECLARED IN THE CLASS STATEMENT, THE REFERED METHOD
// *** IS NOT ABLE TO ACCESS THEM. W H Y !?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!
TGComboBox *fComboGraph;
TGComboBox *fComboMeas;
TGComboBox *fComboFrom;

//unsigned long int totalEvents=0;
Long_t totalEvents=0;
unsigned long save_count=0;
//int t_zero=0;

unsigned char graph_counter=1;
bool running=false;
bool saving=false;
bool zero_seek=false;
unsigned int zscounter=0;
unsigned long bscounter=0;
int start_time=0;


MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h)
{	
	fbslope = true;
	saveWave = false;
	saveTable = false;
	timeP = 0;
	timeA = 0;
	timer = 0;
	sIntervalEn = false;
	last_timestamp = 0;

	SetCleanup(kDeepCleanup);
	
	SetWindowName("NDAQ Event Catcher"); //set a name to the main frame
	//SetIconPixmap("./");
	
	
	/****************************************************************************************************/
	/*** MainFame: Menu Bar *****************************************************************************/
	/****************************************************************************************************/
	// Create menubar and popup menus. The hint objects are used to place
	// and group the different menu widgets with respect to eachother.
	fMenuDock = new TGDockableFrame(this);
	AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
	fMenuDock->SetWindowName("NDEC Menu");
	fMenuDock->EnableUndock(kTRUE);
	fMenuDock->EnableHide(kTRUE);

	fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
	fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
	fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

	fMenuFile = new TGPopupMenu(gClient->GetRoot());
	//fMenuFile->AddSeparator();
	fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
	fMenuFile->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

	fMenuSettings = new TGPopupMenu(gClient->GetRoot());
	fMenuSettings->AddEntry("&Channels", M_SETTINGS_CHANNELS);
	fMenuSettings->AddEntry("&Data Flow", M_SETTINGS_DATAFLOW);
	//fMenuSettings->AddSeparator();
	//fMenuSettings->DisableEntry(M_VIEW_DOCK);
	fMenuSettings->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

	//fMenuSettings->CheckEntry(M_VIEW_ENBL_DOCK);
	//fMenuSettings->CheckEntry(M_VIEW_ENBL_HIDE);

	fMenuView = new TGPopupMenu(gClient->GetRoot());
	fMenuView->AddEntry("&All Channels", M_VIEW_GRAPHS);
	fMenuView->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

	// When using the DockButton of the MenuDock,
	// the states 'enable' and 'disable' of menus have to be updated.
	//fMenuDock->Connect("Undocked()", "MainFrame", this, "HandleMenu(255)");

	fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
	fMenuBar->AddPopup("&File",		fMenuFile,		fMenuBarItemLayout);
	fMenuBar->AddPopup("&Settings",	fMenuSettings,	fMenuBarItemLayout);
	fMenuBar->AddPopup("&CAN",		fMenuView,		fMenuBarItemLayout);
	//fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

	fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);
	
	/****************************************************************************************************/
	/*** MainFrame: MiddleFrame *************************************************************************/
	/****************************************************************************************************/
	fMiddleFrame = new TGCompositeFrame(this, 60, 20, kHorizontalFrame |
                                                      kSunkenFrame);

	/****************************************************************************************************/
	/*** MiddleFrame: Canvas ****************************************************************************/
	/****************************************************************************************************/
	fEcanvas1 = new TRootEmbeddedCanvas(0,fMiddleFrame,400,300);
	fIDcanvas1 = fEcanvas1->GetCanvasWindowId();
   	fcanvasMPD1 = new TCanvas("ndeccanvas", 10, 10, fIDcanvas1);
   	fEcanvas1->AdoptCanvas(fcanvasMPD1);
	fEcanvas1->GetCanvas()->SetGridx();
	fEcanvas1->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas1, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));
	

	/****************************************************************************************************/
	/*** MiddleFrame: LGroupFrame ***********************************************************************/
	/****************************************************************************************************/
	fLGroupFrame = new TGCompositeFrame(fMiddleFrame, 60, 20, kVerticalFrame |
                                                      kSunkenFrame);

	/****************************************************************************************************/
	/*** Graph Group ************************************************************************************/
	/****************************************************************************************************/
   	fgroupGraph = new TGGroupFrame(fLGroupFrame,"Graphic", kVerticalFrame);
	
	// Create ComboBox Graphic
  	fComboGraph = new TGComboBox(fgroupGraph,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  	fComboGraph->AddEntry("None", 0);
	fComboGraph->Select(0, kTRUE);
  	fgroupGraph->AddFrame(fComboGraph, new TGLayoutHints(kLHintsLeft | kLHintsTop ,0,0,0,0));
	fComboGraph->MoveResize(10,24,100,20);
	
	fLGroupFrame->AddFrame(fgroupGraph, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsLeft, 10, 10, 0, 0));


	/****************************************************************************************************/
	/*** Measures Group *********************************************************************************/
	/****************************************************************************************************/
	
	fgroupMeas = new TGGroupFrame(fLGroupFrame,"Measurements", kVerticalFrame);
	//fgroupMeas->SetTitlePos(TGGroupFrame::kRight); // right aligned

	// 2 column, n rows
	fgroupMeas->SetLayoutManager(new TGMatrixLayout(fgroupMeas, 0, 2, 10));

	// Create ComboBox Measurements
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Display:")));	

	fComboMeas = new TGComboBox(fgroupMeas,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  	fComboMeas->AddEntry("None", 0);
	fComboMeas->Select(0, kTRUE);
  	fgroupMeas->AddFrame(fComboMeas, new TGLayoutHints(kLHintsLeft | kLHintsTop ,0,0,0,0));
	fComboMeas->MoveResize(10,24,100,20);

	//Create Events Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Events:")));	
	fNumEvents = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESInteger, 
						TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumEvents->SetIntNumber(0);
	//fNumEvents->SetFormat(TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive);
	fNumEvents->SetAlignment(/*ETextJustification::*/kTextLeft);
	fNumEvents->Resize(85, 20/*fNumEvents->GetDefaultHeight()*/);
	fNumEvents->SetEnabled(kFALSE);
	fNumEvents->SetToolTipText("Mannnnnnyyyyyyyy Events!");
  	fgroupMeas->AddFrame(fNumEvents);
	

	//Create Time Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Time(s):")));	
	fNumTime = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESInteger, 
					TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumTime->SetIntNumber(10);
	fNumTime->SetAlignment(kTextLeft);
	fNumTime->SetEnabled(kFALSE);
	fNumTime->Resize(85, 20/*fNumEvents->GetDefaultHeight()*/);
	fNumTime->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumTime);

		
	//Create Baseline Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Baseline(mV):")));	
	fNumBaseline = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumBaseline->SetIntNumber(10);
	fNumBaseline->SetAlignment(kTextLeft);
	fNumBaseline->Resize(85,20);
	fNumBaseline->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumBaseline);
	
	
	//Create Amplitude Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Amplitude(mV):")));	
	fNumAmplitude = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumAmplitude->SetIntNumber(10);
	fNumAmplitude->SetAlignment(kTextLeft);
	fNumAmplitude->Resize(85,20);
	fNumAmplitude->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumAmplitude);

	//Create Charge Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Charge(pC):")));	
	fNumCharge = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumCharge->SetIntNumber(10);
	fNumCharge->SetAlignment(kTextLeft);
	fNumCharge->Resize(85,20);
	fNumCharge->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumCharge);

	
	//Create Counter Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Counter:")));	
	fNumCounter = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESInteger, 
						TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumEvents->SetIntNumber(0);
	//fNumEvents->SetFormat(TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive);
	fNumCounter->SetAlignment(kTextLeft);
	fNumCounter->Resize(85,20);
	fNumCounter->SetEnabled(kFALSE);
	fNumCounter->SetToolTipText("Mannnnnnyyyyyyyy Counts!");
  	fgroupMeas->AddFrame(fNumCounter);

	//Create Count Rate Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Count Rate(1/s):")));	
	fNumCountRate = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumCountRate->SetIntNumber(10);
	fNumCountRate->SetAlignment(kTextLeft);
	fNumCountRate->Resize(85,20);
	fNumCountRate->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumCountRate);

	//Create RX Rate Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("RX Rate(KB/s):")));	
	fNumRxRate = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumEvents->SetIntNumber(0);
	//fNumEvents->SetFormat(TGNumberFormat::kNESInteger,TGNumberFormat::kNEAPositive);
	fNumRxRate->SetAlignment(kTextLeft);
	fNumRxRate->Resize(85,20);
	fNumRxRate->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumRxRate);

	
	//MoveResize(x, y, width, height)
	//fgroupMeas->SetLayoutManager(new TGVerticalLayout(fgroupMeas));
	//fgroupMeas->Resize();

	fLGroupFrame->AddFrame(fgroupMeas, new TGLayoutHints(kLHintsTop | kLHintsExpandY | kLHintsLeft, 10, 10, 0, 10));

	/****************************************************************************************************/
	/*** MiddleFrame: LGroupFrame: END  *****************************************************************/
	/****************************************************************************************************/
	fMiddleFrame->AddFrame(fLGroupFrame, new TGLayoutHints(kLHintsTop | /*kLHintsExpandX |*/ kLHintsExpandY, 0, 0, 0, 0));

	/****************************************************************************************************/
	/*** MiddleFrame: RGroupFrame ***********************************************************************/
	/****************************************************************************************************/
	fRGroupFrame = new TGCompositeFrame(fMiddleFrame, 60, 20, kVerticalFrame |
                                                      kSunkenFrame);

	/****************************************************************************************************/
	/*** Trigger Group **********************************************************************************/
	/****************************************************************************************************/
   	fgroupTrigg = new TGGroupFrame(fRGroupFrame,"Trigger", kVerticalFrame);

	// 2 column, n rows
	fgroupTrigg->SetLayoutManager(new TGMatrixLayout(fgroupTrigg, 0, 2, 10));

	// Create ComboBox Trigger Mode
	fgroupTrigg->AddFrame(new TGLabel(fgroupTrigg, new TGHotString("Mode:")));	
  	fComboTri = new TGComboBox(fgroupTrigg,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
  	fComboTri->AddEntry("External", 0);
  	fComboTri->AddEntry("Internal", 1);
  	//fComboTri->AddEntry("Frequency", 2);
  	fComboTri->Select(0, kTRUE);
  	fComboTri->Resize(100,20);
	fComboTri->Connect("Selected(Int_t)","MainFrame",this,"HandleComboTri()");
  	fgroupTrigg->AddFrame(fComboTri);

	// Create ComboBox Trigger From
	fgroupTrigg->AddFrame(new TGLabel(fgroupTrigg, new TGHotString("From:")));	
  	fComboFrom = new TGComboBox(fgroupTrigg,-1,kHorizontalFrame | kSunkenFrame | kDoubleBorder | kOwnBackground);
	fComboFrom->AddEntry("None",0);
	fComboFrom->Select(0, kTRUE);	
	fComboFrom->SetEnabled(kFALSE);
  	fComboFrom->Resize(100,20);
	fgroupTrigg->AddFrame(fComboFrom);


	//Create Treshold NumerEntry
	fgroupTrigg->AddFrame(new TGLabel(fgroupTrigg, new TGHotString("Threshold(mV):")));	
	//fNumTreshold = new TGNumberEntry(0, value, digit width, id, numberformat
	fNumTreshold = new TGNumberEntry(fgroupTrigg,500,4,-1,TGNumberFormat::kNESReal,
						TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,-750.0,750.0);
	//fNumTreshold->SetIntNumber(10);
	fNumTreshold->Resize(55,20);
	fNumTreshold->SetState(kFALSE);
	//fNumTreshold->SetState(kTRUE);
	fgroupTrigg->AddFrame(fNumTreshold);

  	//Slope Button and Label
	fgroupTrigg->AddFrame(new TGLabel(fgroupTrigg, new TGHotString("Slope:")));
  	//fPButtonSlope = new TGPictureButton(fgroupTrigg,gClient->GetPicture("./icons/slopeup.png"));
  	fPButtonSlope = new TGTextButton(fgroupTrigg,"P");
   	fPButtonSlope->Resize(20,20);
   	fPButtonSlope->Connect("Clicked()","MainFrame",this,"FButtonSlope()");
	fPButtonSlope->SetEnabled(kTRUE);
   	fgroupTrigg->AddFrame(fPButtonSlope);

	//Apply Button 
  	//fButtonApply = new TGTextButton(fgroupTrigg,"Apply");
  	//fButtonApply->Connect("Clicked()","MainFrame",this,"FButtonConfig()");
  	//fButtonApply->SetEnabled(kFALSE);
  	//fButtonApply->Resize(50,20);
   	//fgroupTrigg->AddFrame(fButtonApply);
	
  	fRGroupFrame->AddFrame(fgroupTrigg, new TGLayoutHints(kLHintsLeft | kLHintsTop,10,10,0,0));
	

	/****************************************************************************************************/
	/*** Control Group **********************************************************************************/
	/****************************************************************************************************/
   	fgroupCtrl = new TGGroupFrame(fRGroupFrame,"Control", kVerticalFrame);

	//2 column, n rows
	fgroupCtrl->SetLayoutManager(new TGMatrixLayout(fgroupCtrl, 0, 2, 10));

	//Create Count NumberEntry
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Count (#):")));	
	fCount = new TGNumberEntry(fgroupCtrl,0,8,-1,TGNumberFormat::kNESInteger,
						TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0,10000000);
  	fgroupCtrl->AddFrame(fCount);

	//Create Time NumberEntry
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Time (s):")));	
	fTime = new TGNumberEntry(fgroupCtrl,0,8,-1,TGNumberFormat::kNESInteger,
						TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0,10000000);
  	fgroupCtrl->AddFrame(fTime);

	//Create Time NumberEntry
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Interval (s):")));	
	fInterval = new TGNumberEntry(fgroupCtrl,0,8,-1,TGNumberFormat::kNESInteger,
						TGNumberFormat::kNEAAnyNumber,TGNumberFormat::kNELLimitMinMax,0,10000000);
  	fgroupCtrl->AddFrame(fInterval);

	//Run Button  
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Acquisition:")));
  	fButtonRunMPD1 = new TGTextButton(fgroupCtrl,"Run");
  	fButtonRunMPD1->Connect("Clicked()","MainFrame",this,"FButtonRunMPD1()");
  	fButtonRunMPD1->SetEnabled(kFALSE);
  	fButtonRunMPD1->Resize(40,25);
  	fgroupCtrl->AddFrame(fButtonRunMPD1);
	
	//Save Table
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Save Cal':")));
	fScal = new TGCheckButton(fgroupCtrl);
	fScal->SetState(kButtonUp);
	fScal->Connect("Clicked()", "MainFrame", this, "HandleCheckBoxes()");
  	fgroupCtrl->AddFrame(fScal);

	//Save CheckBox
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Save Waveform:")));
	fSave = new TGCheckButton(fgroupCtrl/*, new TGHotString("Save Waveform"), 14*/);
	//fSave->Resize(27,20);
	fSave->SetState(kButtonUp);
	fSave->Connect("Clicked()", "MainFrame", this, "HandleCheckBoxes()");
  	fgroupCtrl->AddFrame(fSave);
	

	//Save Table
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Save Table:")));
	fTable = new TGCheckButton(fgroupCtrl);
	fTable->SetState(kButtonUp);
	fTable->Connect("Clicked()", "MainFrame", this, "HandleCheckBoxes()");
  	fgroupCtrl->AddFrame(fTable);
	
	//Save Table
	fgroupCtrl->AddFrame(new TGLabel(fgroupCtrl, new TGHotString("Save Counter:")));
	fCounter = new TGCheckButton(fgroupCtrl);
	fCounter->SetState(kButtonUp);
	fCounter->Connect("Clicked()", "MainFrame", this, "HandleCheckBoxes()");
  	fgroupCtrl->AddFrame(fCounter);

	fRGroupFrame->AddFrame(fgroupCtrl, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | 
										kLHintsExpandY | kLHintsTop,10,10,0,10));

	/****************************************************************************************************/
	/*** MiddleFrame: RGroupFrame: END  *****************************************************************/
	/****************************************************************************************************/
	fMiddleFrame->AddFrame(fRGroupFrame, new TGLayoutHints(kLHintsTop | /*kLHintsExpandX |*/ kLHintsExpandY, 0, 0, 0, 0));


	/****************************************************************************************************/
	/*** MiddleFrame: END  ******************************************************************************/
	/****************************************************************************************************/
	AddFrame(fMiddleFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));


	/****************************************************************************************************/
	/*** Status Bar  ************************************************************************************/
	/****************************************************************************************************/
  	fStatusBar = new TGStatusBar(this, 860, 16, kHorizontalFrame);
  	fStatusBar->SetParts(2);
  	fStatusBar->SetText("NOT Connected",0);

	AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 0, 0, 0, 0));

	/****************************************************************************************************/

	MapSubwindows(); // Map all subwindows of main frame
	Resize(); // Initialize the layout algorithm
	//CenterOnParent();
	MapWindow(); // Map main frame
	//Print();

	/**************************************************************************************************/
	/**************************************************************************************************/
	/**************************************************************************************************/
	/**************************************************************************************************/
	/**************************************************************************************************/
	/**************************************************************************************************/
	/**************************************************************************************************/
	//GraphsOpen = false;
	core = new Core();  
	setts = new Settings();

	//setts->Connect("SettingsChanged()", "MainFrame", "this", "SettingsUpdate()");
	//setts->SettingsChanged();
	
	//

	fStatusBar->SetText("Connecting...",0);

	int r = core->SetConnect(true);
	
	if(r == -1)
		fStatusBar->SetText("Already Connected! Something Wrong?!",0);
	else if (r == 1){
	  	fStatusBar->SetText("Connected!",0);
  		//fButtonApply->SetEnabled(kTRUE);
		core->Initialize();
		//new Thread();
		fButtonRunMPD1->SetEnabled(kTRUE);
	}else{
	  	fStatusBar->SetText("Connection Failed.",0);
		//fButtonApply->SetEnabled(kFALSE);
	 	fButtonRunMPD1->SetEnabled(kFALSE);
	}

	FILEDEBUG("\nLast Execution: %u. **************************************************\n", r, 0);

	SettingsUpdate();
}

// Destructor
MainFrame::~MainFrame()
{
	printf("\nDestructor!\n");
	DeleteWindow();
	Cleanup();
}

// Close Window
void MainFrame::CloseWindow()
{
	printf("\nCloseWindow()!\n");
	
	// Terminates the application.
	gApplication->Terminate();
	delete this;
}

void MainFrame::HandleMenu(Int_t id)
{
   // Handle menu items.
	switch (id) {
		
		case M_FILE_EXIT:
			CloseWindow();   // terminate theApp no need to use SendCloseMessage()
			break;
	  
		case M_SETTINGS_CHANNELS:
			fChannels = new fChannelsFrame(gClient->GetRoot(), this, 400, 200);
			fChannels->Connect("SettingsChanged()", "MainFrame", this, "SettingsUpdate()");
			break;
		
		case M_VIEW_GRAPHS:
			fGraphs = new fGraphsFrame(gClient->GetRoot(), this, 1000, 1000);
			break;

		default:
			printf("Menu item %d selected\n", id);
			break;
	}
}

void MainFrame::SettingsUpdate()
{
	unsigned char *chmap_ptr;
	unsigned char t_channels = 0;
	char cstr[11];
	char bstr[160]; //8*20
	char *bptr = &bstr[0];
	NDAQ_CONFIG config;

	printf("Channel Config: %u\n", setts->GetChanConfig());

	//Calculates t_blocks! MUST be the FIRST to be performed.
	core->MakeConfig(setts->GetChanConfig(), &config);
	core->Config(config);
	
	printf("Total Blocks: %u\n", core->GetTBlocks());

	//Mapping Channels
	t_channels = core->MapChannels(setts->GetChanConfig());

	printf("Total Channels: %u\n", t_channels);

	//Populating 'Graphic From' and 'Measurements' ComboBox
	fComboGraph->RemoveAll();
	fComboMeas->RemoveAll();
  	fComboGraph->AddEntry("None", 0);
  	fComboMeas->AddEntry("None", 0);

	chmap_ptr = core->GetChMap();
	for(unsigned char i=0;i<core->GetTBlocks();i++)
	{	
		if (*chmap_ptr > 0)
		{
			sprintf(cstr, "From Ch%u", *chmap_ptr);
			fComboGraph->AddEntry(cstr, *chmap_ptr);
			fComboMeas->AddEntry(cstr, *chmap_ptr);
		}
		chmap_ptr++;

		if (*chmap_ptr > 0)
		{
			sprintf(cstr, "From Ch%u", *chmap_ptr);
			fComboGraph->AddEntry(cstr, *chmap_ptr);
			fComboMeas->AddEntry(cstr, *chmap_ptr);
		}
		chmap_ptr++;
	}

	//fComboGraph->Select(1, kTRUE);
	fComboGraph->Select(0, kTRUE); //Antes selecionava grafico 'From Ch1', agora seleciona 'None'
	fComboGraph->MapSubwindows();
	fComboGraph->Layout();
	fComboMeas->Select(0, kTRUE); //Antes selecionava grafico 'From Ch1', agora seleciona 'None'
	fComboMeas->MapSubwindows();
	fComboMeas->Layout();
	
	//Populating 'Trigger From' ComboBox
	fComboFrom->RemoveAll();

	chmap_ptr = core->GetChMap();
	for(unsigned char i=0;i<core->GetTBlocks();i++)
	{	
		if (*chmap_ptr > 0)
		{
			sprintf(cstr, "Ch%u", *chmap_ptr);
			fComboFrom->AddEntry(cstr, *chmap_ptr-1);
		}
		chmap_ptr++;

		if (*chmap_ptr > 0)
		{
			sprintf(cstr, "Ch%u", *chmap_ptr);
			fComboFrom->AddEntry(cstr, *chmap_ptr-1);
		}
		chmap_ptr++;
	}

	fComboFrom->Select(*core->GetChMap()-1, kTRUE);
	fComboFrom->MapSubwindows();
	fComboFrom->Layout();

	//Setting Filenames
	SetFilename(setts->GetChanConfig(), bstr, filename, suffix);
	for(unsigned char i=0;i<t_channels;i++)
	{
		printf("%s\n", bptr);
		bptr+=(strlen(bptr)+1);
	}
}

void MainFrame::HandleComboTri()
{

	if (fComboTri->GetSelected() == 2){
		fComboFrom->SetEnabled(kTRUE);
		fNumTreshold->SetState(kTRUE);
		fPButtonSlope->SetEnabled(kTRUE);
	}else 
	if(fComboTri->GetSelected() == 1){
		fComboFrom->SetEnabled(kTRUE);
		fNumTreshold->SetState(kTRUE);
		fPButtonSlope->SetEnabled(kTRUE);
	}else{
		fComboFrom->SetEnabled(kFALSE);
		fNumTreshold->SetState(kFALSE);
		fPButtonSlope->SetEnabled(kTRUE);
	} 
}

void MainFrame::FButtonSlope()
{
	if(fbslope){
		fPButtonSlope->SetText("N");
		fbslope = false;
	}else{
		fPButtonSlope->SetText("P");
		fbslope = true;
	}
}	

void MainFrame::FButtonConfig()
{
	//
}

void MainFrame::FButtonRunMPD1()
{
	Double_t th=0;

	//Stop
	//If 'running', the only way is stop it.
	if  (core->GetRun()){
		//printf("\nStop!\n");		
		//Toggle Running!
		core->ToggleRun();
		fButtonRunMPD1->SetText("Run");
		running=false;
		saving=false;
	}
	//Start
	//If 'not running', the only way is start it.
	else{
		
		printf("Start!\n");

		SetFilename(setts->GetChanConfig(), namevector, filename, suffix);
		totalEvents=0;
		save_count=0;
		fNumEvents->SetIntNumber(totalEvents);
		fButtonRunMPD1->SetText("Stop");
	
		//th = fNumTreshold->GetNumber()*0.3859f+1.576f;
		th = fNumTreshold->GetNumber();
		
		switch(fComboTri->GetSelected())
		{	
			//External
			case 0: core->SetTrigger(true, fbslope, (signed short) th, fComboFrom->GetSelected()); break;
			//Internal			
			case 1: core->SetTrigger(false, fbslope, (signed short) th, fComboFrom->GetSelected()); break;
			//Frequency			
			case 2: core->SetTrigger(false, fbslope, (signed short) th, fComboFrom->GetSelected()); break;
		}
		
		//Configure Channels
		core->SetChannels(setts->GetChanConfig());
		//Toggle Running!
		core->ToggleRun();
		//printf("\nt_zero!\n");
		//t_zero = (int)time(NULL);
		running = true;
		zero_seek = true;
		timeP = timeA = timer = 0;
		last_timestamp = 0;
	}
	
}

void MainFrame::HandleCheckBoxes()
{
	//
}

bool MainFrame::Update(){
	
	unsigned char Buffer[BUFFER];

	unsigned short block_size=0;
	unsigned short event_count=0;

	unsigned int header=0;
	unsigned int timestamp=0;
	unsigned int cntr=0;

	signed int x[ADC_SAMPLES];
	signed int y[ADC_SAMPLES];

	double ftemp=0;
	double gtemp=0;

	unsigned int itime=0;
	unsigned int stime=0;
	
	unsigned int ltimer=0;

	signed short peak = 0;
	signed int acc = 0;
	double baseline = 0;
	double amplitude = 0;

	/********************************************************************************************/
	
	//Checking for  Data Block...
	if (core->GetRun() && running == true) block_size = core->Acq(Buffer);
	
	//If the Data Block has enough data...
	if ( block_size > (core->GetBS()-1) ) 
	{	
		if (DEBUG) printf(" - Read Size: %u\n\n", block_size);
		
		/********************************************/
		/*** Checking Header for THIS (one) block ***/
		/********************************************/

		GetDWORD(HEADER_OFFSET, HEADER_SIZE, Buffer, CopyData, &header);
		
		//if header value is wrong...
		if (header != 0xAA55AA55)
		{
			printf("Header ERROR!\n\n");
			FILEDEBUG("Header Error - 0x%08X\n", header, 0);
			return true;			//descarta este bloco de dados.
		}
		
		/**************************************************/
		/*** Getting the timestamp value for this block ***/
		/**************************************************/

		GetDWORD(TIMESTAMP_OFFSET, TIMESTAMP_SIZE, Buffer, CopyData, &timestamp);
		
		//If there was NO new timestamp, keep last timestamp.
		if (timestamp == 0xFFFFFFFF)
			timestamp = last_timestamp;

		//Saving timestamp;
		last_timestamp = timestamp;

		//temp for calculation.
		ftemp = timestamp*0.1;	//0.1 because the timebase is 100ms and time unit is seconds.
		itime = (unsigned int) ftemp;	//internal time - absolute time generated on CORE FPGA.

		//ZERO SEEK!
		if (zero_seek == true)
		{			
			if (itime != 0)
			{
				zscounter++;
				bscounter=bscounter+block_size;
				if (DEBUG) {printf("ZERO Seek ERROR!\n\n"); _getch();}
				return true;		//descarta este bloco de dados e mantem a procura pelo ZERO.
			}
			FILEDEBUG("ZSK: %u - BS.acc: %u.\n", zscounter, bscounter);
			zero_seek = false;
			zscounter=0;
			bscounter=0;
		}

		//Time Dif and Save Interval stuff
		timeP = timeA;
		timeA = timestamp;
		timer += (timeA - timeP);
		
		//temp for calculation.
		ftemp = timer*0.1;	//timer increments on a 100ms basis and ltimer must increment on a second basis.
		
		//ltimer is the Save Interval timer.
		ltimer = (unsigned int) ftemp;
		//printf("Time Dif: %08u\n", ltimer);
		if ((fInterval->GetIntNumber() > 0) && (ltimer == fInterval->GetIntNumber()))
		{
			printf("Timer : %08u\n", ltimer);
			timer = 0;
			sIntervalEn = true;
		}
		if (fInterval->GetIntNumber() == 0) timer = 0;
		
		//Time
		fNumTime->SetIntNumber(itime);
		//printf("itime: %u\n\n", itime);

		/********************************************************************************************/

		//SAVE TIME CHECK
		if((fScal->GetState() != kButtonUp) || (fSave->GetState() != kButtonUp) || (fTable->GetState() != kButtonUp)
			|| (fCounter->GetState() != kButtonUp)){
			if(saving==false)
			{
				saving=true;
				start_time = itime;
			}
			stime = itime-start_time;
			//printf("sv: %u - it: %u - st: %u\n\n", stime, itime, start_time);
		}else
			saving=false;

		//For 3 seconds save test:
		#ifdef SAVE3
			if ((stime > 3) && (fTime->GetIntNumber() == 3) && (fTable->GetState() != kButtonUp))
			{
				FILEDEBUG("Timestamp Error - %u!\n", timestamp, 0);
			}
		#endif

		//Test Save Count Limit
		if ((stime >= (unsigned int) fTime->GetIntNumber()) && (fTime->GetIntNumber() > 0) && (saving == true)){
			start_time = 0;
			saving=false;
			printf("\ntime: %0.6u\n", stime);
			FButtonRunMPD1();
			#ifdef SAVE3
				while(core->GetRun() == true);
				Sleep(500);
				FButtonRunMPD1();
			#endif
		}

		/********************************************************************************************/
		/*
		// Counter Debug
		for (unsigned short i=0; i<528; i+=132)
		{
			//Timestamp
			GetDWORD(i+1, 1, (unsigned char*)Buffer, CopyData, &timestamp);
			printf("Timestamp ChA&B:%u\n", timestamp);
			
			//Counter A
			GetDWORD(i+130, 1, Buffer, CopyData, &cntr);
			if (timestamp > 0)
				printf("Counter ChA:%u - Frequency: %08u\n", cntr, (cntr/timestamp)*10);
			//Counter B
			GetDWORD(i+131, 1, Buffer, CopyData, &cntr);
			if (timestamp > 0)
				printf("Counter ChB:%u - Frequency: %08u\n", cntr, (cntr/timestamp)*10);
		}

		printf("*******************************\n");
		printf("*******************************\n");
		*/
		/****************************************************************************************/

		event_count = block_size/(FIFO_BS*SLOT_SIZE*core->GetTBlocks());

		totalEvents=totalEvents+event_count;
		
		//Events
		fNumEvents->SetIntNumber(totalEvents);
		
		//Rx Rate *** PROBABLY WRONG ! ! ! CHECK LATER ! ! ! ***
		if (itime > 0)
			//fNumRxRate->SetNumber((Double_t) ((block_size/_D(TIMESTAMP_)/1024) );
		
		//Graphic
		if(fComboGraph->GetSelected() > 0){
					
			/****************************************/
			for(unsigned char i=0;i<128;i++) x[i] = i;
			/****************************************/
			
			unsigned char *ChMap = core->GetChMap();

			for (unsigned char c=0;c<core->GetTBlocks();c++)
			{
				//Even
				if (*ChMap++ == fComboGraph->GetSelected())
				{
					//Waveform
					GetLSWORD(ADC_OFFSET+(c*FIFO_BS), ADC_SIZE, Buffer, GraphData, y);					
				}

				//Odd
				if (*ChMap++ == fComboGraph->GetSelected())
				{
					GetMSWORD(ADC_OFFSET+(c*FIFO_BS), ADC_SIZE, Buffer, GraphData, y);
				}
			}
			
			//Initialize graph1
			fEcanvas1->GetCanvas()->cd();
			//if(graph1) delete graph1;
			
			graph1 = new TGraph(ADC_SAMPLES, x, y);
			graph1->SetTitle("Event");
			graph1->SetEditable(kFALSE);
			//graph1->SetMarkerStyle(kFullDotMedium);
			//graph1->SetMarkerSize(1);
			//graph1->SetMarkerColor(kRed);
			graph1->SetLineColor(kRed);
			graph1->GetYaxis()->SetRangeUser(-600, 600);
			graph1->GetXaxis()->SetRangeUser(0, (ADC_SAMPLES)); //*TIMEBIN				
			graph1->Draw("AL");			

			fEcanvas1->GetCanvas()->Update();
			
		}
		
		//MEASUREMENTS
		if(fComboMeas->GetSelected() > 0)
		{
			//Getting Information...
			unsigned char *ChMap = core->GetChMap();

			for (unsigned char c=0;c<core->GetTBlocks();c++)
			{
				//Even
				if (*ChMap++ == fComboMeas->GetSelected())
				{
					//Peak:
					//1) We will find the waveform's NEGATIVE or POSITIVE peak value starting at PK_OFFSET.
					if (fbslope == false)
					{	
						peak = 32767;
						GetLSWORD(ADC_OFFSET+PK_OFFSET+(c*FIFO_BS), PK_INTERVAL, Buffer, GetNPeak, &peak);
					}
					else
					{
						peak = -32768;
						GetLSWORD(ADC_OFFSET+PK_OFFSET+(c*FIFO_BS), PK_INTERVAL, Buffer, GetPPeak, &peak);
					}
					//2) We have to consider ADC calibration for the final value.
					peak = peak*A+B;
					
					//Baseline:
					//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
					GetLSWORD(ADC_OFFSET+BASE_OFFSET+(c*FIFO_BS), BASE_INTEGRAL, Buffer, GetSum, &acc);
					//2) 'baseline' is the mean value of the above sumatorie.
					baseline = acc / BASE_INTEGRAL;
					//3) We have to consider ADC calibration for the final value.
					baseline = baseline*A+B;

					//Even Counter
					GetDWORD(ACOUNTER_OFFSET+(c*FIFO_BS), COUNTER_SIZE, Buffer, CopyData, &cntr);
				}

				//Odd
				if (*ChMap++ == fComboMeas->GetSelected())
				{
					//Peak:
					//1) We will find the waveform's NEGATIVE or POSITIVE peak value starting at PK_OFFSET.
					if (fbslope == false)
					{	
						peak = 32767;
						GetMSWORD(ADC_OFFSET+PK_OFFSET+(c*FIFO_BS), PK_INTERVAL, Buffer, GetNPeak, &peak);
					}
					else
					{
						peak = -32768;
						GetMSWORD(ADC_OFFSET+PK_OFFSET+(c*FIFO_BS), PK_INTERVAL, Buffer, GetPPeak, &peak);
					}
					//2) We have to consider ADC calibration for the final value.
					peak = peak*A+B;
					
					//Baseline:
					//1) 'acc' is the sumatorie of BASE_INTEGRAL ADC samples starting at BASE_OFFSET.
					GetMSWORD(ADC_OFFSET+BASE_OFFSET+(c*FIFO_BS), BASE_INTEGRAL, Buffer, GetSum, &acc);
					//2) 'baseline' is the mean value of the above sumatorie.
					baseline = acc / BASE_INTEGRAL;
					//3) We have to consider ADC calibration for the final value.
					baseline = baseline*A+B;
					
					//Event Counter
					GetDWORD(BCOUNTER_OFFSET+(c*FIFO_BS), COUNTER_SIZE, Buffer, CopyData, &cntr);
				}
			}

			//Baseline
			fNumBaseline->SetNumber(baseline);
			
			//Amplitude: It's the Peak - Baseline.
			fNumAmplitude->SetNumber(peak - (baseline));
			
			//Charge
			//fNumCharge->SetNumber(get_charge(EVENT_SIZE,base,yCAL));
			
			//Counter
			fNumCounter->SetIntNumber(cntr);
			
			//Count Rate
			if (timestamp > 0)
			{
				ftemp = cntr;
				gtemp = timestamp;
				ftemp = (ftemp/gtemp);
				fNumCountRate->SetNumber(ftemp*10);
			}
		}
		

		//SAVE CAL
		if((fScal->GetState() != kButtonUp) && ((fInterval->GetIntNumber() == 0) || (sIntervalEn == true))){
			save_count+= SaveCal(core->GetTBlocks(), core->GetChMap(), namevector, block_size, Buffer)/core->GetTChannels();
			sIntervalEn = false;
		}

		//SAVE WAVE
		if((fSave->GetState() != kButtonUp) && ((fInterval->GetIntNumber() == 0) || (sIntervalEn == true))){
			save_count += SaveWave(core->GetTBlocks(), core->GetChMap(), namevector, block_size, Buffer)/core->GetTChannels();
			sIntervalEn = false;
		}

		//SAVE TABLE
		if((fTable->GetState() != kButtonUp) && ((fInterval->GetIntNumber() == 0) || (sIntervalEn == true))){
			
			//If SAVE 3 seconds test is defined, Save Table should do NOTHING.
			#ifndef SAVE3
				if (fbslope == false)			
					save_count+= SaveNTable(core->GetTBlocks(), core->GetChMap(), namevector, block_size, Buffer)/core->GetTChannels();
				else
					save_count+= SavePTable(core->GetTBlocks(), core->GetChMap(), namevector, block_size, Buffer)/core->GetTChannels();
			#endif
			sIntervalEn = false;
		}

		//SAVE COUNTER
		if((fCounter->GetState() != kButtonUp) && ((fInterval->GetIntNumber() == 0) || (sIntervalEn == true))){
			save_count+= SaveCounter(core->GetTBlocks(), core->GetChMap(), namevector, block_size, Buffer)/core->GetTChannels();
			sIntervalEn = false;
		}

		//Test Save Count Limit
		if ((save_count >= (unsigned long)fCount->GetIntNumber()) && (fCount->GetIntNumber() > 0)){
			printf("\nsave_count: %0.6u\n", save_count);
			save_count = 0;
			FButtonRunMPD1();
		}

		return true;
	}
	
	return false;
}
