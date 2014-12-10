# Interface for built-in functionlity in Graph
import os.path
import os
import sys
import imp
import traceback
import GraphImpl
import Settings
import Data
import Utility
import vcl
import xmlrpc.client
import getopt
import collections
import importlib

# enum values
Radian = Settings.Radian
Degree = Settings.Degree
asNone = Settings.asNone
asCrossed = Settings.asCrossed
asBoxed = Settings.asBoxed
lpCustom = Settings.lpCustom
lpTopRight = Settings.lpTopRight
lpBottomRight = Settings.lpBottomRight
lpTopLeft = Settings.lpTopLeft
lpBottomLeft = Settings.lpBottomLeft
npCenter = Settings.npCenter
npBefore = Settings.npBefore
gsLines = Settings.gsLines
gsDots = Settings.gsDots
dtAuto = Data.dtAuto
dtDots = Data.dtDots
dtLines = Data.dtLines
ttTangent = Data.ttTangent
ttNormal = Data.ttNormal
iaLinear = Data.iaLinear
iaCubicSpline = Data.iaCubicSpline
iaHalfCosine = Data.iaHalfCosine
iaCubicSpline2 = Data.iaCubicSpline2
lpAbove = Data.lpAbove
lpBelow = Data.lpBelow
lpLeft = Data.lpLeft
lpRight = Data.lpRight
lpAboveLeft = Data.lpAboveLeft
lpAboveRight = Data.lpAboveRight
lpBelowLeft = Data.lpBelowLeft
lpBelowRight = Data.lpBelowRight
ptCartesian = Data.ptCartesian
ptPolar = Data.ptPolar
lpUserTopLeft = Data.lpUserTopLeft
lpAboveX = Data.lpAboveX
lpBelowX = Data.lpBelowX
lpLeftOfY = Data.lpLeftOfY
lpRightOfY = Data.lpRightOfY
lpUserTopRight = Data.lpUserTopRight
lpUserBottomLeft = Data.lpUserBottomLeft
lpUserBottomRight = Data.lpUserBottomRight
ssAbove = Data.ssAbove
ssBelow = Data.ssBelow
ssXAxis = Data.ssXAxis
ssYAxis = Data.ssYAxis
ssBetween = Data.ssBetween
ssInside = Data.ssInside
rtEquation = Data.rtEquation
rtInequality = Data.rtInequality

Axes = Settings.GetAxes()
Property = Settings.Property
Redraw = Data.Redraw
GuiFormatSettings = Settings.GuiFormatSettings;
PlotSettings = Settings.PlotSettings
GuiSettings = Settings.GuiSettings

Selected = None
TGraphElem = Data.TGraphElem
TBaseFuncType = Data.TBaseFuncType
TStdFunc = Data.TStdFunc
TParFunc = Data.TParFunc
TPolFunc = Data.TPolFunc
TTan = Data.TTan
TTextLabel = Data.TTextLabel
TShading = Data.TShading
TRelation = Data.TRelation
TPointSeries = Data.TPointSeries
Form22 = GraphImpl.Form22
EFuncError = GraphImpl.EFuncError
EGraphError = GraphImpl.EGraphError

# Current version of Graph
VersionInfoType = collections.namedtuple("VersionInfoType", ["Major","Minor","Release","Build","ReleaseLevel"])
VersionInfoType.__eq__ = lambda self, other: self[:len(other)] == other if isinstance(other, tuple) else false
VersionInfo = VersionInfoType._make(GraphImpl.version_info)
Debug = GraphImpl.Debug

def LoadPlugins(Path):
    sys.path.append(Path)
    ModuleNames = []
    Modules = []
    AllowedExt = [ i[0] for i in imp.get_suffixes() ]
    for ModuleName, Ext in [ os.path.splitext(f) for f in os.listdir(Path) ]:
        if Ext in AllowedExt and not ModuleName in ModuleNames:
            try:
                ModuleNames.append(ModuleName)
                Modules.append(importlib.import_module(ModuleName))
            except Exception:
                traceback.print_exc()
    return Modules

# Function called when Graph is started. It will load the plugins in the Plugins directory. BaseDir is the directory where Graph is installed.
def InitPlugins(BaseDir):
    global PluginsDir
    global Form1
    global GraphDir
    GraphDir = BaseDir
    PluginsDir = BaseDir + '\\Plugins'
    Form1 = GraphImpl.Form1 # Form1 does not exist when Graph is started with /regserver
    LoadPlugins(PluginsDir)

# Put the ClearConsole function into the module used in the console.
sys.modules["__main__"].clear = GraphImpl.ClearConsole

"""Create a new action.
   Caption is the caption used when the action is shown in a menu.
   OnExecute is the function called when the action is executed, e.g. the menu item is selected.
   Hint is an optional hint for the action which is shown in the menu.
   ShortCut is an optional shortcut as a string, e.g. "Ctrl+A".
   IconFile is an optional file name of an image file that will be used as icon.
   OnUpdate is an optinal function called when idle and may be used to update the action.
   AddToToolBar, which is default True, specifies if the action can be added to the tool bar. The user will have to add it himself. This only makes it available.
"""
def CreateAction(Caption, OnExecute, Hint="", ShortCut="", IconFile=None, OnUpdate=None, AddToToolBar=True):
    Action = vcl.TAction(vcl.Application, Name=OnExecute.__module__+"_"+OnExecute.__name__, Caption=Caption, OnExecute=OnExecute, Hint=Hint, ShortCut=vcl.TextToShortCut(ShortCut), Category="Plugins", OnUpdate=OnUpdate)
    if AddToToolBar:
        Action.ActionList = Form1.ActionManager;
    if IconFile:
        Action.ImageIndex = LoadImage(IconFile)
    return Action


def AddActionToMainMenu(Action):
    MenuItems = Form1.ActionMainMenuBar1.ActionClient.Items;
    PluginsItem = MenuItems.Items[5];
    Item = PluginsItem.Items.Items[0];
    if Item.Action != None: #Workaround for bug in CB2009: There must always be at least one item
        Item = PluginsItem.Items.Add();
    Item.Action = Action;
    PluginsItem.Visible = True;
    return Item

def AddActionToContextMenu(Action):
    Item = vcl.TMenuItem(Form1)
    Item._owned = False
    Item.Action = Action
    Form1.PopupMenu1.Items.Insert(Form1.N10.MenuIndex, Item)
    return Item

def LoadImage(FileName, BkColor=0xFFFFFF):
    Bitmap = vcl.TBitmap()
    Bitmap.LoadFromFile(os.path.join(PluginsDir, FileName))
    return Form1.ImageList2.AddMasked(Bitmap, BkColor)

import collections
class ConstantsType(collections.MutableMapping):
    def keys(self):
        return GraphImpl.GetConstantNames()
    def __getitem__(self, name):
        return GraphImpl.GetConstant(name)
    def __setitem__(self, name, value):
        if value.__class__ == tuple:
            GraphImpl.SetConstant(name, value[1:], str(value[0]))
        else:
            GraphImpl.SetConstant(name, None, str(value))
    def __delitem__(self, name):
        GraphImpl.DelConstant(name)
    def __iter__(self):
        for k in self.keys(): yield k
    def __len__(self): return len(keys)
    def __repr__(self): return repr(dict(self))

class CustomFunctionsType(collections.MutableMapping):
    def keys(self):
        return GraphImpl.GetCustomFunctionNames()
    def __getitem__(self, name):
        return GraphImpl.GetCustomFunction(name)
    def __setitem__(self, name, value):
        GraphImpl.SetCustomFunction(name, value)
    def __delitem__(self, name):
        GraphImpl.DelCustomFunction(name)
    def __iter__(self):
        for k in self.keys(): yield k
    def __len__(self): return len(keys)
    def __repr__(self): return repr(dict(self))

def ExecuteEvent(event, args):
    for action in EventList[event]:
        try:
            if action(*args):
                return True
        except:
            traceback.print_exc()

def LoadDfmFile(FileName):
    Stream = vcl.TFileStream(FileName, 0x20)
    Stream2 = vcl.TMemoryStream()
    vcl.ObjectTextToBinary(Stream, Stream2)
    Stream2.Position = 0
    Form = vcl.TForm(None)
    Stream2.ReadComponent(Form)
    return Form

class ChildListType(collections.MutableSequence):
    def __init__(self, node):
        self.node = node
    def __getitem__(self, key):
        if key < Data.ChildCount(self.node):
            return Data.GetChild(self.node, key)
        raise IndexError
    def __len__(self):
        return Data.ChildCount(self.node)
    def insert(self, key, value):
        Data.InsertChild(self.node, value, key)
    def __setitem__(self, key, value):
        Data.ReplaceChild(self.node, key, value)
    def append(self, value):
        Data.InsertChild(self.node, value, -1)
    def __delitem__(self, key):
        Data.RemoveChild(self.node, key)
    def __repr__(self):
        return repr(list(self))
    Selected = property(lambda self:Data.GetSelected(), lambda self,x:Data.SetSelected(x))

class PluginDataType(collections.MutableMapping):
    def __init__(self, dict): self.data = dict
    def __len__(self): return self.size()
    def __getitem__(self, key): return xmlrpc.client.loads(self.data[key])[0]
    def __setitem__(self, key, item): self.data[key] = xmlrpc.client.dumps(item).replace("\n", "")
    def __delitem__(self, key): self.data.erase(key)
    def __iter__(self): return iter(self.data)
    def __contains__(self, key): return key in self.data
    def __repr__(self): return repr(dict(self))

TGraphElem.PluginData = property(lambda self: PluginDataType(self._PluginData))
TGraphElem.ChildList = property(lambda self: ChildListType(self))
PluginData = PluginDataType(Data.GetPluginData())

Constants = ConstantsType()
CustomFunctions = CustomFunctionsType()
FunctionList = ChildListType(Data.GetTopElem())

Eval = GraphImpl.Eval
EvalComplex = GraphImpl.EvalComplex
SaveAsImage = GraphImpl.SaveAsImage
Update = GraphImpl.Update
InputQuery = GraphImpl.InputQuery

# TProperty
cfReal = Settings.cfReal
cfRectangular = Settings.cfRectangular
cfPolar = Settings.cfPolar

def HandleOnSelect(Elem):
    global Selected
    Selected = Elem

OnNew = []
OnLoad = []
OnSelect = [HandleOnSelect]
OnClose = []
OnEdit = []
OnAnimate = []
OnDelete = []
OnAxesChanged = []
OnZoom = []
OnOptionsChanged = []
OnCustomFunctions = []
OnNewElem = []
OnChanged = []
OnMoved = []
EventList = [OnNew, OnLoad, OnSelect, OnClose, OnEdit, OnAnimate, OnDelete, OnAxesChanged, OnZoom, OnOptionsChanged, OnCustomFunctions, OnNewElem, OnChanged, OnMoved]

# Imports from Utility
BeginMultiUndo = Utility.BeginMultiUndo
EndMultiUndo = Utility.EndMultiUndo
LoadDefault = Utility.LoadDefault
LoadFromFile = Utility.LoadFromFile
SaveToFile = Utility.SaveToFile
Import = Utility.Import
ImportPointSeries = Utility.ImportPointSeries
