import wx
import os
import sys

GUI_image_X = 240
GUI_image_Y = 180
import numpy as np

import time

class MyFileDropTarget(wx.FileDropTarget):
    def __init__(self, panel):
        wx.FileDropTarget.__init__(self)
        self.panel = panel

    def OnDropFiles(self, x, y, filenames):
        for filepath in filenames:
            # Get extention
            extention =  "." + str(str(filenames).split(".")[-1:][-1])[:-2]
            wildcard = [".jpg",".jpeg",".png",".bmp",".mpg",".mpeg",".mp2",".mp4",".avi"]

            if extention in wildcard:
                self.panel.updateImage(filepath)
                self.panel.updateText(filepath)
                self.panel.GetTopLevelParent().setstatusbarTXT(filepath)

            else:
                # Ignore
                DnDFrame.updateText("Not supported file.")

        # Return True to accept the data, False to veto it.
        return True


class DnDPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent=parent)

        # Image box
        img = wx.Image(GUI_image_X, GUI_image_Y)
        self.imageCtrl = wx.StaticBitmap(self, wx.ID_ANY, wx.Bitmap(img))

        # Text box
        self.fileTextCtrl = wx.TextCtrl(self, style=wx.TE_MULTILINE|wx.HSCROLL|wx.TE_READONLY)
        self.SetDropTarget(MyFileDropTarget(self))

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.imageCtrl, 1, wx.ALL, 5)
        sizer.Add(self.fileTextCtrl, 1, wx.EXPAND|wx.ALL, 5)
        self.SetSizer(sizer)
        self.Fit()

    #----------------------------------------------------------------------
    def updateText(self, text):
        self.fileTextCtrl.SetValue(text)

    #----------------------------------------------------------------------
    def updateImage(self, fileaddress):
        self.imageCtrl.SetBitmap(wx.Bitmap(fileaddress))


class DnDFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, parent=None, title="GUI test", style=wx.DEFAULT_FRAME_STYLE ^ wx.RESIZE_BORDER)

        self.InitMenu()
        self.panel = DnDPanel(self)
        self.Fit()
        self.Show()

    def InitMenu(self):
        # Menubar
        menubar = wx.MenuBar()
        fileMenu = wx.Menu()
        f_item1 = fileMenu.Append(wx.ID_ANY, '&Load file(L)', 'Load image file')
        menubar.Append(fileMenu, '&File(F)')

        self.SetMenuBar(menubar)

        # Event call definition
        self.Bind(wx.EVT_MENU, self.OnLoad, f_item1)

        # Frame definition
        self.Centre()

        # Status bar
        self.CreateStatusBar()
        self.SetStatusText("Please drop/load a image file.")
        self.GetStatusBar().SetBackgroundColour(None)
        self.Show(True)

    def OnLoad(self, event):
        # Call a dialog
        self.dirname = ''
        wildcard = "Supported media files (*.jpg,*.jpeg,*.png,*.bmp,*.mpg,*.mpeg,*.mp2,*.mp4,*.avi))|*.jpg;*.jpeg;*.png;*.bmp;*.mpg;*.mpeg;*.mp2;*.mp4;*.avi"
        dlg = wx.FileDialog(self, "Please select a file.", self.dirname, defaultFile="", wildcard=wildcard, style= wx.FD_OPEN )

        if dlg.ShowModal() == wx.ID_OK:
            # Selected
            self.filename = dlg.GetFilename()
            self.dirname = dlg.GetDirectory()
            with open(os.path.join(self.dirname, self.filename), 'r') as f:
                self.panel.updateText(self.dirname + "\\" + self.filename)
                self.panel.updateImage(self.dirname + "\\" + self.filename)
                self.SetStatusText(self.dirname + "\\" + self.filename)

        else:
            # Cancelled
            self.panel.updateText("")
            self.SetStatusText("Please drop/load a image file.")
        dlg.Destroy()

    def updateImage(self, img):
        time_sta = time.perf_counter()
        a = np.shape(img)
        img2 = wx.ImageFromData(a[0], a[1], img)
        bmp = wx.BitmapFromImage(img2)
        self.panel.imageCtrl.SetBitmap(bmp)
        print(time.perf_counter() - time_sta)

if __name__ == "__main__":
    app = wx.App(False)
    frame = DnDFrame()
    app.MainLoop()