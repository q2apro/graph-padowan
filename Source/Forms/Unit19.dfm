object Form19: TForm19
  Left = 409
  Top = 178
  HelpContext = 160
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Animate'
  ClientHeight = 280
  ClientWidth = 299
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
  object Label1: TLabel
    Left = 8
    Top = 20
    Width = 45
    Height = 13
    Caption = 'Constant:'
    FocusControl = ComboBox1
  end
  object ComboBox1: TComboBox
    Left = 56
    Top = 16
    Width = 137
    Height = 21
    Hint = 'Choose a constant to change between each frame.'
    Style = csDropDownList
    TabOrder = 0
    OnChange = ComboBox1Change
  end
  object Button1: TButton
    Left = 216
    Top = 16
    Width = 75
    Height = 25
    Caption = 'Animate'
    Default = True
    TabOrder = 3
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 216
    Top = 56
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Close'
    ModalResult = 2
    TabOrder = 4
  end
  object Button4: TButton
    Left = 216
    Top = 136
    Width = 75
    Height = 25
    Caption = 'Options...'
    TabOrder = 5
    Visible = False
  end
  object Button3: TButton
    Left = 216
    Top = 96
    Width = 75
    Height = 25
    Caption = 'Help'
    TabOrder = 6
    OnClick = Button3Click
  end
  object TntGroupBox1: TGroupBox
    Left = 8
    Top = 48
    Width = 193
    Height = 105
    Caption = 'Range'
    TabOrder = 1
    object Label2: TLabel
      Left = 8
      Top = 20
      Width = 26
      Height = 13
      Caption = 'From:'
      FocusControl = Edit1
    end
    object Label3: TLabel
      Left = 8
      Top = 48
      Width = 16
      Height = 13
      Caption = 'To:'
      FocusControl = Edit2
    end
    object Label4: TLabel
      Left = 8
      Top = 76
      Width = 25
      Height = 13
      Caption = 'Step:'
      FocusControl = Edit3
    end
    object Edit1: TMyEdit
      Left = 96
      Top = 16
      Width = 89
      Height = 21
      Hint = 'The first value of the constant.'
      TabOrder = 0
      Text = '1'
    end
    object Edit2: TMyEdit
      Left = 96
      Top = 44
      Width = 89
      Height = 21
      Hint = 'The last value of the constant.'
      TabOrder = 1
      Text = '10'
    end
    object Edit3: TMyEdit
      Left = 96
      Top = 72
      Width = 89
      Height = 21
      Hint = 'The chosen constant is changed by this value between each frame.'
      TabOrder = 2
      Text = '1'
    end
  end
  object TntGroupBox2: TGroupBox
    Left = 8
    Top = 168
    Width = 193
    Height = 105
    Caption = 'Frame options'
    TabOrder = 2
    object Label5: TLabel
      Left = 8
      Top = 20
      Width = 60
      Height = 13
      Caption = 'Frame width:'
      FocusControl = Edit4
    end
    object Label6: TLabel
      Left = 8
      Top = 48
      Width = 64
      Height = 13
      Caption = 'Frame height:'
      FocusControl = Edit5
    end
    object Label7: TLabel
      Left = 8
      Top = 76
      Width = 77
      Height = 13
      Caption = 'Frames/second:'
      FocusControl = Edit6
    end
    object Edit4: TMyEdit
      Left = 96
      Top = 16
      Width = 89
      Height = 21
      Hint = 'Width of each frame, in pixels.'
      TabOrder = 0
      Text = '320'
      OnKeyPress = TntEditKeyPress
    end
    object Edit5: TMyEdit
      Left = 96
      Top = 44
      Width = 89
      Height = 21
      Hint = 'Height of each frame, in pixels.'
      TabOrder = 1
      Text = '320'
      OnKeyPress = TntEditKeyPress
    end
    object Edit6: TMyEdit
      Left = 96
      Top = 72
      Width = 89
      Height = 21
      Hint = 'Number of frames per second used when the animation is played.'
      TabOrder = 2
      Text = '1'
    end
  end
  object ProgressForm1: TProgressForm
    Caption = 'Working'
    Delay = 0
    ButtonCaption = 'Cancel'
    Left = 264
    Top = 240
  end
end
