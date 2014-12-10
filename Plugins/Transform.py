import Graph
import vcl
import Gui
class TransformDialog(Gui.SimpleDialog):
    def __init__(self):
        Gui.SimpleDialog.__init__(self)
        self.Caption = "Transform point series"
        self.Height = 146
        self.Width = 346

        self.label1 = vcl.TLabel(None, Parent = self.panel, Caption = "x[n] = ", Top = 12, Left = 8)
        self.label2 = vcl.TLabel(None, Parent = self.panel, Caption = "y[n] = ", Top = 44, Left = 8)
        self.edit1 = vcl.TEdit(None, Parent = self.panel, Top = 8, Left = 44, Width = 270)
        self.edit2 = vcl.TEdit(None, Parent = self.panel, Top = 40, Left = 44, Width = 270)

    def OnOk(self, sender):
        P = Graph.TPointSeries()
        Graph.CustomFunctions["x"] = lambda n: Graph.Selected.Points[int(n)][0]
        Graph.CustomFunctions["y"] = lambda n: Graph.Selected.Points[int(n)][1]
        for n in range(len(Graph.Selected.Points)):
            Graph.CustomFunctions["n"] = lambda: n
            x = Graph.Eval(self.edit1.Text) if self.edit1.Text != "" else Graph.Selected.Points[n][0]
            y = Graph.Eval(self.edit2.Text) if self.edit2.Text != "" else Graph.Selected.Points[n][1]
            P.Points.append((x, y))
        Graph.FunctionList.append(P)
        del Graph.CustomFunctions["x"]
        del Graph.CustomFunctions["y"]
        del Graph.CustomFunctions["n"]
        Graph.Redraw()
        self.Close()

def Execute(action):
    d = TransformDialog()
    d.ShowModal()

def OnSelect(Item):
    Action.Enabled = isinstance(Item, Graph.TPointSeries)
    ContextMenuItem.Visible = Action.Enabled

Action = Graph.CreateAction(Caption="Transform", OnExecute=Execute, Hint="Transform point series.")
Graph.AddActionToMainMenu(Action)
ContextMenuItem = Graph.AddActionToContextMenu(Action)
Graph.OnSelect.append(OnSelect)
