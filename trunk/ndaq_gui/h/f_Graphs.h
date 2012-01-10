// header files required by application

//#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include <TClass.h>
#include <TThread.h>

//#include <TVirtualPadEditor.h>

//#include <TGResourcePool.h>
//#include <TGListBox.h>
//#include <TGListTree.h>
//#include <TGFSContainer.h>
//#include <TGClient.h>
#include <TGFrame.h>
//#include <TGIcon.h>
//#include <TGLabel.h>
//#include <TGButton.h>
//#include <TGTextEntry.h>
//#include <TGNumberEntry.h>
//#include <TGMsgBox.h>
//#include <TGMenu.h>
//#include <TGCanvas.h>
//#include <TGComboBox.h>
//#include <TGTab.h>
//#include <TGSlider.h>
//#include <TGDoubleSlider.h>
//#include <TGFileDialog.h>
//#include <TGTextEdit.h>
//#include <TGShutter.h>
//#include <TGProgressBar.h>
//#include <TGColorSelect.h>
#include <RQ_OBJECT.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
//#include <TColor.h>
//#include <TH1.h>
//#include <TH2.h>
//#include <TRandom.h>
//#include <TSystem.h>
#include <TGraph.h>
#include <TAxis.h>
//#include <TSystemDirectory.h>
//#include <TEnv.h>
//#include <TFile.h>
//#include <TKey.h>
//#include <TGFontDialog.h>

#include "c_Settings.h"

class fGraphsFrame{

RQ_OBJECT("fGraphsFrame")

private:

	TGTransientFrame	*fMain;

	TGCompositeFrame	*fMiddleFrame;

	//TRootEmbeddedCanvas	*fEcanvas1, *fEcanvas2, *fEcanvas3, *fEcanvas4, *fEcanvas5, *fEcanvas6, *fEcanvas7, *fEcanvas8;
	TCanvas				*fcanvas1, *fcanvas2, *fcanvas3, *fcanvas4, *fcanvas5, *fcanvas6, *fcanvas7, *fcanvas8;
	TGraph				*graph1;

	Settings			*setts;

/***************************************************************/
/*
   TGLabel *ChanLabel;

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
*/
protected:

public:
	TRootEmbeddedCanvas	*fEcanvas1, *fEcanvas2, *fEcanvas3, *fEcanvas4, *fEcanvas5, *fEcanvas6, *fEcanvas7, *fEcanvas8;

	fGraphsFrame(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, 
					UInt_t options = kVerticalFrame)/*: Settings()*/;
	
	virtual ~fGraphsFrame();
	
	// slots
	void DoClose();
	void CloseWindow();

	void PlotGraph1(Int_t *x, Int_t *y);
};
