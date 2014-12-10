object Form21: TForm21
  Left = 522
  Top = 274
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Placement'
  ClientHeight = 144
  ClientWidth = 193
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 20
    Width = 34
    Height = 13
    Caption = 'Corner:'
    FocusControl = ComboBox1
  end
  object Label2: TLabel
    Left = 8
    Top = 48
    Width = 11
    Height = 13
    Caption = 'x='
  end
  object Label3: TLabel
    Left = 8
    Top = 76
    Width = 11
    Height = 13
    Caption = 'y='
  end
  object TntButton1: TButton
    Left = 16
    Top = 112
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 3
    OnClick = TntButton1Click
  end
  object TntButton2: TButton
    Left = 104
    Top = 112
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 4
  end
  object ComboBox1: TComboBox
    Left = 64
    Top = 16
    Width = 121
    Height = 21
    Hint = 'Choose which corner of the label should be locked.'
    Style = csDropDownList
    TabOrder = 0
    OnChange = ComboBox1Change
    Items.Strings = (
      'Upper Left'
      'Upper Right'
      'Lower Left'
      'Lower Right')
  end
  object Edit1: TMyEdit
    Left = 64
    Top = 48
    Width = 121
    Height = 21
    Hint = 'Enter a coordinate to lock label at.'
    TabOrder = 1
  end
  object Edit2: TMyEdit
    Left = 64
    Top = 76
    Width = 121
    Height = 21
    Hint = 'Enter a coordinate to lock label at.'
    TabOrder = 2
  end
end
