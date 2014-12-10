object Form17: TForm17
  Left = 343
  Top = 154
  HelpContext = 90
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Custom functions/constants'
  ClientHeight = 364
  ClientWidth = 442
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  DesignSize = (
    442
    364)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 92
    Top = 331
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 180
    Top = 331
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object Button4: TButton
    Left = 356
    Top = 331
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Help'
    TabOrder = 4
    OnClick = Button4Click
  end
  object Button3: TButton
    Left = 268
    Top = 331
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Apply'
    TabOrder = 3
    OnClick = Button3Click
  end
  object Grid1: TGrid
    Left = 16
    Top = 16
    Width = 413
    Height = 300
    Anchors = [akLeft, akTop, akRight, akBottom]
    BiDiMode = bdLeftToRight
    ColCount = 2
    DefaultRowHeight = 16
    FixedCols = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected, goColSizing, goEditing, goTabs, goAlwaysShowEditor, goThumbTracking]
    ParentBiDiMode = False
    PopupMenu = PopupMenu1
    TabOrder = 0
    AutoAddRows = True
    ExportFixedRows = False
    EditorPopupMenu = PopupMenu1
    TitleCaptions.Strings = (
      'Names'
      'Definition')
    OnEditorKeyPress = Grid1EditorKeyPress
    ColWidths = (
      76
      328)
  end
  object PopupMenu1: TPopupActionBar
    Top = 328
    object Popup_Cut: TMenuItem
      Caption = 'Cut'
      Hint = 'Cut the selected text to the clipboard.'
      ShortCut = 16472
      OnClick = Popup_CutClick
    end
    object Popup_Copy: TMenuItem
      Caption = 'Copy'
      Hint = 'Copy the selected text to the clipboard.'
      ShortCut = 16451
      OnClick = Popup_CopyClick
    end
    object Popup_Paste: TMenuItem
      Caption = 'Paste'
      Hint = 
        'Paste text from the clipboard into the grid. Other cells may be ' +
        'overwritten by the text from the clipboard.'
      ShortCut = 16470
      OnClick = Popup_PasteClick
    end
    object Popup_Delete: TMenuItem
      Caption = 'Delete'
      Hint = 'Erase selected cells.'
      ShortCut = 16430
      OnClick = Popup_DeleteClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object Popup_Insert: TMenuItem
      Caption = 'Insert row'
      Hint = 'Inserts a row in the grid moving rows below one down.'
      OnClick = Popup_InsertClick
    end
    object Popup_Remove: TMenuItem
      Caption = 'Remove row'
      Hint = 'Removes the selected rows.'
      OnClick = Popup_RemoveClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Popup_Select: TMenuItem
      Caption = 'Select all'
      Hint = 'Select all cells.'
      ShortCut = 16449
      OnClick = Popup_SelectClick
    end
    object Popup_Import: TMenuItem
      Caption = 'Import from file...'
      Hint = 'Import data from a file and place it at the selected cell.'
      OnClick = Popup_ImportClick
    end
    object Popup_Export: TMenuItem
      Caption = 'Export to file...'
      Hint = 'Export selected data to a file.'
      OnClick = Popup_ExportClick
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 32
    Top = 328
  end
  object SaveDialog1: TSaveDialogEx
    DefaultExt = 'csv'
    Options = [ofOverwritePrompt, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 64
    Top = 328
  end
end
