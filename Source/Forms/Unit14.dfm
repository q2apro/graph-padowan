object Form14: TForm14
  Left = 381
  Top = 114
  HelpContext = 120
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Insert point series'
  ClientHeight = 556
  ClientWidth = 473
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
  OnShow = FormShow
  DesignSize = (
    473
    556)
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 16
    Top = 26
    Width = 56
    Height = 13
    Caption = 'Description:'
    FocusControl = Edit1
  end
  object Button1: TButton
    Left = 208
    Top = 521
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 4
    OnClick = Button1Click
  end
  object Grid: TGrid
    Left = 16
    Top = 56
    Width = 236
    Height = 455
    Anchors = [akLeft, akTop, akRight, akBottom]
    BiDiMode = bdLeftToRight
    ColCount = 2
    DefaultColWidth = 105
    DefaultRowHeight = 16
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected, goColSizing, goEditing, goTabs, goAlwaysShowEditor, goThumbTracking]
    ParentBiDiMode = False
    PopupMenu = PopupMenu1
    TabOrder = 1
    MinColWidth = 50
    AutoAddRows = True
    ExportFixedRows = False
    EditorPopupMenu = PopupMenu1
    OnEditorKeyPress = GridEditorKeyPress
    OnGetText = GridGetText
    OnSetText = GridSetText
  end
  object Button2: TButton
    Left = 296
    Top = 521
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 5
  end
  object Edit1: TMyEdit
    Left = 80
    Top = 24
    Width = 172
    Height = 21
    Hint = 
      'The name of the point series, which is shown in the list to the ' +
      'left of the coordinate system.'
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = 'Series'
  end
  object GroupBox4: TGroupBox
    Left = 264
    Top = 442
    Width = 194
    Height = 68
    Hint = 
      'This shows a sample of the markers and the lines between the mar' +
      'kers.'
    Anchors = [akRight, akBottom]
    Caption = 'Sample'
    TabOrder = 3
    object PaintBox1: TPaintBox
      Left = 2
      Top = 15
      Width = 190
      Height = 51
      Align = alClient
      OnPaint = PaintBox1Paint
    end
  end
  object Button3: TButton
    Left = 384
    Top = 521
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Help'
    TabOrder = 6
    OnClick = Button3Click
  end
  object PageControl1: TPageControl
    Left = 264
    Top = 24
    Width = 194
    Height = 407
    ActivePage = TabSheet1
    Anchors = [akTop, akRight, akBottom]
    TabOrder = 2
    object TabSheet1: TTabSheet
      Caption = 'Markers'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        186
        379)
      object GroupBox1: TGroupBox
        Left = 9
        Top = 57
        Width = 169
        Height = 103
        Anchors = [akLeft, akRight, akBottom]
        Caption = 'Marker'
        TabOrder = 1
        DesignSize = (
          169
          103)
        object Label1: TLabel
          Left = 11
          Top = 20
          Width = 26
          Height = 13
          Caption = 'Style:'
          FocusControl = PointSelect1
        end
        object Label2: TLabel
          Left = 11
          Top = 46
          Width = 27
          Height = 13
          Caption = 'Color:'
          FocusControl = ExtColorBox1
        end
        object Label4: TLabel
          Left = 11
          Top = 72
          Width = 23
          Height = 13
          Caption = 'Size:'
          FocusControl = Edit2
        end
        object Edit2: TEdit
          Left = 76
          Top = 70
          Width = 41
          Height = 21
          Hint = 'Choose the size of the markers.'
          Anchors = [akTop, akRight]
          TabOrder = 2
          Text = '5'
          OnChange = Change
          OnKeyPress = EditKeyPress
        end
        object UpDown1: TUpDown
          Left = 117
          Top = 70
          Width = 15
          Height = 21
          Anchors = [akTop, akRight]
          Associate = Edit2
          Max = 20
          Position = 5
          TabOrder = 3
        end
        object PointSelect1: TPointSelect
          Left = 76
          Top = 16
          Width = 49
          Height = 22
          Hint = 'Choose the type of markers to be shown at each point.'
          OnChange = Change
          TabOrder = 0
          Anchors = [akTop, akRight]
        end
        object ExtColorBox1: TExtColorBox
          Left = 76
          Top = 43
          Width = 86
          Height = 22
          Hint = 'Choose the color of the markers.'
          Anchors = [akTop, akRight]
          TabOrder = 1
          OnChange = Change
          AutoDroppedWidth = True
          Selected = clScrollBar
          DefaultName = 'Default'
          CustomName = 'Custom...'
        end
      end
      object GroupBox2: TGroupBox
        Left = 9
        Top = 165
        Width = 169
        Height = 131
        Anchors = [akLeft, akRight, akBottom]
        Caption = 'Line'
        TabOrder = 2
        DesignSize = (
          169
          131)
        object Label5: TLabel
          Left = 11
          Top = 20
          Width = 26
          Height = 13
          Caption = 'Style:'
          FocusControl = LineSelect1
        end
        object Label6: TLabel
          Left = 11
          Top = 46
          Width = 27
          Height = 13
          Caption = 'Color:'
          FocusControl = ExtColorBox2
        end
        object Label7: TLabel
          Left = 11
          Top = 72
          Width = 31
          Height = 13
          Caption = 'Width:'
          FocusControl = Edit3
        end
        object Label11: TLabel
          Left = 11
          Top = 100
          Width = 61
          Height = 13
          Caption = 'Interpolation:'
          FocusControl = ComboBox2
        end
        object LineSelect1: TLineSelect
          Left = 76
          Top = 16
          Width = 86
          Height = 22
          Hint = 
            'Choose the style of lines to be drawn between the points. You ma' +
            'y also choose not to draw any lines.'
          EnableClear = True
          LineStyle = psClear
          OnChange = Change
          TabOrder = 0
          Anchors = [akTop, akRight]
        end
        object Edit3: TEdit
          Left = 76
          Top = 70
          Width = 41
          Height = 21
          Hint = 'Choose the width of the lines in pixels.'
          Anchors = [akTop, akRight]
          TabOrder = 2
          Text = '1'
          OnChange = Change
          OnKeyPress = EditKeyPress
        end
        object UpDown2: TUpDown
          Left = 117
          Top = 70
          Width = 15
          Height = 21
          Anchors = [akTop, akRight]
          Associate = Edit3
          Max = 20
          Position = 1
          TabOrder = 3
        end
        object ComboBox2: TExtComboBox
          Left = 76
          Top = 96
          Width = 86
          Height = 21
          Hint = 'Select which kind of line to draw between the markers.'
          Style = csDropDownList
          Anchors = [akTop, akRight]
          TabOrder = 4
          Items.Strings = (
            'Linear'
            '1D cubic spline'
            '2D cubic spline'
            'Half cosine')
          AutoDroppedWidth = True
        end
        object ExtColorBox2: TExtColorBox
          Left = 76
          Top = 43
          Width = 86
          Height = 22
          Hint = 'Choose the color of the lines.'
          Anchors = [akTop, akRight]
          TabOrder = 1
          OnChange = Change
          AutoDroppedWidth = True
          Selected = clScrollBar
          DefaultName = 'Default'
          CustomName = 'Custom...'
        end
      end
      object GroupBox3: TGroupBox
        Left = 9
        Top = 301
        Width = 169
        Height = 72
        Anchors = [akLeft, akRight, akBottom]
        Caption = 'Labels'
        TabOrder = 3
        DesignSize = (
          169
          72)
        object Label8: TLabel
          Left = 11
          Top = 44
          Width = 40
          Height = 13
          Caption = 'Position:'
          FocusControl = ComboBox1
        end
        object CheckBox2: TCheckBox
          Left = 11
          Top = 16
          Width = 126
          Height = 17
          Hint = 'Check to show the coordinate at each point.'
          Caption = 'Show coordinates'
          TabOrder = 0
          OnClick = Change
        end
        object BitBtn1: TBitBtn
          Left = 140
          Top = 14
          Width = 21
          Height = 21
          Hint = 'Select the font for the coordinates.'
          Anchors = [akTop, akRight]
          DoubleBuffered = True
          Glyph.Data = {
            F6000000424DF600000000000000760000002800000010000000100000000100
            04000000000080000000130B0000130B00001000000010000000A64D4D00FF4D
            4D00D1D8DB000000000000000000000000000000000000000000000000000000
            0000000000000000000000000000000000000000000000000000222222222222
            2222222000000000000222222222222222222220000022200002222220022222
            0022222222002222002211111220000000222211222200220022222112222002
            0022222211111100002222222112211000222222221121120022222222211112
            2222222222221112222222222222211222222222222222222222}
          ParentDoubleBuffered = False
          TabOrder = 1
          OnClick = BitBtn1Click
        end
        object ComboBox1: TComboBox
          Left = 76
          Top = 40
          Width = 86
          Height = 21
          Hint = 'Select where the coordinates should be shown.'
          Style = csDropDownList
          Anchors = [akTop, akRight]
          Enabled = False
          TabOrder = 2
          OnChange = Change
          Items.Strings = (
            'Above'
            'Below'
            'Left'
            'Right'
            'Above Left'
            'Above Right'
            'Below Left'
            'Below Right')
        end
      end
      object RadioGroup1: TRadioGroup
        Left = 9
        Top = -2
        Width = 169
        Height = 53
        Hint = 
          'Choose between cartesian (x,y) coordinates and polar ('#952',r) coord' +
          'inates.'
        Caption = 'Coordinate type'
        ItemIndex = 0
        Items.Strings = (
          'Cartesian'
          'Polar')
        TabOrder = 0
        OnClick = RadioGroup1Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Error bars'
      ImageIndex = 1
      DesignSize = (
        186
        379)
      object GroupBox5: TGroupBox
        Left = 8
        Top = 10
        Width = 170
        Height = 112
        Anchors = [akLeft, akTop, akRight]
        Caption = 'X error amount'
        TabOrder = 0
        DesignSize = (
          170
          112)
        object Label9: TLabel
          Left = 157
          Top = 68
          Width = 8
          Height = 13
          Anchors = [akTop, akRight]
          Caption = '%'
        end
        object CheckBox3: TCheckBox
          Left = 8
          Top = 16
          Width = 137
          Height = 17
          Hint = 'Show horizontal error bars.'
          Caption = 'Show x-bars'
          TabOrder = 0
          OnClick = RadioButtonClick
        end
        object RadioButton1: TRadioButton
          Left = 8
          Top = 40
          Width = 73
          Height = 17
          Hint = 'All error bars are the same size.'
          Caption = 'Fixed:'
          Checked = True
          TabOrder = 1
          TabStop = True
          OnClick = RadioButtonClick
        end
        object Edit4: TMyEdit
          Left = 87
          Top = 36
          Width = 67
          Height = 21
          Hint = 'Enter fixed value.'
          Anchors = [akTop, akRight]
          Enabled = False
          TabOrder = 2
          OnChange = EditChange
        end
        object RadioButton2: TRadioButton
          Left = 8
          Top = 64
          Width = 73
          Height = 17
          Hint = 'All error bars are the same percentage of the x-coordinate.'
          Caption = 'Relative:'
          TabOrder = 3
          OnClick = RadioButtonClick
        end
        object RadioButton3: TRadioButton
          Left = 8
          Top = 88
          Width = 113
          Height = 17
          Hint = 'Each error bar is given a custom value.'
          Caption = 'Custom'
          TabOrder = 5
          OnClick = RadioButtonClick
        end
        object Edit5: TMyEdit
          Left = 87
          Top = 64
          Width = 67
          Height = 21
          Hint = 'Enter relative value in percent.'
          Anchors = [akTop, akRight]
          Enabled = False
          TabOrder = 4
          OnChange = EditChange
        end
      end
      object GroupBox6: TGroupBox
        Left = 8
        Top = 138
        Width = 170
        Height = 112
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Y error amount'
        TabOrder = 1
        DesignSize = (
          170
          112)
        object Label10: TLabel
          Left = 157
          Top = 68
          Width = 8
          Height = 13
          Anchors = [akTop, akRight]
          Caption = '%'
        end
        object CheckBox4: TCheckBox
          Left = 8
          Top = 16
          Width = 137
          Height = 17
          Hint = 'Show vertical error bars.'
          Caption = 'Show y-bars'
          TabOrder = 0
          OnClick = RadioButtonClick
        end
        object RadioButton4: TRadioButton
          Left = 8
          Top = 40
          Width = 73
          Height = 17
          Hint = 'All error bars are the same size.'
          Caption = 'Fixed:'
          Checked = True
          TabOrder = 1
          TabStop = True
          OnClick = RadioButtonClick
        end
        object Edit6: TMyEdit
          Left = 87
          Top = 36
          Width = 67
          Height = 21
          Hint = 'Enter fixed value.'
          Anchors = [akTop, akRight]
          Enabled = False
          TabOrder = 2
          OnChange = EditChange
        end
        object RadioButton5: TRadioButton
          Left = 8
          Top = 64
          Width = 73
          Height = 17
          Hint = 'All error bars are the same percentage of the y-coordinate.'
          Caption = 'Relative:'
          TabOrder = 3
          OnClick = RadioButtonClick
        end
        object RadioButton6: TRadioButton
          Left = 8
          Top = 88
          Width = 113
          Height = 17
          Hint = 'Each error bar is given a custom value.'
          Caption = 'Custom'
          TabOrder = 5
          OnClick = RadioButtonClick
        end
        object Edit7: TMyEdit
          Left = 87
          Top = 64
          Width = 67
          Height = 21
          Hint = 'Enter relative value in percent.'
          Anchors = [akTop, akRight]
          Enabled = False
          TabOrder = 4
          OnChange = EditChange
        end
      end
    end
  end
  object PopupMenu1: TPopupActionBar
    OnPopup = PopupMenu1Popup
    Left = 24
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
    FilterIndex = 5
    Left = 56
    Top = 328
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Left = 88
    Top = 328
  end
  object SaveDialog1: TSaveDialogEx
    DefaultExt = 'csv'
    Options = [ofOverwritePrompt, ofPathMustExist, ofNoReadOnlyReturn, ofEnableSizing]
    Left = 120
    Top = 328
  end
end
