// header files required by application

//#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include <TClass.h>

#include <TVirtualPadEditor.h>

//#include <TGResourcePool.h>
#include <TGListBox.h>
//#include <TGListTree.h>
//#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
//#include <TGMenu.h>
//#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
//#include <TGTextEdit.h>
//#include <TGShutter.h>
//#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <RQ_OBJECT.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
//#include <TSystemDirectory.h>
//#include <TEnv.h>
//#include <TFile.h>
//#include <TKey.h>
//#include <TGFontDialog.h>

#include "c_Settings.h"

class fChannelsFrame{

RQ_OBJECT("fChannelsFrame")

private:

	Settings *setts;
	TGLabel *ChanLabel;

	TGTransientFrame		*fMain;
   TGCompositeFrame		*fFrame1, *fF1, *fF2, *fF3, *fF4, *fF5;
   TGGroupFrame			*fF6, *fF7;
   TGButton				*fOkButton, *fCancelButton, *fStartB, *fStopB;
   TGButton				*fBtn1, *fBtn2, *fChk1, *fChk2, *fRad1, *fRad2;
   TGPictureButton		*fPicBut1;
   TGCheckButton		*fCheck1;
   TGCheckButton		*fCheckMulti;
   TGListBox			*fChanList;
   TGComboBox			*fCombo;
   TGTab				*fTab;
   TGTextEntry			*fTxt1, *fTxt2;
   TGLayoutHints		*fL1, *fL2, *fL3, *fL4;
   TRootEmbeddedCanvas	*fEc1, *fEc2;
   Int_t                fFirstEntry;
   Int_t                fLastEntry;

public:
   fChannelsFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
					UInt_t options = kVerticalFrame)/*: Settings()*/;
   virtual ~fChannelsFrame();

   // slots
   void DoClose();
   void CloseWindow();
   void DoOK();
   void DoCancel();
   void DoTab(Int_t id);
   void HandleButtons(Int_t id = -1);

   //void SetChanConfig(unsigned char nc_config) { c_config = nc_config; }
   //unsigned char GetChanConfig(void) { return c_config; }
   //void SetChanTotal(unsigned char nc_total) { c_total = nc_total; }
   //unsigned char GetChanTotal(void) { return c_total; }

   void HandleChanList(Int_t id = -1);

   void SettingsChanged() { Emit("SettingsChanged()"); }	//*SIGNAL*
};
