object Form13: TForm13
  Left = 366
  Top = 206
  HelpContext = 130
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Insert trendline'
  ClientHeight = 298
  ClientWidth = 508
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
  DesignSize = (
    508
    298)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 96
    Top = 264
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 4
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 184
    Top = 264
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 5
  end
  object Button3: TButton
    Left = 272
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Help'
    TabOrder = 6
    OnClick = Button3Click
  end
  object GroupBox1: TGroupBox
    Left = 360
    Top = 24
    Width = 141
    Height = 121
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Line'
    TabOrder = 1
    DesignSize = (
      141
      121)
    object Label5: TLabel
      Left = 11
      Top = 90
      Width = 31
      Height = 13
      Caption = 'Width:'
      FocusControl = Edit2
    end
    object Label6: TLabel
      Left = 11
      Top = 54
      Width = 27
      Height = 13
      Caption = 'Color:'
      FocusControl = ExtColorBox1
    end
    object Label7: TLabel
      Left = 11
      Top = 24
      Width = 26
      Height = 13
      Caption = 'Style:'
      FocusControl = LineSelect1
    end
    object Edit2: TEdit
      Left = 48
      Top = 86
      Width = 41
      Height = 21
      Hint = 'Width of graph in pixels.'
      Anchors = [akTop, akRight]
      TabOrder = 2
      Text = '1'
      OnKeyPress = EditKeyPress
    end
    object LineSelect1: TLineSelect
      Left = 48
      Top = 19
      Width = 81
      Height = 22
      Hint = 'Choose which line style to use for drawing the graph.'
      TabOrder = 0
      Anchors = [akTop, akRight]
    end
    object UpDown1: TUpDown
      Left = 89
      Top = 86
      Width = 15
      Height = 21
      Anchors = [akTop, akRight]
      Associate = Edit2
      Min = 1
      Max = 20
      Position = 1
      TabOrder = 3
    end
    object ExtColorBox1: TExtColorBox
      Left = 48
      Top = 52
      Width = 81
      Height = 22
      Hint = 'Select the color of the graph.'
      Anchors = [akTop, akRight]
      TabOrder = 1
      AutoDroppedWidth = True
      Selected = clScrollBar
      DefaultName = 'Default'
      CustomName = 'Custom...'
    end
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 345
    Height = 241
    ActivePage = TabSheet1
    TabOrder = 0
    OnChange = RadioButtonClick
    object TabSheet1: TTabSheet
      Caption = 'Built-in'
      ImageIndex = 1
      object Panel1: TPanel
        Left = 8
        Top = 8
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 0
        object Image1: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a linear function: f(x)=ax+b'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000230B0000230B0000020000000200
            000000000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFCFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF9FFFFFFFFFFFFFFFEFFFFFFFFFF
            FFFFFF3FE7FFFFFFFFFFFFCFE7FFFFFFFFFFFFF7FFFFFFFFFFFFFFF9FFFFFFFF
            FFFFFE7EFFFFFFFFFFFFFE7F3FFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFF7FFFFFF
            FFFFFFFFF9FE7FFFFFFFFFFFFEFE7FFFFFFFFFFFFF3FFFCFFFFFFFFFF3CFFFCF
            FFFFFFFFF3F7FFFFFFFFFFFFFFF9FFFFFFFFFFE7FFFEFFFFFFFFFFE7FFFF3FFF
            FFFFFFFFFFFFCFFFFFFFFFFFFFFFF7FFFFFFFFFFFFF9F9FFFFFFFFFFFFF9FEFF
            3FE7FFFFFFFFFF3F3FE7FFFFFFFFFFCFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF9
            FFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF3FFFFFFFFFFFFCFFCFFFFFFFFFFFFCFF
            F7FFFFFFFFFFFFFFF9FFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF3FFFFFFFFFFFFF
            FFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3FFFFFFFFFFFFFFF3
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object Panel2: TPanel
        Left = 104
        Top = 8
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 1
        object Image2: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a logarithmic function: f(x)=a*ln(x)+b'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000430B0000430B0000020000000200
            000000000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF73FFFFFFFFF
            FFFFF73FFFFFFFFFFFFFFBFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFF
            FFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFFFF
            FFFFFFDFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFF3FB9FFFFFFF
            FFFFF3F99FFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF3FFFFFFFFFFFFFFFDFFFFFFF
            FFFFFFFFEFFFFFFFFFFFFFFFF3FFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFFFF
            FFFFFFFFFF1FFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF3FFFFFFFFFFFFFFFCFF3F
            FFFFFFFFFFF33F3FFFFFFFFFCFF3C7FFFFFFFFFFCFFFFBFFFF9FFFFFFFFFFC7F
            FF9FFFFFFFFFFF8FFFFFFFFFFFFFFFF1FFFFFFFFFFFFFFFE3FFFFFFFFFFFFFFF
            C7FFFFFFFFFFFFFFF87FFFFFFFFFFFFFFF8FFFFFFFFFFFFCFFF0FFFFFFFFFFFC
            FFFEFFFFFFFFFFFFFFE7FFFFFFFFFFFFFFE7FFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object Panel3: TPanel
        Left = 200
        Top = 8
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 2
        object Image3: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a polynomial function: f(x)=ax^3+bx^2+cx+d'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000530B0000530B0000020000000200
            000000000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFF9BFFFFFFFFFFFFFF9DFFFFFFFFCF
            FFFFFDFFFFFFFFCFFFFFFDCFFFFFFFFFFFFFFECFFFFFFFFFFFFFFEFFFFFFFFFF
            FFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFC1FFFFFFFBFFFF9F3CF
            FFFFFCDFFFF9CFF7FFFFFCDFFFFF9FFBFFFFFFEFFFFF3FF9FFFFFFEFFFFEFFFC
            FFFFFFF7FFFDFFFEFFFFFFFBFFFBFFF2FFFFFFFBFFF7FFF37FFFFFFDFFCFFFFF
            3FFFFFFEFF9FFFFFBFFFFFFF3E7FFFFFBFFFFFFF81FFFFFFBFFFFFFFFFFFFFFF
            DFFFFFFFFFFFFFFFD9FFFFFFCFFFFFFFE9FFFFFFCFFFFFFFEFFFFFFFFFFFFFFF
            F7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFF
            F7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFF
            FDFFFFFFFFFFFFFFE5FFFFFFFFFFFFFFE5FFFFFFFFFFFFFFFEFFFFFFFFFFFFFF
            FEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFF
            FEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFFFF
            FF7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFB9FFFFFFFFFFFFFFB9FFFFFFFFFFFF
            FCBFFFFFFFFFFFFFFCDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object Panel4: TPanel
        Left = 8
        Top = 112
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 3
        object Image4: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a power function: f(x)=ax^b'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000530B0000530B0000020000000200
            000000000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFF1FFFFFFFFFFFFFFFCFF3FFFFFFFFFFFFE7F3FFFFFFFFFFFFF9FFFFFFFFF
            FFFFF3E7FFFFFFFFFFFFF3F9FFFFFFFFFFFFFFFE7FFFFFFFFFFFFFFFBFFCFFFF
            FFFFFFFFDFFCFFFFFFFFFFFFE7FFFFFFFFFFFFFFF9FFFFFFFFFFFFFFFEFFFFFF
            FFFFFFFFFF7FFFFFFFFFFFFFFF9FFFFFFFFFFFFFFFDCFFFFFFFFFFFFFFECFFFF
            FFFFFFFF9FF3FFFFFFFFFFFF9FFBFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFF
            FFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFDFFF
            FFFFFFFFFFFFEFFFFFFFFFFFFFFF2FFFFFFFFFFFFFFF37FFFFFFFFFFFFFFF7FF
            FFFFFFFFFFFFF7E7FFFFFFFFFFFFFBE7FFFFFFFFFFFFFBFFFFFFFFFFFFFFFDFF
            FFFFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7F
            FFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFF9BFFFFFFFFFFFFFF9BF
            FFFFFFFFFFFFFFBFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFEF
            FFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEE7FFFFFFFFFFFFFEE7FFFFFFFFFFFFFF7
            FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFCBFFFFFFFFFFFFFFCD
            FFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFC
            FFFFFFFFFFFFFFFECFFFFFFFFFFFFFFECFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object Panel5: TPanel
        Left = 104
        Top = 112
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 4
        object Image5: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a exponential function: f(x)=a*b^x'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000530B0000530B0000020000000200
            000000000000FFFFFF00FFCFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFFFFFFE7FFF
            FFFFFFFFFFFE7FFFFFFFF00007FFFFFFFFFFFFFFF81FFFFFFFFFFFFFFFE3FFFF
            FFFFF9FFFFFCFFE7FFFFF9FFFFFE3FE7FFFFFFFFFFFF9FFFFFFFFFFFFFFFEFFF
            FFFFFFFFFFFFE7FFFFFFFFFFCFFFF7FFFFFFFFFFCFFFF9FFFFFFFFFFFFFFFDFF
            FFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7F
            FFFFFFFFFFFF3FBFFFFFFFFFFFFF3FB3FFFFFFFFFFFFFFD3FFFFFFFFFFFFFFDF
            FFFFFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF7
            FFFFFFFFFFFFFFF7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFB
            FFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFD9FFFFFFFFFFFFFFD9FFFFFFFFFFFFFFE
            FFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFFF
            7FFFFFFFFFFFFFF3BFFFFFFFFFFFFFF3BFFFFFFFFFFFFFFFBFFFFFFFFFFFFFFF
            BFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFECFFFFFFFFFFFFFF
            ECFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF7FFFFFFFFFFFFFF
            F7FFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFFFBFFFFFFFFFFFFFF
            FBFFFFFFFFFFFFFF9BFFFFFFFFFFFFFF9DFFFFFFFFFFFFFFFDFFFFFFFFFFFFFF
            FDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object StaticText1: TStaticText
        Left = 280
        Top = 40
        Width = 33
        Height = 17
        Caption = 'Order:'
        FocusControl = Edit1
        TabOrder = 12
      end
      object RadioButton3: TMyRadioButton
        Left = 200
        Top = 80
        Width = 129
        Height = 17
        Caption = 'Polynomial'
        TabOrder = 8
        OnClick = RadioButtonClick
      end
      object RadioButton2: TMyRadioButton
        Left = 104
        Top = 80
        Width = 89
        Height = 17
        Caption = 'Logarithmic'
        TabOrder = 7
        OnClick = RadioButtonClick
      end
      object RadioButton1: TMyRadioButton
        Left = 8
        Top = 80
        Width = 81
        Height = 17
        Caption = 'Linear'
        Checked = True
        TabOrder = 6
        TabStop = True
        OnClick = RadioButtonClick
      end
      object RadioButton4: TMyRadioButton
        Left = 8
        Top = 184
        Width = 73
        Height = 17
        Caption = 'Power'
        TabOrder = 9
        OnClick = RadioButtonClick
      end
      object RadioButton5: TMyRadioButton
        Left = 104
        Top = 184
        Width = 89
        Height = 17
        Caption = 'Exponential'
        TabOrder = 10
        OnClick = RadioButtonClick
      end
      object Panel6: TPanel
        Left = 200
        Top = 112
        Width = 68
        Height = 68
        BevelOuter = bvNone
        BorderStyle = bsSingle
        TabOrder = 5
        object Image6: TImage
          Left = 0
          Top = 0
          Width = 64
          Height = 64
          Hint = 'Create a moving average as a series of straight lines.'
          Picture.Data = {
            07544269746D61703E020000424D3E020000000000003E000000280000004000
            000040000000010001000000000000020000430B0000430B0000020000000200
            000000000000FFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFCFFFFFFFF
            FFFFFFFFE7FFFFFFFFFFFFFFE7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFE7FFE7FFFFFFFFFFE7FFE7FFFFFFFFFFFFFFC7FFFFFFFFFFFFFE3BFFFFF
            FFFFFFFF9FBFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFFF
            FFFFFFFFFFF7FFFFFFFFFFFFFFF7FFFFFFFFFFE7FFFBFFFFFFFFFFE7FFFDFFFF
            FFFFFFFFFFFDFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFF87FFFFFFFFFFFFFF99FFF
            3FFFFFFFFFFFEFFF3FFFFFFFFFFFF3FFFFFFFFFFFFFF9DFFFFFFFFFFFFFF9E7F
            FFFFFFFFFFFFFF9FFFFFFFFFFFFFFFE1FFFFFFFFFFFFFCFE7FFFFFFFFFFFFCFF
            9FFFFFFFFFFFFFFFE7FFFFFFFFFFFFFFF9FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3FFFFFFFFFFFFFFF3
            FFFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFFFFFFF}
          Stretch = True
          OnClick = ImageClick
        end
      end
      object RadioButton6: TMyRadioButton
        Left = 200
        Top = 184
        Width = 129
        Height = 17
        Caption = 'Moving average'
        TabOrder = 11
        OnClick = RadioButtonClick
      end
      object StaticText2: TStaticText
        Left = 280
        Top = 144
        Width = 37
        Height = 17
        Caption = 'Period:'
        FocusControl = Edit4
        TabOrder = 15
      end
      object Edit4: TEdit
        Left = 280
        Top = 160
        Width = 33
        Height = 21
        Hint = 'Determines how many points are used for the average.'
        Enabled = False
        TabOrder = 16
        Text = '2'
        OnKeyPress = EditKeyPress
      end
      object UpDown3: TUpDown
        Left = 313
        Top = 160
        Width = 15
        Height = 21
        Associate = Edit4
        Min = 1
        Position = 2
        TabOrder = 17
      end
      object Edit1: TEdit
        Left = 280
        Top = 56
        Width = 33
        Height = 21
        Hint = 'The order of the polynomium. For example 2 means a parable.'
        Enabled = False
        TabOrder = 13
        Text = '2'
        OnKeyPress = EditKeyPress
      end
      object UpDown2: TUpDown
        Left = 313
        Top = 56
        Width = 15
        Height = 21
        Associate = Edit1
        Position = 2
        TabOrder = 14
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'User defined'
      ImageIndex = 1
      object Label1: TLabel
        Left = 8
        Top = 12
        Width = 32
        Height = 13
        Caption = 'Model:'
        FocusControl = Edit3
      end
      object ListBox1: TListBox
        Left = 8
        Top = 40
        Width = 321
        Height = 121
        ItemHeight = 13
        PopupMenu = TntPopupMenu1
        TabOrder = 1
        OnClick = ListBox1Click
        OnDblClick = Button1Click
      end
      object Button4: TButton
        Left = 64
        Top = 176
        Width = 81
        Height = 25
        Caption = 'Add model'
        TabOrder = 2
        OnClick = Button4Click
      end
      object Button5: TButton
        Left = 160
        Top = 176
        Width = 81
        Height = 25
        Caption = 'Remove model'
        Enabled = False
        TabOrder = 3
        OnClick = Button5Click
      end
      object Button6: TButton
        Left = 256
        Top = 176
        Width = 75
        Height = 25
        Caption = 'Edit model'
        Enabled = False
        TabOrder = 4
        OnClick = Button6Click
      end
      object Edit3: TMyEdit
        Left = 48
        Top = 8
        Width = 281
        Height = 21
        Hint = 'Enter a function model to base the trendline on.'
        BiDiMode = bdLeftToRight
        ParentBiDiMode = False
        TabOrder = 0
        OnChange = Edit3Change
      end
    end
  end
  object CheckBox1: TCheckBoxEx
    Left = 360
    Top = 168
    Width = 70
    Height = 17
    Hint = 
      'Type a value in the Intercept box to specify the point where you' +
      ' want the trendline to meet the y-axis. This is only available f' +
      'or linear, polynomial and exponential trendlines.'
    Caption = 'Intercept ='
    TabOrder = 2
    OnClick = CheckBox1Click
  end
  object Edit5: TMyEdit
    Left = 435
    Top = 166
    Width = 65
    Height = 21
    Hint = 
      'Type a value in the Intercept box to specify the point where you' +
      ' want the trendline to meet the y-axis. This is only available f' +
      'or linear, polynomial and exponential trendlines.'
    TabOrder = 3
    Text = '0'
    OnChange = Edit5Change
  end
  object TntPopupMenu1: TPopupMenu
    Left = 416
    Top = 264
    object Popup_Import: TMenuItem
      Caption = 'Import from file...'
      Hint = 'Import user models from a file.'
      OnClick = Popup_ImportClick
    end
    object Popup_Export: TMenuItem
      Caption = 'Export all to file...'
      Hint = 'Export user models to a file.'
      OnClick = Popup_ExportClick
    end
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'ini'
    Filter = 'User models [*.ini]|*.ini'
    Options = [ofOverwritePrompt, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 480
    Top = 264
  end
  object OpenDialog1: TOpenDialog
    Filter = 'User models [*.ini]|*.ini'
    Left = 448
    Top = 264
  end
end
