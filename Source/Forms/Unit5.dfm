object Form5: TForm5
  Left = 379
  Top = 228
  HelpContext = 50
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Insert function'
  ClientHeight = 424
  ClientWidth = 353
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poMainFormCenter
  ShowHint = True
  DesignSize = (
    353
    424)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 18
    Width = 67
    Height = 13
    Caption = 'Function type:'
    FocusControl = ComboBox1
  end
  object Button1: TButton
    Left = 89
    Top = 392
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 6
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 177
    Top = 392
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 7
  end
  object ComboBox1: TComboBox
    Left = 88
    Top = 16
    Width = 250
    Height = 21
    Hint = 'Select the function type to graph.'
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    OnChange = ComboBox1Change
    OnKeyPress = ComboBox1KeyPress
    Items.Strings = (
      'Standard function   y=f(x)'
      'Parametric function x(t), y(t)'
      'Polar function         r=f(t)')
  end
  object GroupBox1: TGroupBox
    Left = 16
    Top = 48
    Width = 322
    Height = 73
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Function equation'
    TabOrder = 1
    DesignSize = (
      322
      73)
    object Label2: TLabel
      Left = 8
      Top = 20
      Width = 20
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'x(t)='
      ParentBiDiMode = False
    end
    object Label3: TLabel
      Left = 8
      Top = 48
      Width = 20
      Height = 13
      BiDiMode = bdLeftToRight
      Caption = 'y(t)='
      ParentBiDiMode = False
    end
    object Edit1: TMyEdit
      Left = 32
      Top = 16
      Width = 282
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      BiDiMode = bdLeftToRight
      ParentBiDiMode = False
      TabOrder = 0
    end
    object Edit2: TMyEdit
      Left = 32
      Top = 44
      Width = 282
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      BiDiMode = bdLeftToRight
      ParentBiDiMode = False
      TabOrder = 1
    end
  end
  object GroupBox2: TGroupBox
    Left = 16
    Top = 128
    Width = 322
    Height = 49
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Argument range'
    TabOrder = 2
    DesignSize = (
      322
      49)
    object Label4: TLabel
      Left = 8
      Top = 20
      Width = 26
      Height = 13
      Caption = 'From:'
      FocusControl = Edit3
    end
    object Label5: TLabel
      Left = 124
      Top = 20
      Width = 16
      Height = 13
      Caption = 'To:'
      FocusControl = Edit4
    end
    object Label6: TLabel
      Left = 232
      Top = 20
      Width = 30
      Height = 13
      Caption = 'Steps:'
      FocusControl = Edit5
    end
    object Edit3: TMyEdit
      Left = 38
      Top = 18
      Width = 74
      Height = 21
      Hint = 'Start of range interval for the used variable.'
      TabOrder = 0
    end
    object Edit4: TMyEdit
      Left = 144
      Top = 18
      Width = 74
      Height = 21
      Hint = 'End of range interval for the used variable.'
      TabOrder = 1
    end
    object Edit5: TMyEdit
      Left = 264
      Top = 18
      Width = 49
      Height = 21
      Hint = 'Number of calculation steps used to draw the graph.'
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
  end
  object GroupBox4: TGroupBox
    Left = 16
    Top = 296
    Width = 322
    Height = 84
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Graph properties'
    TabOrder = 5
    object GridPanelEx1: TGridPanelEx
      Left = 2
      Top = 15
      Width = 318
      Height = 67
      Align = alClient
      Alignment = taLeftJustify
      BevelOuter = bvNone
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
          Value = 100.000000000000000000
        end
        item
          SizeStyle = ssAuto
          Value = 50.000000000000000000
        end
        item
          SizeStyle = ssAuto
        end>
      ControlCollection = <
        item
          Column = 0
          Control = Label7
          Row = 0
        end
        item
          Column = 1
          Control = LineSelect1
          Row = 0
        end
        item
          Column = 3
          Control = Label13
          Row = 0
        end
        item
          Column = 4
          Control = ComboBox4
          Row = 0
        end
        item
          Column = 0
          Control = Label9
          Row = 1
        end
        item
          Column = 1
          Control = ExtColorBox1
          Row = 1
        end
        item
          Column = 3
          Control = Label8
          Row = 1
        end
        item
          Column = 4
          Control = Panel1
          Row = 1
        end>
      Padding.Left = 3
      Padding.Right = 3
      RowCollection = <
        item
          Value = 50.000000000000000000
        end
        item
          Value = 50.000000000000000000
        end>
      TabOrder = 0
      DesignSize = (
        318
        67)
      object Label7: TLabel
        Left = 6
        Top = 10
        Width = 47
        Height = 13
        Anchors = []
        Caption = 'Line style:'
        FocusControl = LineSelect1
        Layout = tlCenter
      end
      object LineSelect1: TLineSelect
        Left = 59
        Top = 6
        Width = 81
        Height = 21
        Hint = 'Choose which line style to use for drawing the graph.'
        TabOrder = 0
        Anchors = [akLeft]
        ItemHeight = 15
      end
      object Label13: TLabel
        Left = 167
        Top = 0
        Width = 51
        Height = 33
        Align = alLeft
        Caption = 'Draw type:'
        FocusControl = ComboBox4
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object ComboBox4: TComboBox
        Left = 224
        Top = 6
        Width = 88
        Height = 21
        Hint = 
          'Choose Lines if you want the calculated points for the graph to ' +
          'be connected. Choose Dots if you only want to show a dot at each' +
          ' point.'
        Style = csDropDownList
        Anchors = [akLeft, akTop, akBottom]
        ItemIndex = 0
        TabOrder = 1
        Text = 'Automatic'
        OnChange = ComboBox4Change
        Items.Strings = (
          'Automatic'
          'Dots'
          'Lines')
      end
      object Label9: TLabel
        Left = 6
        Top = 33
        Width = 27
        Height = 34
        Align = alLeft
        Caption = 'Color:'
        FocusControl = ExtColorBox1
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object ExtColorBox1: TExtColorBox
        Left = 59
        Top = 39
        Width = 81
        Height = 22
        Hint = 'Select the color of the graph.'
        Anchors = [akLeft]
        TabOrder = 2
        AutoDroppedWidth = True
        Selected = clScrollBar
        DefaultName = 'Default'
        CustomName = 'Custom...'
      end
      object Label8: TLabel
        Left = 167
        Top = 33
        Width = 31
        Height = 34
        Align = alLeft
        Caption = 'Width:'
        FocusControl = Edit6
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object Panel1: TPanel
        Left = 224
        Top = 33
        Width = 88
        Height = 34
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 3
        DesignSize = (
          88
          34)
        object Edit6: TEdit
          Left = 0
          Top = 8
          Width = 49
          Height = 21
          Hint = 'Width of graph in pixels.'
          TabOrder = 0
          Text = '1'
        end
        object UpDown1: TUpDown
          Left = 49
          Top = 8
          Width = 15
          Height = 21
          Anchors = [akTop, akRight]
          Associate = Edit6
          Min = 1
          Max = 20
          Position = 1
          TabOrder = 1
        end
      end
    end
  end
  object Button3: TButton
    Left = 265
    Top = 392
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Help'
    TabOrder = 8
    OnClick = Button3Click
  end
  object GroupBox3: TGroupBox
    Left = 16
    Top = 240
    Width = 321
    Height = 49
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Legend text'
    TabOrder = 4
    DesignSize = (
      321
      49)
    object Label10: TLabel
      Left = 8
      Top = 20
      Width = 56
      Height = 13
      Caption = 'Description:'
      FocusControl = Edit7
    end
    object Edit7: TMyEdit
      Left = 72
      Top = 18
      Width = 241
      Height = 21
      Hint = 
        'Write a descriptive text to show in the legend and function list' +
        '. If empty the formula will be used.'
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
    end
  end
  object GroupBox5: TGroupBox
    Left = 16
    Top = 184
    Width = 321
    Height = 49
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Endpoints'
    TabOrder = 3
    DesignSize = (
      321
      49)
    object Label11: TLabel
      Left = 8
      Top = 20
      Width = 25
      Height = 13
      Caption = 'Start:'
      FocusControl = ComboBox2
    end
    object Label12: TLabel
      Left = 216
      Top = 20
      Width = 22
      Height = 13
      Anchors = [akTop, akRight]
      Caption = 'End:'
      FocusControl = ComboBox3
    end
    object ComboBox2: TExtComboBox
      Left = 40
      Top = 16
      Width = 65
      Height = 22
      Hint = 
        'Select marker to show at the start of the interval. Only used if' +
        ' an argument range is specified.'
      Style = csOwnerDrawFixed
      TabOrder = 0
      OnDrawItem = ComboBoxDrawItem
      Items.Strings = (
        ''
        ''
        ''
        ''
        ''
        ''
        ''
        '')
      AutoDroppedWidth = True
    end
    object ComboBox3: TExtComboBox
      Left = 248
      Top = 16
      Width = 65
      Height = 22
      Hint = 
        'Select marker to show at the end of the interval. Only used if a' +
        'n argument range is specified.'
      Style = csOwnerDrawFixed
      Anchors = [akTop, akRight]
      TabOrder = 1
      OnDrawItem = ComboBoxDrawItem
      Items.Strings = (
        ''
        ''
        ''
        ''
        ''
        ''
        ''
        '')
      AutoDroppedWidth = True
    end
  end
end
