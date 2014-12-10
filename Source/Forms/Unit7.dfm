object Form7: TForm7
  Left = 476
  Top = 259
  HelpContext = 70
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Insert f'#39'(x)'
  ClientHeight = 247
  ClientWidth = 338
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poMainFormCenter
  ShowHint = True
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 80
    Top = 216
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 3
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 168
    Top = 216
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 4
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 8
    Width = 321
    Height = 49
    Caption = 'Argument range'
    TabOrder = 0
    object Label4: TLabel
      Left = 8
      Top = 20
      Width = 26
      Height = 13
      Caption = 'From:'
      FocusControl = Edit1
    end
    object Label5: TLabel
      Left = 132
      Top = 20
      Width = 16
      Height = 13
      Caption = 'To:'
      FocusControl = Edit2
    end
    object Edit1: TMyEdit
      Left = 38
      Top = 18
      Width = 74
      Height = 21
      Hint = 'Start of range interval for the used variable.'
      TabOrder = 0
    end
    object Edit2: TMyEdit
      Left = 152
      Top = 18
      Width = 74
      Height = 21
      Hint = 'End of range interval for the used variable.'
      TabOrder = 1
    end
  end
  object GroupBox4: TGroupBox
    Left = 7
    Top = 119
    Width = 322
    Height = 84
    Caption = 'Graph properties'
    TabOrder = 2
    object Label7: TLabel
      Left = 8
      Top = 20
      Width = 47
      Height = 13
      Caption = 'Line style:'
      FocusControl = LineSelect1
    end
    object Label8: TLabel
      Left = 179
      Top = 20
      Width = 31
      Height = 13
      Alignment = taRightJustify
      Caption = 'Width:'
      FocusControl = Edit4
    end
    object Label9: TLabel
      Left = 8
      Top = 52
      Width = 27
      Height = 13
      Caption = 'Color:'
      FocusControl = ExtColorBox1
    end
    object Edit4: TEdit
      Left = 216
      Top = 18
      Width = 41
      Height = 21
      Hint = 'Width of graph in pixels.'
      TabOrder = 1
      Text = '1'
    end
    object UpDown1: TUpDown
      Left = 257
      Top = 18
      Width = 15
      Height = 21
      Associate = Edit4
      Min = 1
      Max = 20
      Position = 1
      TabOrder = 2
    end
    object LineSelect1: TLineSelect
      Left = 64
      Top = 18
      Width = 81
      Height = 22
      Hint = 'Choose which line style to use for drawing the graph.'
      TabOrder = 0
    end
    object ExtColorBox1: TExtColorBox
      Left = 64
      Top = 48
      Width = 81
      Height = 22
      Hint = 'Select the color of the graph.'
      TabOrder = 3
      AutoDroppedWidth = True
      Selected = clScrollBar
      DefaultName = 'Default'
      CustomName = 'Custom...'
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 64
    Width = 321
    Height = 49
    Caption = 'Legend text'
    TabOrder = 1
    object Label10: TLabel
      Left = 8
      Top = 20
      Width = 56
      Height = 13
      Caption = 'Description:'
      FocusControl = Edit3
    end
    object Edit3: TMyEdit
      Left = 72
      Top = 18
      Width = 241
      Height = 21
      Hint = 
        'Write a descriptive text to show in the legend. If empty the equ' +
        'ation will be used.'
      TabOrder = 0
    end
  end
  object Button3: TButton
    Left = 257
    Top = 216
    Width = 75
    Height = 25
    Caption = 'Help'
    TabOrder = 5
    OnClick = Button3Click
  end
end
