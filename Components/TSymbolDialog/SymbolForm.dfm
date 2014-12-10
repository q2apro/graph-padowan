object SymbolFrm: TSymbolFrm
  Left = 339
  Top = 219
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Insert symbol'
  ClientHeight = 311
  ClientWidth = 524
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  DesignSize = (
    524
    311)
  PixelsPerInch = 96
  TextHeight = 13
  object Image2: TImage
    Left = 439
    Top = 120
    Width = 73
    Height = 65
    Anchors = [akTop, akRight]
  end
  object Label1: TLabel
    Left = 8
    Top = 12
    Width = 53
    Height = 13
    Caption = 'Font name:'
    FocusControl = ComboBox1
  end
  object Label2: TLabel
    Left = 216
    Top = 12
    Width = 36
    Height = 13
    Alignment = taRightJustify
    Caption = 'Subset:'
    FocusControl = ComboBox2
  end
  object Button1: TButton
    Left = 439
    Top = 79
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Close'
    TabOrder = 4
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 439
    Top = 39
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Insert'
    Default = True
    TabOrder = 3
  end
  object ComboBox1: TComboBox
    Left = 64
    Top = 8
    Width = 129
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    OnSelect = ComboBox1Select
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 292
    Width = 524
    Height = 19
    Panels = <
      item
        Width = 400
      end
      item
        Width = 50
      end>
    SizeGrip = False
  end
  object FocusPanel1: TFocusPanel
    Left = 8
    Top = 40
    Width = 403
    Height = 243
    BevelInner = bvLowered
    BevelOuter = bvNone
    TabOrder = 2
    TabStop = True
    OnKeyDown = FocusPanel1KeyDown
    object Image1: TImage
      Left = 1
      Top = 1
      Width = 401
      Height = 241
      Align = alClient
      OnMouseDown = Image1MouseDown
      OnMouseMove = Image1MouseMove
    end
  end
  object ScrollBar1: TScrollBar
    Left = 410
    Top = 40
    Width = 16
    Height = 243
    Kind = sbVertical
    LargeChange = 12
    Max = 205
    PageSize = 12
    TabOrder = 6
    TabStop = False
    OnChange = ScrollBar1Change
    OnScroll = ScrollBar1Scroll
  end
  object ComboBox2: TComboBox
    Left = 256
    Top = 8
    Width = 170
    Height = 21
    Style = csDropDownList
    TabOrder = 1
    OnChange = ComboBox2Change
  end
end
