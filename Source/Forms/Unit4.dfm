object Form4: TForm4
  Left = 428
  Top = 192
  HelpContext = 40
  BiDiMode = bdLeftToRight
  BorderIcons = [biMinimize]
  BorderStyle = bsDialog
  Caption = 'Options'
  ClientHeight = 333
  ClientWidth = 281
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Padding.Left = 10
  Padding.Top = 10
  Padding.Right = 10
  Padding.Bottom = 10
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poMainFormCenter
  ShowHint = True
  DesignSize = (
    281
    333)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 10
    Top = 300
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 102
    Top = 300
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object Button3: TButton
    Left = 196
    Top = 300
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Help'
    TabOrder = 3
    OnClick = Button3Click
  end
  object GridPanelEx1: TGridPanelEx
    Left = 13
    Top = 10
    Width = 261
    Height = 280
    BevelInner = bvRaised
    BevelOuter = bvLowered
    ColumnCollection = <
      item
        SizeStyle = ssAuto
        Value = 50.000000000000000000
      end
      item
        SizeStyle = ssAuto
        Value = 100.000000000000000000
      end
      item
        SizeStyle = ssAuto
        Value = 100.000000000000000000
      end>
    Constraints.MinWidth = 261
    ControlCollection = <
      item
        Column = 0
        Control = Label1
        Row = 0
      end
      item
        Column = 1
        Control = Panel1
        Row = 0
      end
      item
        Column = 2
        Control = RadioGroup1
        Row = 0
        RowSpan = 3
      end
      item
        Column = 0
        Control = Label2
        Row = 1
      end
      item
        Column = 1
        Control = Panel2
        Row = 1
      end
      item
        Column = 0
        Control = Label3
        Row = 2
      end
      item
        Column = 1
        Control = Panel3
        Row = 2
      end
      item
        Column = 0
        Control = Label5
        Row = 3
      end
      item
        Column = 1
        ColumnSpan = 2
        Control = ComboBox1
        Row = 3
      end
      item
        Column = 0
        Control = Label4
        Row = 4
      end
      item
        Column = 1
        ColumnSpan = 2
        Control = ComboBox2
        Row = 4
      end
      item
        Column = 0
        ColumnSpan = 3
        Control = CheckBox2
        Row = 6
      end
      item
        Column = 0
        ColumnSpan = 3
        Control = CheckBox3
        Row = 7
      end
      item
        Column = 0
        ColumnSpan = 3
        Control = CheckBox4
        Row = 8
      end
      item
        Column = 0
        ColumnSpan = 2
        Control = CheckBox5
        Row = 5
      end
      item
        Column = 2
        Control = Edit4
        Row = 5
      end>
    Padding.Left = 3
    Padding.Right = 2
    RowCollection = <
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 32.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 28.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 28.000000000000000000
      end
      item
        SizeStyle = ssAbsolute
        Value = 28.000000000000000000
      end>
    TabOrder = 0
    AutoSize = True
    DesignSize = (
      261
      280)
    object Label1: TLabel
      Left = 8
      Top = 11
      Width = 46
      Height = 13
      Anchors = [akLeft]
      Caption = 'Decimals:'
      FocusControl = Edit1
      ExplicitTop = 10
    end
    object Panel1: TPanel
      Left = 91
      Top = 7
      Width = 61
      Height = 21
      Anchors = [akLeft]
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 0
      DesignSize = (
        61
        21)
      object Edit1: TEdit
        Left = 0
        Top = 0
        Width = 46
        Height = 21
        Hint = 'Number of decimals used when results are shown.'
        Anchors = [akLeft]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        Text = '4'
      end
      object UpDown1: TUpDown
        Left = 46
        Top = 0
        Width = 15
        Height = 21
        Anchors = []
        Associate = Edit1
        Max = 15
        Position = 4
        TabOrder = 1
      end
    end
    object RadioGroup1: TRadioGroupEx
      AlignWithMargins = True
      Left = 160
      Top = 12
      Width = 92
      Height = 76
      Hint = 'Format used when complex numbers are shown.'
      Anchors = [akTop, akRight, akBottom]
      Caption = 'Complex form'
      ItemIndex = 1
      Items.Strings = (
        'Real'
        'Rectangular'
        'Polar')
      ParentShowHint = False
      ShowHint = True
      TabOrder = 10
    end
    object Label2: TLabel
      Left = 8
      Top = 43
      Width = 59
      Height = 13
      Anchors = [akLeft]
      Caption = 'Recent files:'
      FocusControl = Edit2
      ExplicitTop = 95
    end
    object Panel2: TPanel
      Left = 91
      Top = 39
      Width = 61
      Height = 21
      Anchors = [akLeft]
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        61
        21)
      object Edit2: TEdit
        Left = 0
        Top = 0
        Width = 46
        Height = 21
        Hint = 'The number of most recent used files shown in the File menu.'
        Anchors = [akLeft]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        Text = '4'
      end
      object UpDown2: TUpDown
        Left = 46
        Top = 0
        Width = 15
        Height = 21
        Associate = Edit2
        Max = 9
        Position = 4
        TabOrder = 1
      end
    end
    object Label3: TLabel
      Left = 8
      Top = 75
      Width = 78
      Height = 13
      Anchors = [akLeft]
      Caption = 'Max undo steps:'
      FocusControl = Edit3
    end
    object Panel3: TPanel
      Left = 91
      Top = 71
      Width = 61
      Height = 21
      Anchors = [akLeft]
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 2
      DesignSize = (
        61
        21)
      object Edit3: TEdit
        Left = 0
        Top = 0
        Width = 46
        Height = 21
        Hint = 
          'Maximum number of undo steps saved. Set this to 0 to prevent und' +
          'o.'
        Anchors = [akLeft]
        TabOrder = 0
        Text = '50'
      end
      object UpDown3: TUpDown
        Left = 46
        Top = 0
        Width = 15
        Height = 21
        Associate = Edit3
        Max = 1000
        Increment = 10
        Position = 50
        TabOrder = 1
      end
    end
    object Label5: TLabel
      Left = 8
      Top = 107
      Width = 52
      Height = 13
      Anchors = [akLeft]
      Caption = 'Font scale:'
      FocusControl = ComboBox1
      ExplicitTop = 122
    end
    object ComboBox1: TComboBox
      Left = 91
      Top = 103
      Width = 133
      Height = 21
      Hint = 
        'This is used to scale the user interface if you are having probl' +
        'ems reading the text.'
      AutoComplete = False
      Anchors = [akLeft]
      ItemIndex = 1
      TabOrder = 3
      Text = '100%'
      OnKeyPress = ComboBox1KeyPress
      Items.Strings = (
        '75%'
        '100%'
        '125%'
        '150%'
        '200%')
    end
    object Label4: TLabel
      Left = 8
      Top = 139
      Width = 51
      Height = 13
      Anchors = [akLeft]
      Caption = 'Language:'
      FocusControl = ComboBox2
      ExplicitTop = 121
    end
    object ComboBox2: TComboBox
      Left = 91
      Top = 135
      Width = 133
      Height = 21
      Hint = 'Select the language to use for the user interface.'
      Style = csDropDownList
      Anchors = [akLeft]
      Sorted = True
      TabOrder = 4
    end
    object CheckBox2: TCheckBoxEx
      Left = 8
      Top = 199
      Width = 77
      Height = 17
      Hint = 'Show the tooltip when the cursor is over an item.'
      Anchors = [akLeft]
      Caption = 'Show tooltip'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
      ExplicitTop = 227
    end
    object CheckBox3: TCheckBoxEx
      Left = 8
      Top = 227
      Width = 133
      Height = 17
      Hint = 
        'Save the size of the main window when the program exits. This is' +
        ' used the next time Graph is run.'
      Anchors = [akLeft]
      Caption = 'Save workspace on exit'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 8
      ExplicitTop = 255
    end
    object CheckBox4: TCheckBoxEx
      Left = 8
      Top = 255
      Width = 216
      Height = 17
      Hint = 
        'If checked the program will contact a web server when the progra' +
        'm is started to see if a newer version is available.'
      Anchors = [akLeft]
      Caption = 'Check for new version of Graph at startup'
      TabOrder = 9
      ExplicitTop = 283
    end
    object CheckBox5: TCheckBoxEx
      Left = 8
      Top = 169
      Width = 143
      Height = 17
      Hint = 
        'Use a custom decimal separator instead of the default from the W' +
        'indows Regional settings.'
      Anchors = [akLeft]
      Caption = 'Custom decimal separator:'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      OnClick = CheckBox5Click
    end
    object Edit4: TEdit
      Left = 157
      Top = 167
      Width = 20
      Height = 21
      Hint = 'Decimal separator used when exporting to other programs.'
      Anchors = [akLeft]
      MaxLength = 1
      TabOrder = 6
      Text = '.'
    end
  end
end
