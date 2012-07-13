#include "f_Graphs.h"
#include "defines.h"

#define GRAPH_H	300
#define GRAPH_V	480

fGraphsFrame::fGraphsFrame(const TGWindow *p, const TGWindow *main, UInt_t w,
							   UInt_t h, UInt_t options)
{

	// Create a dialog window. A dialog window pops up with respect to its
	// "main" window.
	fMain = new TGTransientFrame(p, main, w, h, options);
	fMain->Connect("CloseWindow()", "fGraphsFrame", this, "DoClose()");
	fMain->DontCallClose(); // to avoid double deletions.

	// use hierarchical cleaning
	fMain->SetCleanup(kDeepCleanup);

	fMain->SetWindowName("All Channels");

	/****************************************************************************************************/
	/*** MainFrame: MiddleFrame *************************************************************************/
	/****************************************************************************************************/
	fMiddleFrame = new TGCompositeFrame(fMain, 60, 20, kHorizontalFrame |
                                                      kSunkenFrame);
	// 2 column, n rows
	fMiddleFrame->SetLayoutManager(new TGMatrixLayout(fMiddleFrame, 2, 0, 1));

	/****************************************************************************************************/
	/*** MiddleFrame: Canvas ****************************************************************************/
	/****************************************************************************************************/

	//Canvas 1
	fEcanvas1 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas1 = new TCanvas("ch1", 10, 10, fEcanvas1->GetCanvasWindowId());
   	fEcanvas1->AdoptCanvas(fcanvas1);
	fEcanvas1->GetCanvas()->SetGridx();
	fEcanvas1->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas1, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 2
	fEcanvas2 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas2 = new TCanvas("ch2", 10, 10, fEcanvas2->GetCanvasWindowId());
   	fEcanvas2->AdoptCanvas(fcanvas2);
	fEcanvas2->GetCanvas()->SetGridx();
	fEcanvas2->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas2, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));
	
	//Canvas 3
	fEcanvas3 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas3 = new TCanvas("ch3", 10, 10, fEcanvas3->GetCanvasWindowId());
   	fEcanvas3->AdoptCanvas(fcanvas3);
	fEcanvas3->GetCanvas()->SetGridx();
	fEcanvas3->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas3, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 4
	fEcanvas4 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas4 = new TCanvas("ch4", 10, 10, fEcanvas4->GetCanvasWindowId());
   	fEcanvas4->AdoptCanvas(fcanvas4);
	fEcanvas4->GetCanvas()->SetGridx();
	fEcanvas4->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas4, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 5
	fEcanvas5 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas5 = new TCanvas("ch5", 10, 10, fEcanvas5->GetCanvasWindowId());
   	fEcanvas5->AdoptCanvas(fcanvas5);
	fEcanvas5->GetCanvas()->SetGridx();
	fEcanvas5->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas5, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 6
	fEcanvas6 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas6 = new TCanvas("ch6", 10, 10, fEcanvas6->GetCanvasWindowId());
   	fEcanvas6->AdoptCanvas(fcanvas6);
	fEcanvas6->GetCanvas()->SetGridx();
	fEcanvas6->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas6, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 7
	fEcanvas7 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas7 = new TCanvas("ch7", 10, 10, fEcanvas7->GetCanvasWindowId());
   	fEcanvas7->AdoptCanvas(fcanvas7);
	fEcanvas7->GetCanvas()->SetGridx();
	fEcanvas7->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas7, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	//Canvas 8
	fEcanvas8 = new TRootEmbeddedCanvas(0,fMiddleFrame,GRAPH_H,GRAPH_V);
   	fcanvas8 = new TCanvas("ch8", 10, 10, fEcanvas8->GetCanvasWindowId());
   	fEcanvas8->AdoptCanvas(fcanvas8);
	fEcanvas8->GetCanvas()->SetGridx();
	fEcanvas8->GetCanvas()->SetGridy();
   	fMiddleFrame->AddFrame(fEcanvas8, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

	/****************************************************************************************************/
	/*** MiddleFrame: END  ******************************************************************************/
	/****************************************************************************************************/
	fMain->AddFrame(fMiddleFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
	
	fMain->MapSubwindows();
	fMain->Resize();

	// position relative to the parent's window
	//fMain->CenterOnParent();

	fMain->MapWindow();
	//gClient->WaitFor(fMain);    // otherwise canvas contextmenu does not work
	setts = new Settings();
	setts->SetGraphsOpen(true);
}

fGraphsFrame::~fGraphsFrame()
{
	// Delete test dialog widgets.
	//setts->SetGraphsOpen(false);
	//gSystem->Sleep(500);
	printf("destructor\n");
	fMain->DeleteWindow();  // deletes fMain
	//fMain->Cleanup();
}

void fGraphsFrame::DoClose()
{
	setts->SetGraphsOpen(false);
	//gSystem->Sleep(500);
	printf("doclose\n");
	printf("\nTerminating dialog: via window manager\n");
	CloseWindow();

	/*// Close the Ged editor if it was activated.
	if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
		TVirtualPadEditor::Terminate();*/
}

void fGraphsFrame::CloseWindow()
{
	//setts->SetGraphsOpen(false);
	//gSystem->Sleep(500);
	printf("closewindow\n");
	// Called when window is closed via the window manager.
	delete this;
}

void fGraphsFrame::PlotGraph1(Int_t *x, Int_t *y)
{
	fEcanvas1->GetCanvas()->cd();	

	//graph1 = new TGraph(EVENT_SIZE, x, y);
	graph1->SetTitle("Event");
	graph1->SetEditable(kFALSE);
	graph1->SetLineColor(kRed);
	graph1->GetYaxis()->SetRangeUser(-220, 50);
	//graph1->GetXaxis()->SetRangeUser(0, EVENT_SIZE);				
	graph1->Draw("AL");			

	fEcanvas1->GetCanvas()->Update();	
}