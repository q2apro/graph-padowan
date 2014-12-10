# Module with utility functions and classes for helping with GUI dialogs.
import vcl

"""Helper class for creating dialogs. When you derive from this class, your dialog will get an OK button and a Cancel button if ShowCancel is True,
   both at the bottom right of the dialog. A panel will fill the rest of the dialog where you can fill in your content.
"""
class SimpleDialog:
    def __init__(self, ShowCancel=True, **keywords):
        self.form = vcl.TForm(None, **keywords)
        self.Position = "poMainFormCenter"
        self.BorderStyle = "bsDialog"
        self.BorderIcons = "biSystemMenu"
        self.panel = vcl.TPanel(None, Parent=self.form, BevelInner="bvRaised", BevelOuter="bvLowered", Left=8, Top=8, Width=self.ClientWidth - 16, Height=self.ClientHeight - 50, Caption="", Anchors="akLeft,akTop,akRight,akBottom")
        self.button1 = vcl.TButton(None, Parent=self.form, Caption = "OK", Anchors = "akRight,akBottom", Default=True, OnClick=self.OnOk, Top=self.ClientHeight-32, Left=self.ClientWidth-176)
        if ShowCancel:
            self.button2 = vcl.TButton(None, Parent=self.form, Caption="Cancel", Anchors="akRight,akBottom", ModalResult=1, Cancel=True, Top=self.ClientHeight-32, Left=self.ClientWidth-88)
        else:
            self.button2 = None
            self.button1.Cancel = True
            self.button1.Anchors = "akLeft,akRight,akBottom"
            self.button1.Left = (self.ClientWidth - self.button1.Width) / 2
        self.OnShow = self.FormOnShow

    def FormOnShow(self, sender):
        self.button1.TabOrder = 100
        if self.button2:
            self.button2.TabOrder = 101
        pass

    # Method to be overwitten in sub class. It is called when the OK button is pressed.
    def OnOk(self, sender):
        self.Close()

    def __setattr__(self, name, value):
        try:
            setattr(self.form, name, value)
        except: #PropertyError:
            object.__setattr__(self, name, value)

    def __getattr__(self, name):
        try:
            return getattr(self.form, name)
        except:
            return object.__getattr__(self, name)
