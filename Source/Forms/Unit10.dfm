object Form10: TForm10
  Left = 371
  Top = 262
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Tip of the day'
  ClientHeight = 210
  ClientWidth = 422
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  OnClose = FormClose
  DesignSize = (
    422
    210)
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 338
    Top = 16
    Width = 75
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Cancel = True
    Caption = 'Close'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
  object Button2: TButton
    Left = 338
    Top = 56
    Width = 75
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Next tip'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 338
    Top = 96
    Width = 75
    Height = 25
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Previous tip'
    TabOrder = 2
    OnClick = Button3Click
  end
  object CheckBox1: TCheckBox
    Left = 8
    Top = 186
    Width = 289
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'Show tips at startup'
    TabOrder = 3
  end
  object Panel1: TPanel
    Left = 8
    Top = 8
    Width = 315
    Height = 171
    Anchors = [akLeft, akTop, akBottom]
    BevelInner = bvLowered
    BevelOuter = bvNone
    Color = clWhite
    ParentBackground = False
    TabOrder = 4
    DesignSize = (
      315
      171)
    object Label1: TLabel
      Left = 64
      Top = 24
      Width = 104
      Height = 16
      Caption = 'Did you know...'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Image1: TImage
      Left = 16
      Top = 12
      Width = 28
      Height = 37
      AutoSize = True
      Picture.Data = {
        07544269746D6170C6020000424DC60200000000000076000000280000001C00
        000025000000010004000000000050020000130B0000130B0000100000001000
        000000000000FFFF000000FFFF0084828400C6C3C600FFFFFF00000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000555555555555535555555555555500005555555555553335555555555555
        0000555555555553333355555555555500005555555555333333355555555555
        0000555555555333303333555555555500005555555533330503333555555555
        0000555555533330555033335555555500005555553333051515033335555555
        0000555553333055555550333355555500005555333305151515150333355555
        0000555333305555555555503333555500005533330515150000551503333555
        0000555330555550044005555033555500005555051515103333051515055555
        0000555055555550444405555550555500005505151515103333051515150555
        0000555355555550222205555553555500005555351515022222201515355555
        0000555553555022200222055355555500005555553502222002222035555555
        0000555555502222222222220555555500005555055022222002222205505555
        0000555555022222200222222055555500002525250222222002222220525252
        0000555305022222200222222050355500002525250222222002222220525252
        0000555555022222200222222055555500005555055022222002222205505555
        0000555555550222222222205555555500005555555550022222200555555555
        0000555552055550000005555025555500005555532555555555555552355555
        0000555255555055202550555555255500005555255555555355555555525555
        0000555555555555232555555555555500005555555555555555555555555555
        000055555555555525255555555555550000}
    end
    object LinkLabel1: TLinkLabelEx
      Left = 16
      Top = 56
      Width = 289
      Height = 116
      AutoSize = False
      WordWrap = True
      OnLinkClick = LinkLabel1LinkClick
      Anchors = [akLeft, akTop, akRight, akBottom]
      Caption = 'Tip of the day'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Microsoft Sans Serif'
      Font.Style = []
      ParentFont = False
    end
  end
end
