import Graph

def CreateExample(FileName, Rows, Cols, Separator):
    File = open(FileName, "w")
    for i in range(Rows):
        File.write(str(i))
        for j in range(Cols):
            File.write(Separator + "%0.3f" % (i/1000+j))
        File.write("\n")
    File.close()

def TestSeparator(WriteSeparator, ReadSeparator):
    CreateExample(Graph.GraphDir + "/Test/Files/Temp.csv", 10, 1, WriteSeparator)
    Graph.LoadDefault()
    Graph.ImportPointSeries(Graph.GraphDir + "/Test/Files/Temp.csv", ReadSeparator)
    Graph.Update()
    assert list(Graph.FunctionList[1].Points) == [(0,0), (1.0, 0.001), (2.0, 0.002), (3.0, 0.003), (4.0, 0.004), (5.0, 0.005), (6.0, 0.006), (7.0, 0.007), (8.0, 0.008), (9.0, 0.009)]

def Run(Level):
    print("Running import test...")
    Graph.LoadDefault()
    Graph.ImportPointSeries(Graph.GraphDir + "/Test/Files/Test.csv")
    Graph.Update()
    assert Graph.FunctionList[1].LegendText == "a"
    assert Graph.FunctionList[2].LegendText == "b"
    assert Graph.FunctionList[3].LegendText == "c"
    assert Graph.FunctionList[4].LegendText == "d"
    assert list(Graph.FunctionList[1].Points) == [(0.0, 0.0), (1.0, 1.0), (2.0, 2.0), (3.0, 3.0)]
    assert list(Graph.FunctionList[2].Points) == [(0.0, 0.0), (1.0, -1.0), (2.0, -2.0), (3.0, -3.0)]
    assert list(Graph.FunctionList[3].Points) == [(4.0, 4.0), (5.0, 5.0), (6.0, 6.0), (7.0, 7.0)]
    assert list(Graph.FunctionList[4].Points) == [(4.0, -4.0), (5.0, -5.0), (6.0, -6.0), (7.0, -7.0)]

    TestSeparator(',', ',')
    TestSeparator(';', ';')
    TestSeparator(' ', ' ')
    TestSeparator('\t', '\t')
    TestSeparator(',', 0)
    TestSeparator(';', 0)
    TestSeparator(' ', 0)
    TestSeparator('\t', 0)

    # Test huge file
    if Level >= 2:
        CreateExample(Graph.GraphDir + "/Test/Files/Temp.csv", 300000, 20, ',')
        Graph.LoadDefault()
        Graph.ImportPointSeries(Graph.GraphDir + "/Test/Files/Temp.csv", ',')
        Graph.Update()
        assert Graph.FunctionList[1].Points[:4] == [(0,0), (1.0, 0.001), (2.0, 0.002), (3.0, 0.003)]
        Graph.SaveToFile(Graph.GraphDir + "/Test/Files/Temp.grf")
        Graph.LoadDefault()
        Graph.LoadFromFile(Graph.GraphDir + "/Test/Files/Temp.grf")
        Graph.Update()
