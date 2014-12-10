inherited TanFrame: TTanFrame
  Left = 0
  Top = 0
  Width = 184
  Height = 69
  HelpContext = 100
  Anchors = [akLeft, akTop, akRight]
  ParentBackground = False
  TabOrder = 0
  DesignSize = (
    184
    69)
  object Label1: TLabel
    Left = 8
    Top = 10
    Width = 25
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'x='
  end
  object Label2: TLabel
    Left = 8
    Top = 42
    Width = 25
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'y='
  end
  object Edit2: TEdit
    Left = 40
    Top = 40
    Width = 135
    Height = 21
    Hint =
      'This is the y-coordinate corresponding to the entered x-coordina' +
      'te.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 1
  end
  object Edit1: TMyEdit
    Left = 40
    Top = 8
    Width = 120
    Height = 21
    Hint = 'Enter an x-coordinate or click in the graphing area.'
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object UpDownEx1: TUpDownEx
    Left = 160
    Top = 8
    Width = 15
    Height = 21
    Associate = Edit1
    Anchors = [akTop, akRight]
  end
end
