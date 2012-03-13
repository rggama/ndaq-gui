// header files required by the application.
//#include <stdlib.h>
//#include <stdio.h>
#include <time.h>
#include <string.h>
//#include <iostream>
//using namespace std;	

//#include <TQObject.h>
//#include <RQ_OBJECT.h>
//#include <TObject.h>

//#include <TROOT.h>
//#include <TApplication.h>
//#include <TVirtualX.h>
//#include <TGClient.h>

#include <TGMenu.h>
#include <TGDockableFrame.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGNumberEntry.h>
#include <TGStatusBar.h>
#include <TGraph.h>
//#include <TMultiGraph.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGComboBox.h>
#include <TGLayout.h>
#include <TGLabel.h>
//#include <TAxis.h> 
//#include <TGIcon.h> 

#include "c_Core.h"
#include "f_Channels.h"
#include "f_Graphs.h"

enum ETestCommandIdentifiers {
   M_FILE_EXIT,

   M_SETTINGS_CHANNELS,
   M_SETTINGS_DATAFLOW,

   M_VIEW_GRAPHS
};


class MainFrame: public TGMainFrame{

//RQ_OBJECT("MainFrame")

private:
	//Dockable Frame for Menu Bar (child of MainFrame).
	TGDockableFrame    *fMenuDock;

	//Menu Bar itself, Menu Popups and Layout Hints.
	TGMenuBar          *fMenuBar;
	TGPopupMenu        *fMenuFile, *fMenuSettings, *fMenuView/*, *fMenuHelp*/;
	TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;
	
	//Middle frame that contains the Canvas and the Left/Right Group Frames (child of MainFrame).
	TGCompositeFrame   *fMiddleFrame;
	
	//Embedded Canvas for TGraph (child of MiddleFrame).
	TRootEmbeddedCanvas *fEcanvas1;
	Int_t fIDcanvas1;
	TCanvas *fcanvasMPD1;
	//TCanvas *fcanvas1;
	//TCanvas *fcanvas2;
	//TCanvas *fcanvas3;
	//TCanvas *fcanvas4;
	//TCanvas *fcanvas5;
	//TCanvas *fcanvas6;
	//TCanvas *fcanvas7;
	//TCanvas *fcanvas8;

	//Graphics.
	TGraph *graph1;
	
	//Left and Right group frames (children of MiddleFrame).
	TGCompositeFrame   *fLGroupFrame, *fRGroupFrame;

	//Graphics Group (child of the Left group frame).
	TGGroupFrame *fgroupGraph;
	//TGComboBox *fComboGraph;

	//Measures Group (child of the Left group frame).
	TGGroupFrame *fgroupMeas;
	TGNumberEntryField *fNumEvents;
	TGNumberEntryField *fNumTime;
	TGNumberEntryField *fNumBaseline;
	TGNumberEntryField *fNumAmplitude;
	TGNumberEntryField *fNumCharge;
	TGNumberEntryField *fNumCountRate;
	TGNumberEntryField *fNumCounter;
	TGNumberEntryField *fNumRxRate;

	//Trigger Group (child of the Right group frame).
	TGGroupFrame *fgroupTrigg;
	TGComboBox *fComboTri;
	//TGComboBox *fComboFrom;
	TGNumberEntry *fNumTreshold;
	TGPictureButton *fPButtonSlope;
	TGTextButton *fButtonApply;

	//Control Group (child of the Right group frame).
	TGGroupFrame *fgroupCtrl;
	TGTextButton *fButtonRunMPD1;
	TGCheckButton *fScal;
	TGCheckButton *fSave;
	TGCheckButton *fTable;

	//Status Bar (child of MainFrame).
	TGStatusBar *fStatusBar;

	//Core
	Core *core;

	//Settings IO
	Settings *setts;

	//Settings Frame
	fChannelsFrame *fChannels;

	//Graphics Frame
	fGraphsFrame *fGraphs;

	//Variables
	bool fbslope;
	bool saveCal;
	bool saveWave;
	bool saveTable;

protected:

public:

	MainFrame(const TGWindow *p,UInt_t w,UInt_t h);
	virtual ~MainFrame();
	virtual void CloseWindow();

	void HandleMenu(Int_t id);
	void SettingsUpdate();
	void HandleComboTri();
	void FButtonSlope();
	void FButtonConfig();
	void FButtonRunMPD1();
	void HandleCheckBoxes();
	virtual bool Update();
	
	void LocalChanged() { Emit("LocalChanged()"); }	//*SIGNAL*
	
	//void GetGraphsOpen() {return GraphsOpen;}
	//void SetGraphsOpen() (bool state) {GraphsOpen = state;}

	ClassDef(MainFrame,0);
};

