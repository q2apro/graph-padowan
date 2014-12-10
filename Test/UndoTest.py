import Graph

def Run(Level):
    print("Running undo test...")
    Graph.LoadDefault()
    Undo = Graph.Form1.UndoAction.Execute
    Redo = Graph.Form1.RedoAction.Execute
    F1 = Graph.TStdFunc("sin(x)")
    F2 = Graph.TRelation("x^2+y^2=25")
    F3 = Graph.TPointSeries()
    Graph.FunctionList.append(F1)
    Graph.FunctionList.append(F2)
    assert Graph.FunctionList[1] == F1
    assert Graph.FunctionList[2] == F2
    assert len(Graph.FunctionList) == 3
    Undo()
    assert len(Graph.FunctionList) == 2
    Redo()
    assert len(Graph.FunctionList) == 3
    assert Graph.FunctionList[2] == F2
    Graph.FunctionList[1] = F3
    assert Graph.FunctionList[1] == F3
    assert Graph.FunctionList[2] == F2
    Undo()
    assert Graph.FunctionList[1] == F1
    assert Graph.FunctionList[2] == F2
    Redo()
    assert Graph.FunctionList[1] == F3
    assert Graph.FunctionList[2] == F2
    del Graph.FunctionList[1]
    assert Graph.FunctionList[1] == F2
    assert len(Graph.FunctionList) == 2
    Undo()
    assert Graph.FunctionList[1] == F3
    assert Graph.FunctionList[2] == F2
    Redo()
    assert Graph.FunctionList[1] == F2
    assert len(Graph.FunctionList) == 2

