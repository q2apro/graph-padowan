import Graph
import Test

def Run(Level):
    print("Running point series test...")
    Graph.LoadDefault()
    PointSeries = Graph.TPointSeries()
    PointSeries.FillColor = 0x0000FF
    PointSeries.FrameColor = 0x000000
    PointSeries.LineColor = 0xFF0000
    PointSeries.Size = 5
    PointSeries.LineSize = 2
    PointSeries.LineStyle = 1
    P = [(32.0, 1.93), (40.0, 1.67), (50.0, 1.4), (60.0, 1.22), (70.0, 1.06), (80.0, 0.93), (90.0, 0.825), (100.0, 0.74), (150.0, 0.477), (200.0, 0.341), (250.0, 0.269), (300.0, 0.22), (350.0, 0.189), (400.0, 0.17), (450.0, 0.155), (500.0, 0.145), (550.0, 0.139), (600.0, 0.137)]
    PointSeries.Points = P
    Graph.FunctionList.append(PointSeries)
    Graph.Update()
    assert Graph.FunctionList[1].Points == P
    assert Graph.FunctionList[1].Points[4:10] == P[4:10]
    assert Graph.FunctionList[1].Points[4] == P[4]
    assert Graph.FunctionList[1].Points[:] == P
    assert Graph.FunctionList[1].Points[-10:-5] == P[-10:-5]
    assert Graph.FunctionList[1].Points[10:] == P[10:]
    assert Graph.FunctionList[1].Points[:10] == P[:10]
    assert Graph.FunctionList[1].Points[5:15:3] == P[5:15:3]
    assert Graph.FunctionList[1].Points[::2] == P[::2]
    assert Graph.FunctionList[1].Points[-15:-5:2] == P[-15:-5:2]
    assert Graph.FunctionList[1].Points[10:200] == P[10:200]
    assert Graph.FunctionList[1].Points[-100:10] == P[-100:10]
    assert len(Graph.FunctionList[1].Points) == len(P)

    Graph.FunctionList[1].Points[5:10] = [(1,10), (2,20), (3, 30), (4,40), (5,50)]
    P[5:10] = [(1,10), (2,20), (3, 30), (4,40), (5,50)]
    assert Graph.FunctionList[1].Points == P

    Graph.FunctionList[1].Points[7] = (15,80)
    P[7] = (15, 80)
    assert Graph.FunctionList[1].Points == P

    Graph.FunctionList[1].Points[5:10] = [(20,15), (21,16), (22, 17)]
    P[5:10] = [(20,15), (21,16), (22, 17)]
    assert Graph.FunctionList[1].Points == P

    Graph.FunctionList[1].Points[5:8] = [(30,45), (31,46), (32, 47), (33, 48), (34,49), (35,50)]
    P[5:8] = [(30,45), (31,46), (32, 47), (33, 48), (34,49), (35,50)]
    assert Graph.FunctionList[1].Points == P

    del Graph.FunctionList[1].Points[5]
    del P[5]
    assert Graph.FunctionList[1].Points == P

    del Graph.FunctionList[1].Points[5:10]
    del P[5:10]
    assert Graph.FunctionList[1].Points == P

    with Test.assertRaises(IndexError):
      Graph.FunctionList[1].Points[200]
    with Test.assertRaises(IndexError):
      del Graph.FunctionList[1].Points[200]
    with Test.assertRaises(SystemError):
      Graph.FunctionList[1].Points[10] = 5
    with Test.assertRaises(ValueError):
      Graph.FunctionList[1].Points[2:10:2] = [(1,2), (3,4), (5,6)]

    Graph.FunctionList[1].Points[2:10:2] = [(1,2), (3,4), (5,6), (7,8)]
    P[2:10:2] = [(1,2), (3,4), (5,6), (7,8)]
    assert Graph.FunctionList[1].Points == P

    del Graph.FunctionList[1].Points[2:10:2]
    del P[2:10:2]
    assert Graph.FunctionList[1].Points == P
