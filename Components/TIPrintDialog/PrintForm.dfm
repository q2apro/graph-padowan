object PrintFrm: TPrintFrm
  Left = 378
  Top = 209
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Print'
  ClientHeight = 369
  ClientWidth = 441
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
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    441
    369)
  PixelsPerInch = 96
  TextHeight = 13
  object Shape2: TShape
    Left = 328
    Top = 144
    Width = 106
    Height = 145
    Anchors = [akLeft, akTop, akRight]
    Brush.Color = clGray
    Pen.Style = psClear
  end
  object Shape1: TShape
    Left = 320
    Top = 136
    Width = 106
    Height = 145
  end
  object Label6: TLabel
    Left = 304
    Top = 304
    Width = 62
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Scale:'
    FocusControl = Edit4
  end
  object Image1: TImage
    Left = 321
    Top = 137
    Width = 104
    Height = 144
  end
  object Button1: TButton
    Left = 272
    Top = 336
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 7
  end
  object Button2: TButton
    Left = 360
    Top = 336
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 8
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 425
    Height = 113
    Caption = 'Printer'
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 24
      Width = 31
      Height = 13
      Caption = 'Name:'
      FocusControl = ComboBox1
    end
    object Label7: TLabel
      Left = 16
      Top = 48
      Width = 27
      Height = 13
      Caption = 'Type:'
    end
    object Label8: TLabel
      Left = 16
      Top = 68
      Width = 35
      Height = 13
      Caption = 'Where:'
    end
    object Label9: TLabel
      Left = 16
      Top = 88
      Width = 47
      Height = 13
      Caption = 'Comment:'
    end
    object Label11: TLabel
      Left = 80
      Top = 48
      Width = 38
      Height = 13
      Caption = 'Label11'
    end
    object Label12: TLabel
      Left = 80
      Top = 68
      Width = 38
      Height = 13
      Caption = 'Label12'
    end
    object Label13: TLabel
      Left = 80
      Top = 88
      Width = 38
      Height = 13
      Caption = 'Label13'
    end
    object ComboBox1: TComboBox
      Left = 80
      Top = 20
      Width = 233
      Height = 21
      Hint = 
        'Select the printer to use. The Properties button may be used to ' +
        'change more advanced settings for the printer.'
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = ComboBox1Change
    end
    object Button3: TButton
      Left = 328
      Top = 18
      Width = 75
      Height = 25
      Caption = 'Properties'
      TabOrder = 1
      OnClick = Button3Click
    end
  end
  object RadioGroup1: TRadioGroup
    Left = 8
    Top = 136
    Width = 113
    Height = 113
    Hint = 'Choose paper orientation.'
    Caption = 'Orientation'
    ItemIndex = 0
    Items.Strings = (
      'Portrait'
      'Landscape')
    TabOrder = 1
    OnClick = RadioGroup1Click
  end
  object RadioGroup2: TRadioGroup
    Left = 8
    Top = 264
    Width = 113
    Height = 57
    Hint = 
      'Select to print in color (or grayscale if your printer is not a ' +
      'color printer) or convert all colors to black.'
    Caption = 'Print output'
    ItemIndex = 0
    Items.Strings = (
      'Color'
      'Black && white')
    TabOrder = 3
  end
  object GroupBox2: TGroupBox
    Left = 128
    Top = 136
    Width = 177
    Height = 113
    Caption = 'Position (millimeters)'
    TabOrder = 2
    object Label3: TLabel
      Left = 12
      Top = 52
      Width = 55
      Height = 13
      Caption = 'Left margin:'
      FocusControl = Edit1
    end
    object Label4: TLabel
      Left = 12
      Top = 80
      Width = 56
      Height = 13
      Caption = 'Top margin:'
      FocusControl = Edit2
    end
    object CheckBox1: TCheckBox
      Left = 12
      Top = 24
      Width = 145
      Height = 17
      Hint = 'Center the printing area on the page.'
      Caption = 'Center on page'
      TabOrder = 0
      OnClick = CheckBox1Click
    end
    object Edit1: TEdit
      Left = 96
      Top = 48
      Width = 57
      Height = 21
      Hint = 
        'Specify the printing area of the page. The printer will print on' +
        'ly within these margins.'
      TabOrder = 1
      Text = '0'
      OnChange = Edit1Change
      OnKeyPress = EditKeyPress
    end
    object Edit2: TEdit
      Left = 96
      Top = 76
      Width = 57
      Height = 21
      Hint = 
        'Specify the printing area of the page. The printer will print on' +
        'ly within these margins.'
      TabOrder = 2
      Text = '0'
      OnChange = Edit1Change
      OnKeyPress = EditKeyPress
    end
    object UpDown1: TUpDown
      Left = 153
      Top = 48
      Width = 15
      Height = 21
      Associate = Edit1
      Max = 1000
      TabOrder = 3
    end
    object UpDown2: TUpDown
      Left = 153
      Top = 76
      Width = 15
      Height = 21
      Associate = Edit2
      Max = 1000
      TabOrder = 4
    end
  end
  object GroupBox3: TGroupBox
    Left = 128
    Top = 264
    Width = 177
    Height = 57
    TabOrder = 4
    object Label5: TLabel
      Left = 12
      Top = 24
      Width = 86
      Height = 13
      Caption = 'Number of copies:'
      FocusControl = Edit3
    end
    object Edit3: TEdit
      Left = 112
      Top = 20
      Width = 41
      Height = 21
      Hint = 'Choose number of copies to print.'
      TabOrder = 0
      Text = '1'
      OnKeyPress = EditKeyPress
    end
    object UpDown3: TUpDown
      Left = 153
      Top = 20
      Width = 15
      Height = 21
      Associate = Edit3
      Min = 1
      Position = 1
      TabOrder = 1
    end
  end
  object UpDown4: TUpDown
    Left = 409
    Top = 300
    Width = 15
    Height = 21
    Associate = Edit4
    Min = 1
    Max = 1000
    Position = 100
    TabOrder = 6
  end
  object Edit4: TEdit
    Left = 368
    Top = 300
    Width = 41
    Height = 21
    Hint = 'Enter percent of original size.'
    TabOrder = 5
    Text = '100'
    OnChange = Edit1Change
    OnKeyPress = EditKeyPress
  end
end
