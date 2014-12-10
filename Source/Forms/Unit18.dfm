object Form18: TForm18
  Left = 346
  Top = 182
  BiDiMode = bdLeftToRight
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Image options'
  ClientHeight = 250
  ClientWidth = 323
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
    323
    250)
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 8
    Top = 8
    Width = 306
    Height = 193
    ActivePage = TabSheet1
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Size'
      object Label1: TLabel
        Left = 8
        Top = 92
        Width = 60
        Height = 13
        Caption = 'Image width:'
        FocusControl = Edit1
      end
      object Label2: TLabel
        Left = 8
        Top = 124
        Width = 64
        Height = 13
        Caption = 'Image height:'
        FocusControl = Edit2
      end
      object Edit1: TEdit
        Left = 80
        Top = 88
        Width = 121
        Height = 21
        Hint = 'Set a custom image width in pixels.'
        TabOrder = 1
        OnKeyPress = EditKeyPress
      end
      object Edit2: TEdit
        Left = 80
        Top = 120
        Width = 121
        Height = 21
        Hint = 'Set a custom image height in pixels.'
        TabOrder = 2
        OnKeyPress = EditKeyPress
      end
      object RadioGroup3: TRadioGroup
        Left = 8
        Top = 8
        Width = 193
        Height = 65
        Hint = 
          'Choose to use the same size as shown on the screen or a custom i' +
          'mage size.'
        Caption = 'Image size'
        ItemIndex = 0
        Items.Strings = (
          'Screen size'
          'Custom size')
        TabOrder = 0
        OnClick = RadioGroup3Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'JPEG'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ImageIndex = 1
      ParentFont = False
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        298
        165)
      object RadioGroup1: TRadioGroup
        Left = 8
        Top = 72
        Width = 145
        Height = 73
        Hint = 
          'Progressive encoding allows viewers to see a rough copy of the i' +
          'mage while it is downloaded.'
        Caption = 'Encoding'
        ItemIndex = 0
        Items.Strings = (
          'Sequential'
          'Progressive')
        TabOrder = 1
      end
      object GroupBox1: TGroupBox
        Left = 8
        Top = 8
        Width = 282
        Height = 57
        Hint = 'Select a compression quality.'
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Quality'
        TabOrder = 0
        DesignSize = (
          282
          57)
        object TrackBar1: TTrackBar
          Left = 43
          Top = 12
          Width = 231
          Height = 40
          Anchors = [akLeft, akTop, akRight]
          Max = 99
          Min = 1
          PageSize = 10
          Frequency = 5
          Position = 1
          TabOrder = 1
          TickMarks = tmTopLeft
          OnChange = TrackBar1Change
        end
        object Edit3: TEdit
          Left = 8
          Top = 20
          Width = 25
          Height = 21
          MaxLength = 2
          TabOrder = 0
          Text = '0'
          OnChange = Edit3Change
          OnKeyPress = EditKeyPress
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'PDF'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label3: TLabel
        Left = 8
        Top = 8
        Width = 52
        Height = 13
        Caption = 'Paper size:'
        FocusControl = ComboBox1
      end
      object RadioGroup2: TRadioGroup
        Left = 8
        Top = 40
        Width = 105
        Height = 65
        Hint = 'Choose paper orientation.'
        Caption = 'Orientation'
        ItemIndex = 0
        Items.Strings = (
          'Portrait'
          'Landscape')
        TabOrder = 1
      end
      object ComboBox1: TComboBox
        Left = 64
        Top = 4
        Width = 89
        Height = 21
        Hint = 'Choose paper size to use.'
        Style = csDropDownList
        TabOrder = 0
        Items.Strings = (
          'A2'
          'A3'
          'A4'
          'A5'
          'A6'
          'Letter'
          'Ledger'
          'Legal')
      end
    end
  end
  object Button2: TButton
    Left = 241
    Top = 216
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 220
    Width = 145
    Height = 17
    Hint = 'Select this to use these settings as defaults in the future.'
    Caption = 'Save as default'
    TabOrder = 1
  end
  object Button1: TButton
    Left = 153
    Top = 216
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
end
