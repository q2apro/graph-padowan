# Example plugin for Graph for showing statistics information for a point series.
# The plugin creates an action and adds it to the main menu and the context menu of the function list.
# The plugin hooks into the function list. Every time a new element is selected, the action is updated to be enabled if the element is a point series.
# At the same time the visibility of the context menu item is updated, so it only is visible if enabled.
# When the action is executed a dialog form is shown with staticks data calculated from the points in the point series.
import Graph
import vcl
import Gui

def Execute(Sender):
    import math
    yList = sorted(p[1] for p in Graph.Selected.Points)
    N = len(yList)
    Sum = math.fsum(yList)
    Mean = Sum / N

    Form = Gui.SimpleDialog(Caption="Statistics", Height=220, ShowCancel=False)
    Lines = "Statistics for " + Graph.Selected.LegendText + ":"
    Lines += "\n  Mean:\t\t" + format(Mean, ".8g")
    Lines += "\n  Sum:\t\t" + str(Sum)
    Lines += "\n  Median:\t\t" + format(yList[N//2] if N % 2 else (yList[N//2-1] + yList[N//2]) / 2, ".8g")
    Lines += "\n  Standard deviation: " + format( math.sqrt(math.fsum([(y-Mean)**2 for y in yList]) / N), ".8g")
    Lines += "\n  1st quartile:\t" + str(yList[N//4] if (N//2) % 2 else (yList[N//4-1] + yList[N//4]) / 2)
    Lines += "\n  3rd quartile:\t" + str(yList[math.ceil(N/2)+N//4] if (N//2) % 2 else (yList[math.ceil(N/2)+N//4-1] + yList[math.ceil(N/2)+N//4]) / 2)
    Lines += "\n  Min:\t\t" + str(min(yList))
    Lines += "\n  Max:\t\t" + str(max(yList))
    Memo = vcl.TMemo(None, Parent = Form.panel, ReadOnly=True, Align="alClient", Color=-16777201, WantReturns=False)
    Memo.Font.Size = 10
    Memo.Lines.Text = Lines
    Form.ShowModal()

def OnSelect(Item):
    Action.Enabled = isinstance(Item, Graph.TPointSeries)
    ContextMenuItem.Visible = Action.Enabled

Action = Graph.CreateAction(Caption="Statistics", OnExecute=Execute, Hint="Statistics on a point series.")
Graph.AddActionToMainMenu(Action)
ContextMenuItem = Graph.AddActionToContextMenu(Action)
Graph.OnSelect.append(OnSelect)
