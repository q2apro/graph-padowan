inherited AreaFrame: TAreaFrame
  Width = 178
  Height = 101
  Anchors = [akLeft, akTop, akRight]
  ParentBackground = False
  ExplicitWidth = 178
  ExplicitHeight = 101
  object GridPanel1: TGridPanelEx
    Left = 0
    Top = 0
    Width = 178
    Height = 90
    BevelOuter = bvNone
    ColumnCollection = <
      item
        SizeStyle = ssAuto
        Value = 50.000000000000000000
      end
      item
        Value = 100.000000000000000000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = Label1
        Row = 0
      end
      item
        Column = 0
        Control = Label2
        Row = 1
      end
      item
        Column = 0
        Control = Label3
        Row = 2
      end
      item
        Column = 1
        Control = Edit3
        Row = 2
      end
      item
        Column = 1
        Control = Panel1
        Row = 0
      end
      item
        Column = 1
        Control = Panel2
        Row = 1
      end>
    Padding.Left = 2
    Padding.Top = 4
    Padding.Right = 2
    Padding.Bottom = 5
    RowCollection = <
      item
        Value = 33.333333333333330000
      end
      item
        Value = 33.333333333333330000
      end
      item
        Value = 33.333333333333330000
      end>
    TabOrder = 0
    object Label1: TLabel
      Left = 4
      Top = 8
      Width = 28
      Height = 17
      Align = alLeft
      Caption = 'From:'
      Layout = tlCenter
      ExplicitHeight = 13
    end
    object Label2: TLabel
      Left = 4
      Top = 34
      Width = 16
      Height = 17
      Align = alLeft
      Caption = 'To:'
      Layout = tlCenter
      ExplicitHeight = 13
    end
    object Label3: TLabel
      Left = 4
      Top = 60
      Width = 23
      Height = 20
      Align = alLeft
      Caption = 'Area'
      Layout = tlCenter
      ExplicitHeight = 13
    end
    object Edit3: TEdit
      Left = 36
      Top = 60
      Width = 138
      Height = 20
      Align = alClient
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 2
      ExplicitLeft = 46
      ExplicitWidth = 128
      ExplicitHeight = 21
    end
    object Panel1: TPanel
      Left = 36
      Top = 8
      Width = 138
      Height = 17
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      ExplicitLeft = 46
      ExplicitWidth = 128
      DesignSize = (
        138
        17)
      object Edit1: TMyEdit
        Left = 0
        Top = 0
        Width = 124
        Height = 21
        Hint = 
          'Enter the start coordinate or mark the range on the graph itself' +
          ' with the mouse.'
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        ExplicitWidth = 114
      end
      object UpDown1: TUpDownEx
        Left = 124
        Top = 0
        Width = 15
        Height = 21
        Associate = Edit1
        Anchors = [akRight]
        ExplicitLeft = 114
      end
    end
    object Panel2: TPanel
      Left = 36
      Top = 34
      Width = 138
      Height = 17
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      ExplicitLeft = 46
      ExplicitWidth = 128
      DesignSize = (
        138
        17)
      object Edit2: TMyEdit
        Left = 0
        Top = 0
        Width = 124
        Height = 21
        Hint = 
          'Enter the end coordinate or mark the range on the graph itself w' +
          'ith the mouse.'
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        ExplicitWidth = 114
      end
      object UpDown2: TUpDownEx
        Left = 124
        Top = 0
        Width = 15
        Height = 21
        Associate = Edit2
        Anchors = [akTop, akRight]
        ExplicitLeft = 114
      end
    end
  end
end
