inherited PolFuncFrame: TPolFuncFrame
  Width = 184
  Height = 230
  Anchors = [akLeft, akTop, akRight]
  ParentBackground = False
  ExplicitWidth = 184
  ExplicitHeight = 230
  DesignSize = (
    184
    230)
  object Label2: TLabel
    Left = 8
    Top = 42
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'r(t)='
  end
  object Label3: TLabel
    Left = 8
    Top = 74
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'x(t)='
  end
  object Label4: TLabel
    Left = 8
    Top = 106
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'y(t)='
  end
  object Label1: TLabel
    Left = 8
    Top = 10
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 't='
  end
  object Label5: TLabel
    Left = 8
    Top = 140
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'dr/dt='
  end
  object Label6: TLabel
    Left = 8
    Top = 172
    Width = 33
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'dy/dx='
  end
  object Label7: TLabel
    Left = 8
    Top = 204
    Width = 41
    Height = 13
    Caption = 'Snap to:'
    FocusControl = ComboBox1
  end
  object Edit2: TEdit
    Left = 48
    Top = 40
    Width = 127
    Height = 21
    Hint = 'The distance to the function evaluated for the given angle.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 1
  end
  object Edit3: TEdit
    Left = 48
    Top = 72
    Width = 127
    Height = 21
    Hint = 'The x-coordinate evaluated for the given angle.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 2
  end
  object Edit4: TEdit
    Left = 48
    Top = 104
    Width = 127
    Height = 21
    Hint = 'The y-coordinate evaluated for the given angle.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 3
  end
  object Edit5: TEdit
    Left = 48
    Top = 136
    Width = 127
    Height = 21
    Hint = 'The derivative of the function evaluated for the given angle.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 4
  end
  object Edit6: TEdit
    Left = 48
    Top = 168
    Width = 127
    Height = 21
    Hint = 'The slope of the function evaluated for the given angle.'
    Anchors = [akLeft, akTop, akRight]
    Color = clBtnFace
    ReadOnly = True
    TabOrder = 5
  end
  object Edit1: TMyEdit
    Left = 48
    Top = 8
    Width = 112
    Height = 21
    Hint = 
      'Enter the value of the polar angle for which you want to evaluat' +
      'e the function, or click on the graph in the graphing area.'
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object ComboBox1: TComboBox
    Left = 64
    Top = 200
    Width = 111
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    ItemIndex = 0
    TabOrder = 6
    Text = 'Function'
    OnChange = ComboBox1Change
    Items.Strings = (
      'Function'
      'Intersection'
      'x-axis'
      'y-axis'
      'Extreme x-value'
      'Extreme y-value')
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
