import Graph
import vcl

def Update(action):
  Action.Caption = "Show Python interpreter" if not Graph.Form22.Visible else "Hide Python interpreter"
  Action.Hint = "Show the Python interpreter window." if not Graph.Form22.Visible else "Hide the Python interpreter window."

def Execute(action):
  Graph.Form22.Visible = not Graph.Form22.Visible

Action = Graph.CreateAction(Caption="Show Python interpreter", OnExecute=Execute, OnUpdate=Update, ShortCut="F11")
Graph.AddActionToMainMenu(Action)
