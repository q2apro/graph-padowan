object Form15: TForm15
  Left = 412
  Top = 162
  HelpContext = 150
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  BiDiMode = bdLeftToRight
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Calculate table'
  ClientHeight = 469
  ClientWidth = 322
  Color = clBtnFace
  Constraints.MinHeight = 494
  Constraints.MinWidth = 328
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poMainFormCenter
  ShowHint = True
  OnResize = FormResize
  DesignSize = (
    322
    469)
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 16
    Top = 50
    Width = 17
    Height = 13
    Caption = 'dx='
  end
  object Label1: TLabel
    Left = 16
    Top = 18
    Width = 26
    Height = 13
    Caption = 'From:'
    FocusControl = Edit1
  end
  object Label2: TLabel
    Left = 138
    Top = 18
    Width = 16
    Height = 13
    Caption = 'To:'
    FocusControl = Edit2
  end
  object Edit3: TMyEdit
    Left = 48
    Top = 48
    Width = 57
    Height = 21
    Hint = 
      'The step value. This is the difference between two adjoining val' +
      'ues in the defined interval.'
    TabOrder = 2
    Text = '0.1'
  end
  object Button1: TButton
    Left = 134
    Top = 432
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Close'
    ModalResult = 2
    TabOrder = 5
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 232
    Top = 32
    Width = 75
    Height = 25
    Caption = 'Calc'
    Default = True
    TabOrder = 3
    OnClick = Button2Click
  end
  object Edit1: TMyEdit
    Left = 48
    Top = 16
    Width = 57
    Height = 21
    Hint = 'The first x-value in the interval.'
    TabOrder = 0
    Text = '-10'
  end
  object Edit2: TMyEdit
    Left = 160
    Top = 16
    Width = 57
    Height = 21
    Hint = 'The last x-value in the interval.'
    TabOrder = 1
    Text = '10'
  end
  object Button3: TButton
    Left = 230
    Top = 432
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Help'
    TabOrder = 6
    OnClick = Button3Click
  end
  object Grid1: TGrid
    Left = 16
    Top = 80
    Width = 289
    Height = 337
    Anchors = [akLeft, akTop, akRight, akBottom]
    BiDiMode = bdLeftToRight
    ColCount = 4
    DefaultRowHeight = 16
    FixedCols = 0
    RowCount = 2
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Lucida Sans Unicode'
    Font.Style = []
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected, goColSizing, goThumbTracking]
    ParentBiDiMode = False
    ParentFont = False
    PopupMenu = PopupMenu1
    TabOrder = 4
    MinColWidth = 64
    ColWidths = (
      64
      64
      64
      64)
  end
  object PopupMenu1: TPopupActionBar
    Left = 14
    Top = 423
    object Popup1_Show1: TMenuItem
      AutoCheck = True
      Caption = 'Show %s'
      Checked = True
      Hint = 'Show the %s column in the table.'
      OnClick = Popup1_Show
    end
    object Popup1_Show2: TMenuItem
      AutoCheck = True
      Caption = 'Show %s'
      Checked = True
      Hint = 'Show the %s column in the table.'
      OnClick = Popup1_Show
    end
    object Popup1_Show3: TMenuItem
      AutoCheck = True
      Caption = 'Show %s'
      Checked = True
      Hint = 'Show the %s column in the table.'
      OnClick = Popup1_Show
    end
    object Popup1_Show4: TMenuItem
      AutoCheck = True
      Caption = 'Show %s'
      Checked = True
      Hint = 'Show the %s column in the table.'
      OnClick = Popup1_Show
    end
    object Popup1_Show5: TMenuItem
      AutoCheck = True
      Caption = 'Show %s'
      Checked = True
      Hint = 'Show the %s column in the table.'
      OnClick = Popup1_Show
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Popup1_Copy: TMenuItem
      Caption = 'Copy'
      Hint = 'Copy selected cells to the clipboard.'
      ShortCut = 16451
      OnClick = Popup1_CopyClick
    end
    object Popup1_Export: TMenuItem
      Caption = 'Export to file...'
      Hint = 'Saves the selected data to a file.'
      OnClick = Popup1_ExportClick
    end
    object Popup1_Select: TMenuItem
      Caption = 'Select all'
      Hint = 'Select all cells.'
      ShortCut = 16449
      OnClick = Popup1_SelectClick
    end
  end
  object ProgressForm1: TProgressForm
    Caption = 'Calculating'
    Step = 1000
    ButtonCaption = 'Cancel'
    Left = 56
    Top = 424
  end
  object SaveDialog1: TSaveDialogEx
    DefaultExt = 'csv'
    Filter = 'CSV (Comma delimited) [*.csv]|*.csv|Text [*.txt]|*.txt'
    Options = [ofOverwritePrompt, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 96
    Top = 424
  end
end
