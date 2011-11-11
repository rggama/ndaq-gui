#include "f_channels.h"



fChannelsFrame::fChannelsFrame(const TGWindow *p, const TGWindow *main, UInt_t w,
							   UInt_t h, UInt_t options)
{
	setts = new Settings();

	// Create a dialog window. A dialog window pops up with respect to its
	// "main" window.

	fMain = new TGTransientFrame(p, main, w, h, options);
	fMain->Connect("CloseWindow()", "fChannelsFrame", this, "DoClose()");
	fMain->DontCallClose(); // to avoid double deletions.

	// use hierarchical cleaning
	fMain->SetCleanup(kDeepCleanup);

	

	fFrame1 = new TGHorizontalFrame(fMain, 60, 40, /*kHorizontalFrame | kSunkenFrame |*/ kFixedWidth);

	fOkButton = new TGTextButton(fFrame1, "&Ok", 1);
	fOkButton->Connect("Clicked()", "fChannelsFrame", this, "DoOK()");
	fCancelButton = new TGTextButton(fFrame1, "&Cancel", 2);
	fCancelButton->Connect("Clicked()", "fChannelsFrame", this, "DoCancel()");

	fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           2, 2, 2, 2);
	fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

	fFrame1->AddFrame(fOkButton, fL1);
	fFrame1->AddFrame(fCancelButton, fL1);

	fFrame1->Resize(150, fOkButton->GetDefaultHeight());
	fMain->AddFrame(fFrame1, fL2);

	//--------- create Tab widget and some composite frames for Tab testing

	fTab = new TGTab(fMain, 300, 200);
	fTab->Connect("Selected(Int_t)", "fChannelsFrame", this, "DoTab(Int_t)");

	fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
	fL4 = new TGLayoutHints(kLHintsTop | kLHintsCenterX, 5, 5, 5, 5);

	TGCompositeFrame *tf = fTab;
   
	int i;
	char tmp[10];

	tf = fTab->AddTab("Selection");
	fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
	
	//TGTextButton *bt;
	//fF4->AddFrame(bt = new TGTextButton(fF4, "A&dd Entry", 90), fL3);
	//bt->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");

	//fF4->AddFrame(bt = new TGTextButton(fF4, "Remove &Entry", 91), fL3);
	//bt->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");

	//ChanLabel = new TGLabel();
	//ChanLabel->SetText("Teste");
	//ChanLabel->Layout();
	fF4->AddFrame(ChanLabel = new TGLabel(fF4, "Select the desired channels from the list below.\nThe total selected channels number MUST be a power of 2."), fL3);
	//ChanLabel->Layout();

	//ChanLabel->SetTextColor();
	fF4->AddFrame(fChanList = new TGListBox(fF4, 89), fL4);
	fChanList->SetMultipleSelections();
	//fChanList->Connect("SelectionChanged()", "fChannelsFrame", this, "HandleButtons(=89)");
	fChanList->Connect("Selected(Int_t)", "fChannelsFrame", this, "HandleChanList()");

	//fF4->AddFrame(fCheckMulti = new TGCheckButton(fF4, "&Mutli Selectable", 92), fL3);
	//fCheckMulti->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");
   
	tf->AddFrame(fF4, fL3);

	for (i = 0; i < 8; i++)  {
	   sprintf(tmp, "Channel %i", i+1);
		fChanList->AddEntry(tmp, i);
	}

	unsigned char c_config = setts->GetChanConfig();
	unsigned char btst = 0x01;

	for (i=0;i<8;i++)
	{
		if ((c_config & btst) == btst)
			fChanList->Select(i);
		btst = btst<<1;
	}
	
	fChanList->Resize(80, 140);

   ////--- tab 5
   //tf = fTab->AddTab("Tab 5");
   //tf->SetLayoutManager(new TGHorizontalLayout(tf));

   //fF6 = new TGGroupFrame(tf, "Options", kVerticalFrame);
   //fF6->SetTitlePos(TGGroupFrame::kRight); // right aligned
   //tf->AddFrame(fF6, fL3);

   //// 2 column, n rows
   //fF6->SetLayoutManager(new TGMatrixLayout(fF6, 0, 2, 10));
   //char buff[100];
   //int j;
   //for (j = 0; j < 4; j++) {
   //   sprintf(buff, "Module %i", j+1);
   //   fF6->AddFrame(new TGLabel(fF6, new TGHotString(buff)));

   //   TGTextBuffer *tbuf = new TGTextBuffer(10);
   //   tbuf->AddText(0, "0.0");

   //   TGTextEntry  *tent = new TGTextEntry(fF6, tbuf);
   //   tent->Resize(50, tent->GetDefaultHeight());
   //   tent->SetFont("-adobe-courier-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
   //   fF6->AddFrame(tent);
   //}
   //fF6->Resize();

   //// another matrix with text and buttons
   //fF7 = new TGGroupFrame(tf, "Tab Handling", kVerticalFrame);
   //tf->AddFrame(fF7, fL3);

   //fF7->SetLayoutManager(new TGMatrixLayout(fF7, 0, 1, 10));

   //fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab", 101));
   //bt->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");
   //bt->Resize(90, bt->GetDefaultHeight());

   //fF7->AddFrame(bt = new TGTextButton(fF7, "Add Tab", 103));
   //bt->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");
   //bt->Resize(90, bt->GetDefaultHeight());

   //fF7->AddFrame(bt = new TGTextButton(fF7, "Remove Tab 5", 102));
   //bt->Connect("Clicked()", "fChannelsFrame", this, "HandleButtons()");
   //bt->Resize(90, bt->GetDefaultHeight());

   ////fF7->Resize(fF6->GetDefaultSize());

   ////--- end of last tab

	TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
	fMain->AddFrame(fTab, fL5);

	fMain->MapSubwindows();
	fMain->Resize();

	// position relative to the parent's window
	fMain->CenterOnParent();

	fMain->SetWindowName("Dialog");

	fMain->MapWindow();
	//gClient->WaitFor(fMain);    // otherwise canvas contextmenu does not work
}

fChannelsFrame::~fChannelsFrame()
{
	// Delete test dialog widgets.

	fMain->DeleteWindow();  // deletes fMain
}

void fChannelsFrame::DoClose()
{
	printf("\nTerminating dialog: via window manager\n");
	CloseWindow();

	// Close the Ged editor if it was activated.
	if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
		TVirtualPadEditor::Terminate();
}

void fChannelsFrame::CloseWindow()
{
	// Called when window is closed via the window manager.
	delete this;
}

void fChannelsFrame::DoOK()
{
	printf("\nTerminating dialog: OK pressed\n");
	// Add protection against double-clicks
	fOkButton->SetState(kButtonDisabled);
	fCancelButton->SetState(kButtonDisabled);

	// Send a close message to the main frame. This will trigger the
	// emission of a CloseWindow() signal, which will then call
	// fChannelsFrame::CloseWindow(). Calling directly CloseWindow() will cause
	// a segv since the OK button is still accessed after the DoOK() method.
	// This works since the close message is handled synchronous (via
	// message going to/from X server).
	//fMain->SendCloseMessage();

	SettingsChanged();

	// The same effect can be obtained by using a singleshot timer:
	TTimer::SingleShot(150, "fChannelsFrame", this, "CloseWindow()");

	// Close the Ged editor if it was activated.
	if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
		TVirtualPadEditor::Terminate();
}


void fChannelsFrame::DoCancel()
{
	printf("\nTerminating dialog: Cancel pressed\n");
	// Add protection against double-clicks
	fOkButton->SetState(kButtonDisabled);
	fCancelButton->SetState(kButtonDisabled);
	TTimer::SingleShot(150, "fChannelsFrame", this, "CloseWindow()");
	// Close the Ged editor if it was activated.
	if (TVirtualPadEditor::GetPadEditor(kFALSE) != 0)
		TVirtualPadEditor::Terminate();
}

void fChannelsFrame::HandleButtons(Int_t id)
{
   // Handle different buttons.

   if (id == -1) {
      TGButton *btn = (TGButton *) gTQSender;
      id = btn->WidgetId();
   }

   printf("DoButton: id = %d\n", id);

   char tmp[20];
   static int newtab = 0;

   switch (id) {
      case 61:  // show item 1 in the combo box
         fCombo->Select(1);
         break;
      case 62:  // show item 2 in the combo box
         fCombo->Select(2);
         break;
      case 89:  // add one entry in list box
	     
         printf("Entry Status %u", fChanList->GetSelected());
         break;
      case 90:  // add one entry in list box
         fLastEntry++;
         sprintf(tmp, "Entry %i", fLastEntry);
         fChanList->AddEntry(tmp, fLastEntry);
         fChanList->MapSubwindows();
         fChanList->Layout();
         break;
      case 91:  // remove one entry in list box
         if (fFirstEntry <= fLastEntry) {
            fChanList->RemoveEntry(fFirstEntry);
            fChanList->Layout();
            fFirstEntry++;
         }
         break;
      case 101:  // remove tabs
         {
            TString s = fTab->GetTabTab(0)->GetString();
            if ((s == "Tab 3") && (fMain->MustCleanup() != kDeepCleanup)) {
               // Need to delete the embedded canvases
               // since RemoveTab() will Destroy the container
               // window, which in turn will destroy the embedded
               // canvas windows.
               delete fEc1; fEc1 = 0;
               delete fEc2; fEc2 = 0;
            }
            fTab->RemoveTab(0);
            fTab->Layout();
         }
         break;
      case 102:  // remove tab 5
         {
            int nt = fTab->GetNumberOfTabs();
            for (int i = 0 ; i < nt; i++) {
               TString s = fTab->GetTabTab(i)->GetString();
               if (s == "Tab 5") {
                  fTab->RemoveTab(i);
                  fTab->Layout();
                  break;
               }
            }
         }
         break;
      case 103:  // add tabs
         sprintf(tmp, "New Tab %d", ++newtab);
         fTab->AddTab(tmp);
         fTab->MapSubwindows();
         fTab->Layout();
         break;
      case 81:
         fRad2->SetState(kButtonUp);
         break;
      case 82:
         fRad1->SetState(kButtonUp);
         break;
      case 92:
         fChanList->SetMultipleSelections(fCheckMulti->GetState());
         break;
      default:
         break;
   }
}

void fChannelsFrame::HandleChanList(Int_t id)
{
	unsigned char c_config = setts->GetChanConfig();
	unsigned char c_total = setts->GetChanTotal();

	switch (id) {
	
	case 0:
		if ((c_config & 0x01) == 0x01)
		{
			c_config = c_config & ((unsigned char)~0x01);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x01;	
			c_total++;
		}
		break;

	case 1:
		if ((c_config & 0x02) == 0x02)
		{
			c_config = c_config & ((unsigned char)~0x02);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x02;	
			c_total++;
		}
		break;

	case 2:
		if ((c_config & 0x04) == 0x04)
		{
			c_config = c_config & ((unsigned char)~0x04);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x04;	
			c_total++;
		}
		break;

	case 3:
		if ((c_config & 0x08) == 0x08)
		{
			c_config = c_config & ((unsigned char)~0x08);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x08;	
			c_total++;
		}
		break;

	case 4:
		if ((c_config & 0x10) == 0x10)
		{
			c_config = c_config & ((unsigned char)~0x10);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x10;	
			c_total++;
		}
		break;

	case 5:
		if ((c_config & 0x20) == 0x20)
		{
			c_config = c_config & ((unsigned char)~0x20);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x20;	
			c_total++;
		}
		break;

	case 6:
		if ((c_config & 0x40) == 0x40)
		{
			c_config = c_config & ((unsigned char)~0x40);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x40;	
			c_total++;
		}
		break;

	case 7:
		if ((c_config & 0x80) == 0x80)
		{
			c_config = c_config & ((unsigned char)~0x80);
			c_total--;
		}
		else
		{
			c_config = c_config | 0x80;	
			c_total++;
		}
		break;

	default:
		printf("Toggled %u", fChanList->GetSelected());
		break;
	}

	setts->SetChanConfig(c_config);
	setts->SetChanTotal(c_total);
	
	if ((c_total != 0) && ((8%c_total) > 0))
	{	
		fChanList->ChangeBackground(0xFF0000); //Red
		fOkButton->SetEnabled(kFALSE);
	}
	else
	{	
		fChanList->ChangeBackground(0x00FF00); //Green
		fOkButton->SetEnabled(kTRUE);
	}

	printf("Config: %u\n", setts->GetChanConfig());
	printf("Total: %u\n", setts->GetChanTotal());
}

void fChannelsFrame::DoTab(Int_t id)
{
   printf("Tab item %d activated\n", id);
}

