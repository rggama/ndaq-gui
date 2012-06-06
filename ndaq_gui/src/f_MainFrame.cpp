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

char filename[] = "Filename";
char suffix[] = "Ch";
char namevector[280];

//

// *** HAD TO MAKE THESE VARS GLOBALS, IN ORDER TO THE SIGNAL TRIGGERED METHOD SettingsUpdate()
// *** WORK PROPERLY. WHEN THESE VARS ARE DECLARED IN THE CLASS STATEMENT, THE REFERED METHOD
// *** IS NOT ABLE TO ACCESS THEM. W H Y !?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!
TGComboBox *fComboGraph;
TGComboBox *fComboFrom;

//unsigned long int totalEvents=0;
Long_t totalEvents=0;
unsigned long cal_count=0;
int t_zero=0;
UInt_t dcounter=0;
UInt_t ldcounter=0;

unsigned char graph_counter=1;
bool running=false;
bool saving=false;
int start_time=0;

MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h) : TGMainFrame(p, w, h)
{	
	fbslope = true;
	saveWave = false;
	saveTable = false;

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

	//Create Count Rate Number Display
	fgroupMeas->AddFrame(new TGLabel(fgroupMeas, new TGHotString("Count Rate(1/s):")));	
	fNumCountRate = new TGNumberEntryField(fgroupMeas, -1, 0, TGNumberFormat::kNESRealTwo, 
						TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELNoLimits, 0, 1);
	//fNumCountRate->SetIntNumber(10);
	fNumCountRate->SetAlignment(kTextLeft);
	fNumCountRate->Resize(85,20);
	fNumCountRate->SetEnabled(kFALSE);
  	fgroupMeas->AddFrame(fNumCountRate);
	
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
  	fComboTri->AddEntry("Frequency", 2);
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
	fNumTreshold = new TGNumberEntry(fgroupTrigg,10.5,4,-1,TGNumberFormat::kNESReal,
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
	
	SettingsUpdate();

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
	unsigned char channel[8];
	char cstr[9];
	char bstr[160]; //8*20
	char *bptr = &bstr[0];

	printf("Channel Config: %u\n", setts->GetChanConfig());

	//Mapping Channels
	int on_channels = core->MapChannels(setts->GetChanConfig(), channel);

	printf("On Channels %u\n", on_channels);

	//Populating 'Graphic From' ComboBox
	fComboGraph->RemoveAll();
  	fComboGraph->AddEntry("None", 0);
	for(unsigned char i=0;i<8;i++)
		if(channel[i] > 0){
			sprintf(cstr, "From Ch%u", i+1);
	 		fComboGraph->AddEntry(cstr, channel[i]);
		}
	fComboGraph->Select(1, kTRUE);
	fComboGraph->MapSubwindows();
	fComboGraph->Layout();
	
	//Populating 'Trigger From' ComboBox
	fComboFrom->RemoveAll();
	for(unsigned char i=0;i<8;i++)
		if(channel[i] > 0){
			sprintf(cstr, "Ch%u", i+1);
	 		fComboFrom->AddEntry(cstr, i);
		}

	for(unsigned char i=0;i<8;i++)
		if(channel[i] > 0){
			fComboFrom->Select(i, kTRUE);
			break;
		}
	fComboFrom->MapSubwindows();
	fComboFrom->Layout();

	//Setting Filenames
	SetFilename(setts->GetChanConfig(), bstr, filename, suffix);
	for(unsigned char i=0;i<on_channels;i++)
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
	}
	//Start
	//If 'not running', the only way is start it.
	else{
		SetFilename(setts->GetChanConfig(), namevector, filename, suffix);
		totalEvents=0;
		cal_count=0;
		fNumEvents->SetIntNumber(totalEvents);
		fButtonRunMPD1->SetText("Stop");
	
		th = fNumTreshold->GetNumber()*0.3859f+1.576f;
		
		switch(fComboTri->GetSelected())
		{	
			//External
			case 0: core->SetTrigger(true, fbslope, (signed short) th); break;
			//Internal			
			case 1: core->SetTrigger(false, fbslope, (signed short) th); break;
			//Frequency			
			case 2: core->SetTrigger(false, fbslope, (signed short) th); break;
		}
		
		//Configure Channels
		core->SetChannels(setts->GetChanConfig());
		//Toggle Running!
		core->ToggleRun();
		//printf("\nt_zero!\n");
		t_zero = (int)time(NULL);
		running = true;
	}
	
}

void MainFrame::HandleCheckBoxes()
{
	//
}

bool MainFrame::Update(){
	
	//H -> 8 Most signifcant bits, INCLUDING SIGNAL BIT (The MSb)
	//L -> 4 Least signifcant bits, padded with FOUR ZEROS.
	//
	//Constructing a 12 bits UNSIGNED number:
	//
	//N(signed int) = H(signed char)*2^4+L(signed char)
	//
	//Converting the 12 bits UNSIGNED number to a SIGNED one:
	//
	//N(signed int) = N << 4
	//
	//By shifting left N four times, we are moving the signal bit to the right place on a signed int type.
	//Now, N is signed but 16 times greater. So, letŽs move it back to its original magnitude:
	//
	//N(signed int) = N >> 4
	//
	//ThatŽs all, folks.

	signed char Buffer[BUFFER];
	signed char *ptr = &Buffer[0];

	//signed char *Buffer;
	unsigned int block_size=0;
	unsigned int event_count=0;

	//float x[EVENT_SIZE]/*, yCAL[EVENT_SIZE]*/;
	Int_t x[EVENT_SIZE];
	Int_t y[EVENT_SIZE];

	Int_t x1[EVENT_SIZE];
	Int_t y1[EVENT_SIZE];

	//float base = 0;
	unsigned int residue=0;
	int etime=0;
	int stime=0;

	unsigned int i,j;

	char ch[4];

	TCOUNTER_DATA	counter;
	unsigned int	timestamp;

	/********************************************************************************************/

	if (core->GetRun() && running == true)
	{
		etime = ((int)time(NULL)-t_zero);
		fNumTime->SetIntNumber(etime);

		//SAVE TIME CHECK
		if((fScal->GetState() != kButtonUp) || (fSave->GetState() != kButtonUp) || (fTable->GetState() != kButtonUp)){
			if(saving==false)
			{
				saving=true;
				start_time = etime;
			}
			stime = etime-start_time;
			//printf("sv: %u - e: %u - st: %u - z: %u\n", stime, etime, start_time, t_zero);
		}else
			saving=false;

		//Test Save Count Limit
		if ((stime >= fTime->GetIntNumber()) && (fTime->GetIntNumber() > 0) && saving == true){
			start_time = 0;
			printf("\ntime: %0.6u\n", stime);
			FButtonRunMPD1();
			Sleep(250);
			FButtonRunMPD1();
		}

	}

	/********************************************************************************************/

	//if (core->GetRun()) core->TestCoreRW();
	//if (core->GetRun()) core->Loopback();
	if (core->GetRun() && running == true) block_size = core->Acq((unsigned char *)Buffer);
	
	if ( block_size > BLOCK_SIZE-1 ) {
		
		printf(" - Read Size: %u\n\n", block_size);
		//_getch();

		
		//Timestamp Channel 1 & 2
		counter.b0 = (unsigned char)*(ptr+0+0);
		counter.b1 = (unsigned char)*(ptr+0+1);
		counter.b2 = (unsigned char)*(ptr+0+2);
		counter.b3 = (unsigned char)*(ptr+0+3);

		printf("Timestamp Ch1&2:%u\n", timestamp = counter.wdata);

		//Counter Channel 1
		counter.b0 = (unsigned char)*(ptr+4+0);
		counter.b1 = (unsigned char)*(ptr+4+1);
		counter.b2 = (unsigned char)*(ptr+4+2);
		counter.b3 = (unsigned char)*(ptr+4+3);
		
		if (timestamp > 0)
			printf("Counter Ch1:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);
			
		//Counter Channel 2
		counter.b0 = (unsigned char)*(ptr+8+0);
		counter.b1 = (unsigned char)*(ptr+8+1);
		counter.b2 = (unsigned char)*(ptr+8+2);
		counter.b3 = (unsigned char)*(ptr+8+3);

		if (timestamp > 0)
			printf("Counter Ch2:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Timestamp Channel 3 & 4
		counter.b0 = (unsigned char)*(ptr+12+0);
		counter.b1 = (unsigned char)*(ptr+12+1);
		counter.b2 = (unsigned char)*(ptr+12+2);
		counter.b3 = (unsigned char)*(ptr+12+3);

		printf("Timestamp Ch3&4:%u\n", timestamp = counter.wdata);

		//Counter Channel 3
		counter.b0 = (unsigned char)*(ptr+16+0);
		counter.b1 = (unsigned char)*(ptr+16+1);
		counter.b2 = (unsigned char)*(ptr+16+2);
		counter.b3 = (unsigned char)*(ptr+16+3);
		
		if (timestamp > 0)
			printf("Counter Ch3:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Counter Channel 4
		counter.b0 = (unsigned char)*(ptr+20+0);
		counter.b1 = (unsigned char)*(ptr+20+1);
		counter.b2 = (unsigned char)*(ptr+20+2);
		counter.b3 = (unsigned char)*(ptr+20+3);

		if (timestamp > 0)
			printf("Counter Ch4:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Timestamp Channel 5 & 6
		counter.b0 = (unsigned char)*(ptr+24+0);
		counter.b1 = (unsigned char)*(ptr+24+1);
		counter.b2 = (unsigned char)*(ptr+24+2);
		counter.b3 = (unsigned char)*(ptr+24+3);

		printf("Timestamp Ch5&6:%u\n", timestamp = counter.wdata);

		//Counter Channel 5
		counter.b0 = (unsigned char)*(ptr+28+0);
		counter.b1 = (unsigned char)*(ptr+28+1);
		counter.b2 = (unsigned char)*(ptr+28+2);
		counter.b3 = (unsigned char)*(ptr+28+3);
		
		if (timestamp > 0)
			printf("Counter Ch5:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Counter Channel 6
		counter.b0 = (unsigned char)*(ptr+32+0);
		counter.b1 = (unsigned char)*(ptr+32+1);
		counter.b2 = (unsigned char)*(ptr+32+2);
		counter.b3 = (unsigned char)*(ptr+32+3);

		if (timestamp > 0)
			printf("Counter Ch6:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Timestamp Channel 7 & 8
		counter.b0 = (unsigned char)*(ptr+36+0);
		counter.b1 = (unsigned char)*(ptr+36+1);
		counter.b2 = (unsigned char)*(ptr+36+2);
		counter.b3 = (unsigned char)*(ptr+36+3);

		printf("Timestamp Ch7&8:%u\n", timestamp = counter.wdata);

		//Counter Channel 7
		counter.b0 = (unsigned char)*(ptr+40+0);
		counter.b1 = (unsigned char)*(ptr+40+1);
		counter.b2 = (unsigned char)*(ptr+40+2);
		counter.b3 = (unsigned char)*(ptr+40+3);
		
		if (timestamp > 0)
			printf("Counter Ch7:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		//Counter Channel 8
		counter.b0 = (unsigned char)*(ptr+44+0);
		counter.b1 = (unsigned char)*(ptr+44+1);
		counter.b2 = (unsigned char)*(ptr+44+2);
		counter.b3 = (unsigned char)*(ptr+44+3);

		if (timestamp > 0)
			printf("Counter Ch8:%u - Frequency: %08u\n", counter.wdata, (counter.wdata/timestamp)*10);

		printf("*******************************\n");
		printf("*******************************\n");

		//_getch();
		
		/*	
		//Block Index - Increments in a FIFO block size basis: 512
		for (unsigned int block_i=0; block_i<block_size; block_i+=(12*4))
		{	
			//FIFO 1
			for (unsigned int data_i=0; data_i<12; data_i+=4)
			{
				printf(
					"%02X\t%02X\t%02X\t%02X\n", 
					(unsigned char)*(ptr+0),
					(unsigned char)*(ptr+1),
					(unsigned char)*(ptr+2),
					(unsigned char)*(ptr+3)
				);
				ptr+=4;
			}
			printf("*** FIFO (1) END\n\n");
			//_getch();
			//FIFO 2
			for (unsigned int data_i=0; data_i<12; data_i+=4)
			{
				printf(
					"%02X\t%02X\t%02X\t%02X\n", 
					(unsigned char)*(ptr+0),
					(unsigned char)*(ptr+1),
					(unsigned char)*(ptr+2),
					(unsigned char)*(ptr+3)
				);
				ptr+=4;
			}
			printf("*** FIFO (2) END\n\n");
			//_getch();
			//FIFO 3
			for (unsigned int data_i=0; data_i<12; data_i+=4)
			{
				printf(
					"%02X\t%02X\t%02X\t%02X\n", 
					(unsigned char)*(ptr+0),
					(unsigned char)*(ptr+1),
					(unsigned char)*(ptr+2),
					(unsigned char)*(ptr+3)
				);
				ptr+=4;
			}
			printf("*** FIFO (3) END\n\n");
			//_getch();
			//FIFO 4
			for (unsigned int data_i=0; data_i<12; data_i+=4)
			{
				printf(
					"%02X\t%02X\t%02X\t%02X\n", 
					(unsigned char)*(ptr+0),
					(unsigned char)*(ptr+1),
					(unsigned char)*(ptr+2),
					(unsigned char)*(ptr+3)
				);
				ptr+=4;
			}
			
			printf("*** FIFO (4) END\n\n");
			//_getch();
			
			//ptr+=(512*3);
		}
		*/

		event_count = block_size/(EVENT_SIZE*_step_);

		totalEvents=totalEvents+event_count;
		
		fNumEvents->SetIntNumber(totalEvents);

		if (etime > 0)
			fNumRxRate->SetNumber((Double_t) (((totalEvents*(EVENT_SIZE*_step_))/etime)/1024) );

		//fNumCounter->SetIntNumber(dcounter);

		if(fComboGraph->GetSelected() > 0){
			/*
			for (unsigned char index=0; index<128; index++)
			{
				printf("%u\n", (unsigned char)Buffer[index]);
				if ((index > 0) && (((unsigned char)Buffer[index]) != 0))
					if ( ((unsigned char)Buffer[index]) != ((unsigned char)Buffer[index-1]+1) )
					_getch();
			}*/
			
			if (1/*setts->GetGraphsOpen() == false*/){

				switch(fComboGraph->GetSelected()){
				//posicao
					case 1: T_intgraph(Buffer+0, x, y, 0); break;
					case 2: T_intgraph(Buffer+0, x, y, 2); break;
					case 3: T_intgraph(Buffer+520, x, y, 0); break;
					case 4: T_intgraph(Buffer+520, x, y, 2); break;
					case 5: T_intgraph(Buffer+1040, x, y, 0); break;
					case 6: T_intgraph(Buffer+1040, x, y, 2); break;
					case 7: T_intgraph(Buffer+1560, x, y, 0); break;
					case 8: T_intgraph(Buffer+1560, x, y, 2); break;
				}
								
				//Initialize graph1
 				fEcanvas1->GetCanvas()->cd();
 				//if(graph1) delete graph1;
				
				graph1 = new TGraph(EVENT_SIZE, x, y);
				graph1->SetTitle("Event");
				graph1->SetEditable(kFALSE);
				//graph1->SetMarkerStyle(kFullDotMedium);
				//graph1->SetMarkerSize(1);
				//graph1->SetMarkerColor(kRed);
				graph1->SetLineColor(kRed);
				graph1->GetYaxis()->SetRangeUser(-600, 600);
				graph1->GetXaxis()->SetRangeUser(0, (EVENT_SIZE/**TIMEBIN*/));				
				graph1->Draw("AL");			

				fEcanvas1->GetCanvas()->Update();
			}else{				
				switch(graph_counter){
				//posicao
					case 1: 
						T_intgraph(Buffer+0, x, y, 0); 
						fGraphs->fEcanvas1->GetCanvas()->cd(); 
						fGraphs->fEcanvas1->GetCanvas()->Update(); 
					break;
					case 2: 
						T_intgraph(Buffer+256, x, y, 0); 
						fGraphs->fEcanvas2->GetCanvas()->cd(); 
						fGraphs->fEcanvas2->GetCanvas()->Update(); 
					break;
					case 3: 
						T_intgraph(Buffer+512, x, y, 0); 
						fGraphs->fEcanvas3->GetCanvas()->cd(); 
						fGraphs->fEcanvas3->GetCanvas()->Update(); 
					break;
					case 4: 
						T_intgraph(Buffer+768, x, y, 0); 
						fGraphs->fEcanvas4->GetCanvas()->cd(); 
						fGraphs->fEcanvas4->GetCanvas()->Update(); 
					break;
					case 5: 
						T_intgraph(Buffer+1024, x, y, 0); 
						fGraphs->fEcanvas5->GetCanvas()->cd(); 
						fGraphs->fEcanvas5->GetCanvas()->Update(); 
					break;
					case 6: 
						T_intgraph(Buffer+1280, x, y, 0); 
						fGraphs->fEcanvas6->GetCanvas()->cd(); 
						fGraphs->fEcanvas6->GetCanvas()->Update(); 
					break;
					case 7: 
						T_intgraph(Buffer+1536, x, y, 0); 
						fGraphs->fEcanvas7->GetCanvas()->cd(); 
						fGraphs->fEcanvas7->GetCanvas()->Update(); 
					break;
					case 8: 
						T_intgraph(Buffer+1792, x, y, 0); 
						fGraphs->fEcanvas8->GetCanvas()->cd(); 
						fGraphs->fEcanvas8->GetCanvas()->Update(); 
					break;
				}

				//fEcanvas1->GetCanvas()->cd();

				//sprintf(ch, "%s %u", "Ch", graph_counter);
				//graph1 = new TGraph(EVENT_SIZE, x, y);
				//graph1->SetTitle(ch);
				//graph1->SetEditable(kFALSE);
				//graph1->SetLineColor(kRed);
				//graph1->GetYaxis()->SetRangeUser(-220, 50);
				//graph1->GetXaxis()->SetRangeUser(0, (EVENT_SIZE/**TIMEBIN*/));				
				//graph1->Draw("AL");

				graph_counter++;
				if (graph_counter == 9) graph_counter = 1;
			}

			/*
			if (setts->GetGraphsOpen()){
				printf("true\n");
				fGraphs->PlotGraph1(x, y);
			}else
				printf("false\n");
			*/

		}
		
		//MEASURES
		if(1){

			//fNumBaseline->SetNumber((base = get_baseline(yCAL)));
			//fNumAmplitude->SetNumber(get_amplitude(EVENT_SIZE,base,yCAL));
			//fNumCharge->SetNumber(get_charge(EVENT_SIZE,base,yCAL));
		}
		
		//SAVE CAL
		if(fScal->GetState() != kButtonUp){
			cal_count+= SaveCal(namevector, setts->GetChanTotal(), block_size, Buffer+1);
		}

		//SAVE WAVE
		if(fSave->GetState() != kButtonUp){
			SaveWave(namevector, setts->GetChanTotal(), block_size, Buffer+1);
		}

		//SAVE TABLE
		if(fTable->GetState() != kButtonUp){
			if (fbslope == false)			
				cal_count+= SaveNTable(namevector, setts->GetChanTotal(), block_size, Buffer+1);
			else
				cal_count+= SavePTable(namevector, setts->GetChanTotal(), block_size, Buffer+1);
		}

		//Test Save Count Limit
		if ((cal_count >= fCount->GetIntNumber()) && (fCount->GetIntNumber() > 0)){
			printf("\ncal_count: %0.6u\n", cal_count);
			cal_count = 0;
			FButtonRunMPD1();
		}

		return true;

	}
	
	return false;
}
