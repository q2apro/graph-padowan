object Form22: TForm22
  Left = 335
  Top = 276
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'Python interpreter'
  ClientHeight = 206
  ClientWidth = 498
  Color = clBtnFace
  DragKind = dkDock
  DragMode = dmAutomatic
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object IRichEdit1: TIRichEdit
    Left = 0
    Top = 0
    Width = 498
    Height = 206
    WrapType = wtChar
    HideScrollBars = False
    ScrollBars = ssVertical
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    PopupMenu = PopupMenu1
    ParentFont = False
    TabOrder = 0
    OnKeyDown = IRichEdit1KeyDown
    OnEnter = IRichEdit1Enter
    OnExit = IRichEdit1Exit
    OnProtectChange = IRichEdit1ProtectChange
  end
  object PopupMenu1: TPopupActionBar
    Left = 8
    Top = 8
    object Clear1: TMenuItem
      Caption = 'Clear'
      OnClick = Clear1Click
    end
  end
end
