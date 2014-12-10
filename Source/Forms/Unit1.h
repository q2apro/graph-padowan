/* Graph (http://sourceforge.net/projects/graph)
 * Copyright 2007 Ivan Johansen
 *
 * Graph is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <TRecent.h>
#include <ImgList.hpp>
#include <AppEvnts.hpp>
#include "IPageSetup.h"
#include "IColorSelect.h"
#include "Cross.h"
#include "FocusPanel.h"
#include "MySplitter.h"
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <ActnCtrls.hpp>
#include <ActnList.hpp>
#include <ActnMan.hpp>
#include <CustomizeDlg.hpp>
#include <ToolWin.hpp>
#include <StdActns.hpp>
#include "IColorSelect.h"
#include "IPageSetup.h"
#include "IPolygon.h"
#include "IRichEdit.h"
#include <IdAntiFreeze.hpp>
#include <IdAntiFreezeBase.hpp>
#include <OleCtnrs.hpp>
#include <ExtDlgs.hpp>
#include "OpenPreviewDialog.h"
#include "IPrintDialog.h"
#include "SaveDialogEx.h"
#include <ListActns.hpp>
#include <PlatformDefaultStyleActnCtrls.hpp>
#include <ActnMenus.hpp>
#include <ActnPopup.hpp>
#include <XPMan.hpp>
#include <stack>
#include <boost/scoped_ptr.hpp>
//---------------------------------------------------------------------------
enum TCursorState {csIdle, csMoveLabel, csMoveLegend, csZoomWindow, csMove, csMoving};

enum TIconIndex
{
  iiSave, //Position of save icon used in StatusBar in image list
  iiPrint,
  iiFuncNode,
  iiTanNode,
  iiLabelNode,
  iiAxesNode,
  iiBullet,
  iiNormalNode,
  iiUnChecked,
  iiChecked,
  iiGrayed
};

class TForm1 : public TForm
{
__published:	// IDE-managed Components
  TOpenDialog *OpenDialog;
  TSaveDialogEx *SaveDialog;
  TRecent *Recent1;
  TPopupActionBar *PopupMenu1;
  TMenuItem *Tree_Delete;
  TMenuItem *Tree_Properties;
  TApplicationEvents *ApplicationEvents;
  TTimer *Timer1;
  TImageList *ImageList1;
  TMenuItem *Tree_Copy;
  TMenuItem *Tree_Cut;
  TMenuItem *Tree_Paste;
  TMenuItem *N8;
  TMenuItem *N10;
  TMenuItem *Tree_InsTan;
  TMenuItem *Tree_InsDiff;
  TMenuItem *Tree_InsTrend;
  TMenuItem *Tree_InsShade;
  TImageList *ImageList2;
  TCustomizeDlg *CustomizeDlg;
  TActionManager *ActionManager;
  TAction *NewAction;
  TActionToolBar *ActionToolBar1;
  TAction *OpenAction;
  TAction *SaveAction;
  TAction *SaveAsAction;
  TAction *SaveAsImageAction;
  TAction *PrintAction;
  TAction *ExitAction;
  TAction *UndoAction;
  TAction *RedoAction;
  TAction *CutAction;
  TAction *CopyAction;
  TAction *PasteAction;
  TAction *CopyImageAction;
  TAction *AxesAction;
  TAction *OptionsAction;
  TAction *InsertFunctionAction;
  TAction *InsertTangentAction;
  TAction *InsertShadeAction;
  TAction *InsertPointsAction;
  TAction *InsertTrendlineAction;
  TAction *EditAction;
  TAction *DeleteAction;
  TAction *InsertDifAction;
  TAction *ZoomInAction;
  TAction *ZoomOutAction;
  TAction *ZoomWindowAction;
  TAction *ZoomStandardAction;
  TAction *MoveUpAction;
  TAction *MoveDownAction;
  TAction *MoveLeftAction;
  TAction *MoveRightAction;
  TAction *MoveAction;
  TAction *PathAction;
  TAction *AreaAction;
  TAction *EvalAction;
  TAction *TableAction;
  TAction *ContentsAction;
  TAction *ListAction;
  TAction *HomePageAction;
  TAction *UpdateAction;
  TAction *AboutAction;
  TAction *SeparatorAction;
  TPopupActionBar *PopupMenu2;
  TMenuItem *ToolBar_Reset;
  TMenuItem *ToolBar_Customize;
  TMenuItem *N11;
  TAction *ZoomFitAllAction;
  TAction *FaqAction;
  TAction *ZoomFitAction;
  TAction *ZoomSquareAction;
  TMenuItem *Tree_ShowInLegend;
  TMenuItem *N15;
  TAction *InsertLabelAction;
  TPopupActionBar *PopupMenu3;
  TMenuItem *Label_Edit;
  TMenuItem *Label_Delete;
  TAction *DonateAction;
  TAction *TipAction;
  TAction *SaveCopyAsAction;
  TOpenPreviewDialog *OpenPreviewDialog1;
  TMenuItem *Tree_Export;
  TIPrintDialog *IPrintDialog1;
  TAction *SupportAction;
  TAction *ZoomXInAction;
  TAction *ZoomXOutAction;
  TAction *ZoomYInAction;
  TAction *ZoomYOutAction;
  TAction *InsertRelationAction;
  TAction *CustomFunctionsAction;
  TSaveDialogEx *SaveDialogEx1;
  TMenuItem *Label_Placement;
  TMenuItem *Label_Above;
  TMenuItem *Label_Below;
  TMenuItem *Label_Left;
  TMenuItem *Label_Right;
  TMenuItem *Tree_Placement;
  TMenuItem *Tree_Right;
  TMenuItem *Tree_Left;
  TMenuItem *Tree_Below;
  TMenuItem *Tree_Above;
  TMenuItem *Label_Rotation;
  TMenuItem *Rotation_90;
  TMenuItem *Rotation_180;
  TMenuItem *Rotation_270;
  TMenuItem *Rotation_0;
  TAction *AnimateAction;
  TAction *ImportGraphFileAction;
  TAction *ImportPointSeriesAction;
  TMenuItem *Label_Rotation_Custom;
  TMenuItem *Tree_Rotation;
  TMenuItem *Tree_Rotation_0;
  TMenuItem *Tree_Rotation_90;
  TMenuItem *Tree_Rotation_180;
  TMenuItem *Tree_Rotation_270;
  TMenuItem *Tree_Rotation_Custom;
  TMenuItem *Tree_Placement_Custom;
  TMenuItem *Label_Placement_Custom;
  TTimer *Timer2;
  TActionMainMenuBar *ActionMainMenuBar1;
  TPopupActionBar *PopupMenu4;
  TMenuItem *Legend_Show;
  TMenuItem *Legend_Font;
  TMenuItem *Legend_Placement;
  TMenuItem *Legend_BottomLeft;
  TMenuItem *Legend_TopLeft;
  TMenuItem *Legend_BottomRight;
  TMenuItem *Legend_TopRight;
  TStatusBar *StatusBar1;
  TPanel *Panel5;
  TFocusPanel *Panel2;
  TImage *Image1;
  TCross *Cross;
  TShape *Shape1;
  TIPolygon *IPolygon1;
  TImage *Image2;
  TPanel *Panel3;
  TTreeView *TreeView;
  TPanel *Panel1;
  TPanel *Panel4;
  TPanel *Panel6;
  TMySplitter *Splitter1;
  TSplitter *Splitter2;
  TMenuItem *N1;
  TAction *ScriptDocAction;
  void __fastcall Image1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall Image1MouseMove(TObject *Sender, TShiftState Shift, int X,int Y);
  void __fastcall Image1MouseUp(TObject *Sender, TMouseButton Button,	TShiftState Shift, int X, int Y);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormConstrainedResize(TObject *Sender,
  int &MinWidth, int &MinHeight, int &MaxWidth, int &MaxHeight);
  void __fastcall StatusBar1Resize(TObject *Sender);
  void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar,
  TStatusPanel *Panel, const TRect &Rect);
  void __fastcall Timer1Timer(TObject *Sender);
  void __fastcall ApplicationEventsShowHint(String &HintStr,
  bool &CanShow, THintInfo &HintInfo);
  void __fastcall TreeViewCollapsing(TObject *Sender,
  TTreeNode *Node, bool &AllowCollapse);
  void __fastcall Panel2Resize(TObject *Sender);
  void __fastcall TreeViewContextPopup(TObject *Sender,
  TPoint &MousePos, bool &Handled);
  void __fastcall TreeViewDblClick(TObject *Sender);
  void __fastcall TreeViewMouseDown(TObject *Sender,
  TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall FormMouseWheelUp(TObject *Sender,
  TShiftState Shift, const TPoint &MousePos, bool &Handled);
  void __fastcall FormMouseWheelDown(TObject *Sender,
  TShiftState Shift, const TPoint &MousePos, bool &Handled);
  void __fastcall Splitter1DblClick(TObject *Sender);
  void __fastcall TreeViewChange(TObject *Sender, TTreeNode *Node);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift);
  void __fastcall FormKeyPress(TObject *Sender, char &Key);
  void __fastcall TreeViewMouseMove(TObject *Sender,
    TShiftState Shift, int X, int Y);
  void __fastcall TreeViewKeyPress(TObject *Sender, char &Key);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall ApplicationEventsActivate(TObject *Sender);
  void __fastcall Panel2MouseLeave(TObject *Sender);
  bool __fastcall Recent1LoadFile(TRecent *Sender,
    String FileName);
  void __fastcall xPageSetupDialogPaintText(TIPageSetupDialog *Sender,
    TCanvas *PageCanvas, const TRect &UpdateRect,
    const TRect &PaperSize);
  void __fastcall NewActionExecute(TObject *Sender);
  void __fastcall OpenActionExecute(TObject *Sender);
  void __fastcall SaveActionExecute(TObject *Sender);
  void __fastcall SaveAsActionExecute(TObject *Sender);
  void __fastcall SaveAsImageActionExecute(TObject *Sender);
  void __fastcall PrintActionExecute(TObject *Sender);
  void __fastcall ExitActionExecute(TObject *Sender);
  void __fastcall UndoActionExecute(TObject *Sender);
  void __fastcall RedoActionExecute(TObject *Sender);
  void __fastcall CutActionExecute(TObject *Sender);
  void __fastcall CopyActionExecute(TObject *Sender);
  void __fastcall PasteActionExecute(TObject *Sender);
  void __fastcall CopyImageActionExecute(TObject *Sender);
  void __fastcall AxesActionExecute(TObject *Sender);
  void __fastcall OptionsActionExecute(TObject *Sender);
  void __fastcall InsertFunctionActionExecute(TObject *Sender);
  void __fastcall InsertTangentActionExecute(TObject *Sender);
  void __fastcall InsertShadeActionExecute(TObject *Sender);
  void __fastcall InsertPointsActionExecute(TObject *Sender);
  void __fastcall InsertTrendlineActionExecute(TObject *Sender);
  void __fastcall EditActionExecute(TObject *Sender);
  void __fastcall DeleteActionExecute(TObject *Sender);
  void __fastcall InsertDifActionExecute(TObject *Sender);
  void __fastcall ZoomInActionExecute(TObject *Sender);
  void __fastcall ZoomOutActionExecute(TObject *Sender);
  void __fastcall ZoomWindowActionExecute(TObject *Sender);
  void __fastcall ZoomStandardActionExecute(TObject *Sender);
  void __fastcall PathActionExecute(TObject *Sender);
  void __fastcall AreaActionExecute(TObject *Sender);
  void __fastcall EvalActionExecute(TObject *Sender);
  void __fastcall TableActionExecute(TObject *Sender);
  void __fastcall ContentsActionExecute(TObject *Sender);
  void __fastcall ListActionExecute(TObject *Sender);
  void __fastcall HomePageActionExecute(TObject *Sender);
  void __fastcall UpdateActionExecute(TObject *Sender);
  void __fastcall AboutActionExecute(TObject *Sender);
  void __fastcall MoveRightActionExecute(TObject *Sender);
  void __fastcall MoveLeftActionExecute(TObject *Sender);
  void __fastcall MoveUpActionExecute(TObject *Sender);
  void __fastcall MoveDownActionExecute(TObject *Sender);
  void __fastcall SeparatorActionUpdate(TObject *Sender);
  void __fastcall MoveActionExecute(TObject *Sender);
  void __fastcall ToolBar_CustomizeClick(TObject *Sender);
  void __fastcall ToolBar_ResetClick(TObject *Sender);
  void __fastcall CustomizeDlgClose(TObject *Sender);
  void __fastcall ApplicationEventsSettingChange(TObject *Sender, int Flag,
    const String Section, int &Result);
  void __fastcall ZoomFitAllActionExecute(TObject *Sender);
  void __fastcall FaqActionExecute(TObject *Sender);
  void __fastcall Panel4DockDrop(TObject *Sender, TDragDockObject *Source,
    int X, int Y);
  void __fastcall Panel4UnDock(TObject *Sender, TControl *Client,
    TWinControl *NewTarget, bool &Allow);
  void __fastcall Panel4DockOver(TObject *Sender, TDragDockObject *Source,
    int X, int Y, TDragState State, bool &Accept);
  void __fastcall FormShow(TObject *Sender);
  void __fastcall ZoomFitActionExecute(TObject *Sender);
  void __fastcall ZoomSquareActionExecute(TObject *Sender);
  void __fastcall Tree_ShowInLegendClick(TObject *Sender);
  void __fastcall InsertLabelActionExecute(TObject *Sender);
  void __fastcall Image1DblClick(TObject *Sender);
  void __fastcall Label_EditClick(TObject *Sender);
  void __fastcall Label_DeleteClick(TObject *Sender);
  void __fastcall DonateActionExecute(TObject *Sender);
  void __fastcall TipActionExecute(TObject *Sender);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall SaveCopyAsActionExecute(TObject *Sender);
  void __fastcall TreeViewKeyDown(TObject *Sender, WORD &Key,
    TShiftState Shift);
  bool __fastcall OpenPreviewDialog1PreviewFile(TOpenPreviewDialog *Dialog,
    const String &FileName, TCanvas *Canvas, const TRect &Rect);
  void __fastcall OpenPreviewDialog1Show(TObject *Sender);
  void __fastcall OpenPreviewDialog1Close(TObject *Sender);
  void __fastcall Tree_ExportClick(TObject *Sender);
  void __fastcall IPrintDialog1PaintSample(TIPrintDialog *Sender,
    TCanvas *PageCanvas, const TRect &PaintRect,
    const TRect &PaperSize);
  void __fastcall IPrintDialog1Show(TObject *Sender);
  void __fastcall TreeViewDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
  void __fastcall TreeViewDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
  void __fastcall LegendPlacementClick(TObject *Sender);
  void __fastcall Legend_ShowClick(TObject *Sender);
  void __fastcall PopupMenu4Popup(TObject *Sender);
  void __fastcall SupportActionExecute(TObject *Sender);
  void __fastcall ZoomXInActionExecute(TObject *Sender);
  void __fastcall ZoomXOutActionExecute(TObject *Sender);
  void __fastcall ZoomYInActionExecute(TObject *Sender);
  void __fastcall ZoomYOutActionExecute(TObject *Sender);
  void __fastcall InsertRelationActionExecute(TObject *Sender);
  void __fastcall CustomFunctionsActionExecute(TObject *Sender);
  void __fastcall SaveDialogEx1Help(TObject *Sender);
  void __fastcall PlacementClick(TObject *Sender);
  void __fastcall PopupMenu3Popup(TObject *Sender);
  void __fastcall PopupMenu1Popup(TObject *Sender);
  void __fastcall ApplicationEventsException(TObject *Sender,
          Exception *E);
  void __fastcall RotationClick(TObject *Sender);
  void __fastcall AnimateActionExecute(TObject *Sender);
  void __fastcall ImportGraphFileActionExecute(TObject *Sender);
  void __fastcall ImportPointSeriesActionExecute(TObject *Sender);
  void __fastcall Legend_FontClick(TObject *Sender);
  void __fastcall Panel6UnDock(TObject *Sender, TControl *Client,
          TWinControl *NewTarget, bool &Allow);
  void __fastcall Panel6DockDrop(TObject *Sender, TDragDockObject *Source,
          int X, int Y);
  void __fastcall Panel6DockOver(TObject *Sender, TDragDockObject *Source,
          int X, int Y, TDragState State, bool &Accept);
  void __fastcall Panel6GetSiteInfo(TObject *Sender, TControl *DockClient,
          TRect &InfluenceRect, TPoint &MousePos, bool &CanDock);
  void __fastcall Panel4GetSiteInfo(TObject *Sender, TControl *DockClient,
          TRect &InfluenceRect, TPoint &MousePos, bool &CanDock);
  void __fastcall Timer2Timer(TObject *Sender);
  void __fastcall TreeViewEndDrag(TObject *Sender, TObject *Target, int X,
          int Y);
  void __fastcall MainMenuChange(TObject *Sender, TMenuItem *Source,
          bool Rebuild);
  void __fastcall StatusBar1Hint(TObject *Sender);
  void __fastcall Image1Click(TObject *Sender);
  void __fastcall FormShortCut(TWMKey &Msg, bool &Handled);
  void __fastcall TreeViewMouseLeave(TObject *Sender);
  void __fastcall SaveDialogEx1Close(TObject *Sender);
  void __fastcall ScriptDocActionExecute(TObject *Sender);


private:	// User declarations
  friend class TAddView;

  //Form pos in standard size
  int StdTop, StdLeft, StdWidth, StdHeight;
  String StatusError;
  TColor StatusErrorColor;
  int IsResizing;     // >0 when the window is being resized or dragged
  bool Reseized;      // true if windo was resized
  int Updating;
  Func32::TDblPoint MovePos; //Position when moving system with mouse; Coordinate where left button was pressed
  TPoint LastMousePos; //Last coordiante for mouse
  String DefaultToolBar; //List of tool bar buttons from design time
  String StartToolBar;   //The toolbar shown a startup; Not necessarily the one saved
  boost::shared_ptr<TTextLabel> MovingLabel; //Label being moved
  TLabelPlacement MovingLabelPlacement; //Used while moving a text label
  TLegendPlacement LegendPlacement; //Used while moving the legend
  TCursorState CursorState;
  boost::scoped_ptr<class TForm9> Form9;
  TData PreviewData;
  boost::scoped_ptr<TDraw> PreviewDraw; //Only availble while open dialog is shown
  const int FixedImages; //Number of fixed images in ImageList1 (The rest are dynamically added and deleted)
  boost::scoped_ptr<struct TImageOptions> ActionImageOptions; //Used when saving as image from menu
  TGraphElemPtr Selected;

  int StatusIcon;
  int xZoom, yZoom;
  bool AbortPrinting; //Set in OnClose to signal that we should abort printing
  TPoint ImagePos;   //The actual Position of Image2. The real position may vary from this when it snaps
  TPoint MouseDownPos; //Set from OnMouseDown on Image1

  bool AskSave();
  void LoadSettings();
  void SaveSettings();
  void CreateToolBar(String Str);
  String GetToolBar();
  void Initialize();
  void SetCursorState(TCursorState State);
  void EditLabel(const boost::shared_ptr<TTextLabel> &Label);
  void CheckForUpdate(bool StartupCheck);
  void Translate();
  void DeleteGraphElem(const boost::shared_ptr<TGraphElem> &GraphElem);
  TTreeNode* GetNode(const TGraphElemPtr &Elem);
  void CopyAsImageToClipboard();
  TTreeNode* GetRootNode(unsigned Index);
  bool DoSave(const String &FileName, bool Remember);
  void MoveAndSnapLegend(int dx, int dy, bool Snap);
  void MoveAndSnapLabel(int dx, int dy, bool Snap);
  void MoveLabel(int X, int Y, TLabelPlacement Placement, bool Snap);
  void __fastcall DebugLine(System::TObject* Sender, const String Line, bool &Discard); //Write debug text from dxGetText
  void ChangeVisible(boost::shared_ptr<TGraphElem> GraphElem);
  void HandleCommandLine();
  void SetPanelCoordText();

  int AddImage(int Index, TColor Color);
  int AddImage(TColor Color, TBrushStyle Style);

  //Declare message handlers
  void __fastcall WMDropFiles(TMessage &Message);
  void __fastcall WMEnterSizeMove(TMessage &Message);
  void __fastcall WMExitSizeMove(TMessage &Message);
  void __fastcall WMUser(TMessage &Message);
  void __fastcall ExecuteFunction(TMessage &Message);

  //Workaround for bug in VCL
  void __fastcall WndProc(TMessage &Message);

BEGIN_MESSAGE_MAP
  VCL_MESSAGE_HANDLER(WM_DROPFILES, TMessage, WMDropFiles)
  VCL_MESSAGE_HANDLER(WM_ENTERSIZEMOVE,TMessage, WMEnterSizeMove)
  VCL_MESSAGE_HANDLER(WM_EXITSIZEMOVE,TMessage, WMExitSizeMove)
  VCL_MESSAGE_HANDLER(WM_USER, TMessage, WMUser)
  VCL_MESSAGE_HANDLER(WM_USER+1, TMessage, ExecuteFunction);
END_MESSAGE_MAP(TForm)

public:		// User declarations
  TData Data;
  TDraw Draw;

  __fastcall TForm1(TComponent* Owner);
  __fastcall ~TForm1();
  void Redraw();
  void UpdateMenu();
  bool ZoomWindow(double xMin, double xMax, double yMin, double yMax, bool Update=true, bool SaveUndo=true);
  bool Zoom(TPoint Pos, double ZoomRate, bool ChangeUnits=true);
  bool Zoom(double x, double y, double xZoomRate, double yZoomRate, bool ChangeUnits=true);
  bool Zoom(double ZoomRate, bool ChangeUnits=true);
  bool Zoom(double xZoomRate, double yZoomRate, bool ChangeUnits=true);
  bool ZoomPoint(const TPoint &Point, double ZoomRate, bool ChangeUnits);
  void ShowStatusError(const String &Str, TColor = clRed, unsigned Timeout = 5000);
  void CancelStatusError();
  void ShowStatusMessage(const String &Str, bool AutoHint=false);
  void SetStatusIcon(int AStatusIcon);

  void UpdateTreeView(const boost::shared_ptr<TGraphElem> &Selected = boost::shared_ptr<TGraphElem>());

  void SetCross(int X, int Y);
  void SetCrossPos(long double x, long double y);
  void ChangeLanguage(const String &Ext);
  void ActivateOleUserInterface();

  boost::shared_ptr<TGraphElem> GetGraphElem(TTreeNode *Node);

  void __fastcall UpdateEval();
  void __fastcall BeginUpdate();
  void __fastcall EndUpdate();

  bool LoadFromFile(const String &FileName, bool AddToRecent = true, bool ShowErrorMessages = true);
  void LoadDefault();

  void SetSelected(const TGraphElemPtr &Elem) {TreeView->Selected = GetNode(Elem);}
  TGraphElemPtr GetSelected() {return GetGraphElem(TreeView->Selected);}
};

//---------------------------------------------------------------------------
extern TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
