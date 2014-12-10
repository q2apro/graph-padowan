import Graph

ShowEvents = False
LogFile = "Events.log"

File = open(LogFile, "w") if LogFile else None
class EventLogger:
    def __init__(self, s):
        self.s = s
    def __call__(self, *args):
        Str = self.s + (str(args) if len(args) != 1 else "(%s)" % args[0])
        if ShowEvents:
            print(Str)
        if File:
            File.write(Str + "\n")

Graph.OnNew.append(EventLogger("OnNew"))
Graph.OnLoad.append(EventLogger("OnLoad"))
Graph.OnSelect.append(EventLogger("OnSelect"))
Graph.OnClose.append(EventLogger("OnClose"))
Graph.OnEdit.append(EventLogger("OnEdit"))
Graph.OnAnimate.append(EventLogger("OnAnimate"))
Graph.OnDelete.append(EventLogger("OnDelete"))
Graph.OnAxesChanged.append(EventLogger("OnAxesChanged"))
Graph.OnZoom.append(EventLogger("OnZoom"))
Graph.OnOptionsChanged.append(EventLogger("OnOptionsChanged"))
Graph.OnCustomFunctions.append(EventLogger("OnCustomFunctions"))
Graph.OnNewElem.append(EventLogger("OnNewElem"))
Graph.OnChanged.append(EventLogger("OnChanged"))
Graph.OnMoved.append(EventLogger("OnMoved"))
File.write("PluginTester loaded!\n")

