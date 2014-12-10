//---------------------------------------------------------------------------
#include "Graph.h"
#pragma hdrstop
#include "Unit1.h"
#include "Unit22.h"
#include "PyGraph.h"
#include <clipbrd.hpp>
//---------------------------------------------------------------------------
#pragma link "IRichEdit"
#pragma resource "*.dfm"
TForm22 *Form22 = NULL;
//---------------------------------------------------------------------------
__fastcall TForm22::TForm22(TComponent* Owner)
  : TForm(Owner), LastIndex(0), FAllowChange(false), CacheIndex(0), TextCache(1), PromptIndex(0),
    IndentLevel(0)
{
  WritePrompt();
  Canvas->Font->Assign(IRichEdit1->Font);
  int CharWidth = (Canvas->TextWidth("A") * 15)/20;
  IRichEdit1->Paragraph->TabCount = 22;
  int x = CharWidth * 4;
  for(int I = 0; I < IRichEdit1->Paragraph->TabCount; I++, x+=CharWidth*2)
    IRichEdit1->Paragraph->Tab[I] = x;
  TranslateComponent(this);
}
//---------------------------------------------------------------------------
void TForm22::Translate()
{
  RetranslateComponent(this);
}
//---------------------------------------------------------------------------
void __fastcall TForm22::FormHide(TObject *Sender)
{
  Form1->Panel6->Height = 0;
  Form1->Splitter2->Visible = false;
}
//---------------------------------------------------------------------------
void TForm22::WriteText(const String &Str, TColor Color)
{
  if(IRichEdit1 == NULL) //Handle the case where we are terminating
    return;
  FAllowChange = true;
  int OldSelStart = IRichEdit1->SelStart;
  IRichEdit1->SelStart = PromptIndex;
  IRichEdit1->TextFormat.SetColor(Color);
  IRichEdit1->SelText = Str;
  int IndexChange = IRichEdit1->SelStart - PromptIndex;
  LastIndex += IndexChange;
  PromptIndex = IRichEdit1->SelStart;
  IRichEdit1->SelStart = OldSelStart + IndexChange;
  IRichEdit1->SelLength = 0;
  FAllowChange = false;
  SendMessage(IRichEdit1->Handle, WM_VSCROLL, SB_BOTTOM, 0);
}
//---------------------------------------------------------------------------
void __fastcall TForm22::IRichEdit1ProtectChange(TObject *Sender,
      int StartPos, int EndPos, bool &AllowChange)
{
  AllowChange = FAllowChange;
}
//---------------------------------------------------------------------------
void __fastcall TForm22::IRichEdit1KeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
  //Catch Paste (Ctrl+V or Shift+Insert)
  if((Shift == (TShiftState() << ssCtrl) && Key == 'V') ||
     (Shift == (TShiftState() << ssShift) && Key == VK_INSERT))
  {
		String Str = Clipboard()->AsText;
		int Line = 1;
		while(!Str.IsEmpty())
		{
			int I = Str.Pos("\r\n");
			if(I == 0)
				I = Str.Length()+1;
			IRichEdit1->SelText = Str.SubString(1, I-1);
			Str.Delete(1, I+1);
			if(!Str.IsEmpty() || Line > 1)
			{
				HandleNewLine();
				Line++;
			}
    }
		Key = 0;
		return;
	}

	if((Shift == (TShiftState() << ssCtrl) && Key == 'C'))
		if(IRichEdit1->SelLength == 0)
			KeyboardInterrupt();


	switch(Key)
	{
		case VK_HOME:
		{
      for(int Line = IRichEdit1->GetLine(-1); Line >= 0; Line--)
      {
			  int Index = IRichEdit1->LineIndex(Line);
  			String Str = IRichEdit1->GetText(Index, Index + 4);
  			if(Str == ">>> ")
   			{
  				int Pos = IRichEdit1->SelStart;
	  			IRichEdit1->SelStart = Index + 4;
  				if(Shift.Contains(ssShift))
  				  IRichEdit1->SelLength = Pos - Index - 4;
  				Key = 0;
          break;
        }
			}
			break;
		}

		case VK_UP:
			if(Shift.Empty() && IRichEdit1->GetLine(-1) == IRichEdit1->LineCount() - 1)
			{
				if(CacheIndex > 0)
				{
					IRichEdit1->SelStart = LastIndex;
					IRichEdit1->SelLength = MAXINT;
					if(CacheIndex == (int)TextCache.size() -1)
						TextCache.back() = IRichEdit1->GetText(LastIndex, MAXINT);
					SetUserString(TextCache[--CacheIndex]);
				}
				Key = 0;
			}
			break;

		case VK_DOWN:
			if(Shift.Empty() && IRichEdit1->GetLine(-1) == IRichEdit1->LineCount() - 1)
			{
				if(CacheIndex < (int)TextCache.size() - 1)
					SetUserString(TextCache[++CacheIndex]);
				Key = 0;
			}
			break;

		case VK_RETURN:
			if(Shift.Empty())
			{
				HandleNewLine();
				Key = 0;
			}
			break;

		case VK_ESCAPE:
			if(Shift.Empty())
				SetUserString("");
			break;
	}
}
//---------------------------------------------------------------------------
void TForm22::HandleNewLine()
{
  String Str = IRichEdit1->GetText(LastIndex, MAXINT);
  Str = Str.TrimRight();
  IRichEdit1->SelStart = MAXINT;
  IRichEdit1->SelText = "\r";
  PromptIndex = IRichEdit1->SelStart;
  Command += Str;

  if(!Str.IsEmpty())
  {
    TextCache.back() = Str;
    TextCache.push_back(String());
  }
  CacheIndex = TextCache.size() - 1;

	try
	{
		if(Python::ExecutePythonCommand(Command))
		{
			WritePrompt();
			Command = "";
			IndentLevel = 0;
		}
		else
		{
			WritePrompt("... ");
			Command += "\n";
			if(!Str.IsEmpty() && Str[Str.Length()] == ':')
				IndentLevel++;
		}
	}
	catch(...)
	{
		WritePrompt();
		Command = "";
		IndentLevel = 0;
		throw;
	}
	IRichEdit1->SelText = String::StringOfChar('\t', IndentLevel);
}
//---------------------------------------------------------------------------
void __fastcall TForm22::FormShow(TObject *Sender)
{
  IRichEdit1->SelStart = MAXINT;
  StartTimer(1, AfterFormShown);
}
//---------------------------------------------------------------------------
void TForm22::WritePrompt(const String &Str)
{
  FAllowChange = true;
  IRichEdit1->TextFormat.SetColor(clBlack);
  IRichEdit1->SelStart = MAXINT;
  PromptIndex = IRichEdit1->SelStart;
  IRichEdit1->SelText = Str;
  LastIndex = IRichEdit1->SelStart;
  IRichEdit1->GlobalTextFormat.SetProtected(true);
  IRichEdit1->SelLength = 1;
  IRichEdit1->TextFormat.SetProtected(false);
  IRichEdit1->SelLength = 0;
  FAllowChange = false;
  SendMessage(IRichEdit1->Handle, WM_VSCROLL, SB_BOTTOM, 0);
}
//---------------------------------------------------------------------------
void __fastcall TForm22::Clear1Click(TObject *Sender)
{
  Clear();
  WritePrompt();
}
//---------------------------------------------------------------------------
void TForm22::Clear()
{
  FAllowChange = true;
  IRichEdit1->Clear();
  FAllowChange = false;
  LastIndex = 0;
  PromptIndex = 0;
}
//---------------------------------------------------------------------------
void TForm22::KeyboardInterrupt()
{
  IRichEdit1->SelStart = MAXINT;
  IRichEdit1->SelText = "\r";
  PromptIndex = IRichEdit1->SelStart;
  WriteText("KeyboardInterrupt\r\n", clRed);
  WritePrompt();
  Command = "";
  IndentLevel = 0;
  CacheIndex = TextCache.size() - 1;
}
//---------------------------------------------------------------------------
void TForm22::SetUserString(const String &Str)
{
	IRichEdit1->SelStart = LastIndex;
	IRichEdit1->SelLength = MAXINT;
	IRichEdit1->SelText = Str;
}
//---------------------------------------------------------------------------
void __fastcall TForm22::AfterFormShown(TObject*)
{
	Form1->Splitter2->Visible = Form1->Panel6->VisibleDockClientCount > 0;
	Form1->Panel6->Height = Form1->Panel6->VisibleDockClientCount ? 150 : 0;}
//---------------------------------------------------------------------------
void __fastcall TForm22::WMEnable(TMessage &Message)
{
  //Ensure that this form is always enabled, even when a modal dialog is shown
	if(!Message.WParam)
		EnableWindow(Handle, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm22::IRichEdit1Enter(TObject *Sender)
{
  //Clear shortcuts conflicting with the rich edit
  Form1->MoveLeftAction->ShortCut = 0;
  Form1->MoveRightAction->ShortCut = 0;
  Form1->MoveLeftAction->SecondaryShortCuts->Clear();
  Form1->MoveRightAction->SecondaryShortCuts->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TForm22::IRichEdit1Exit(TObject *Sender)
{
  //Reestablish shortcuts when they no longer conflict with the rich edit
  Form1->MoveLeftAction->ShortCut = ShortCut(VK_LEFT, TShiftState() << ssCtrl);
  Form1->MoveRightAction->ShortCut = ShortCut(VK_RIGHT, TShiftState() << ssCtrl);
  Form1->MoveLeftAction->SecondaryShortCuts->Add("Ctrl+Shift+Left");
  Form1->MoveRightAction->SecondaryShortCuts->Add("Ctrl+Shift+Right");
}
//---------------------------------------------------------------------------

