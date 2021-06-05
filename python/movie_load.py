# -*- coding: utf-8 -*-
"""
Created on Thu May  2 20:07:35 2019

@author: Owner
"""

import os
import tkinter
import tkinter.filedialog
import tkinter.messagebox
import tkinter.ttk as ttk
from tkinter import font
import cv2  #pip install opencv-python
import sys
from pygame import mixer
import glob
import shutil


class EditOneScene(tkinter.Frame):
    def __init__(self, master=None, group=1, scene="a", iDir = os.path.abspath(os.path.dirname(__file__))):
        super().__init__(master)
        self.pack()
        self.MovieFileName = tkinter.StringVar()
        self.AudioFileName = tkinter.StringVar()
        self.audio_pre_playing = False
        
        self.group = group
        self.scene = scene
        self.iDir = iDir
        
        
        #self.MovieBtn = tkinter.Button(self, bg='#808080', fg='#ffffff', text="Change MOVIE", command=self.LoadNewMovie, width=80, height = 40)
        MovieBtn = tkinter.Button(bg='#808080', fg='#ffffff', text="Change MOVIE", command=self.LoadNewMovie)
        MovieBtn.place(x=250, y=20, height=40, width=90)

        AudioBtn = tkinter.Button(bg='#808080', fg='#ffffff', text="Change Audio", command=self.LoadNewAudio)
        AudioBtn.place(x=250, y=70, height=40, width=90)
        
        VideoPlayBtn = tkinter.Button(bg='#808080', fg='#ffffff', text="Play", command=self.PlayMovie)
        VideoPlayBtn.place(x=350, y=20, width=50, height = 40)
        
        AudioPlayBtn = tkinter.Button(bg='#808080', fg='#ffffff', text="Play", command=self.PlayAudio)
        AudioPlayBtn.place(x=350, y=70, width=50, height = 40)
        
        self.val = tkinter.BooleanVar()
        self.val.set(False)
        chkBtn = tkinter.Checkbutton(text='', variable = self.val)
        chkBtn.place(x=30, y=130)

        self.ButtonQuit = tkinter.Button(bg='#808080', fg='#ffffff', text="更新", command=self.SaveData)
        self.ButtonQuit.place(x=210, y=120, width=150, height = 40)    
        
        MoviefNameEntry = ttk.Entry(textvariable= self.MovieFileName)
        MoviefNameEntry.place(x=10, y=20, width=230, height = 40)
        
        AudiofNameEntry = ttk.Entry(textvariable= self.AudioFileName)
        AudiofNameEntry.place(x=10, y=70, width=230, height = 40)

        font3 = font.Font(family='Times', size=15)
        font2 = font.Font(family='Times', size=10)
        label = tkinter.Label(text="Enable Audio",bg = "black",fg="white", font=font3)
        label.place(x=50, y=130)
        self.log = tkinter.StringVar()
        self.log.set("log here")
        label2 = tkinter.Label(textvariable=self.log, bg = "#c0c0c0",fg="black", font=font2, justify='left')
        label2.place(x=15, y=175, width=400, height = 100)
        
        self.MovieFileName.set("Movie path here")
        self.AudioFileName.set("Audio path here")

        self.Movie = None
        self.audio = None
        
        
    def getPath(self, fType= [("","*")], title = "Open", msg = "choose one file"):
        file = tkinter.filedialog.askopenfilename(filetypes = fType,initialdir = self.iDir)
        f2 = file.split("/")
        self.iDir = "/".join(f2[:-1])
        return file
        
    def LoadNewMovie(self):
        self.log.set("Load New Video")
        file_path = self.getPath(fType=[("","*.mp4")], title = "Open", msg = "choose one MOVIE file")
        if file_path == "": return 0
        else:
            self.Movie = cv2.VideoCapture(file_path)
            self.Movie.set(cv2.CAP_PROP_FPS, 30)
        self.MovieFileName.set(file_path)
        self.ButtonQuit["bg"] = "yellow"
        self.log.set("Loaded New Video\n--> "+ file_path+"\nplease update the data!!")


    def LoadNewAudio(self):
        self.log.set("Load New Audio")
        file_path = self.getPath(fType=[("","*.wav")], title = "Open", msg = "choose one MOVIE file")
        if file_path == "": return 0
        else:
            mixer.init(frequency = 44100)
            self.audio=mixer.Sound(file_path)
            self.val.set(True)
        self.AudioFileName.set(file_path)
        self.ButtonQuit["bg"] = "yellow"
        self.ButtonQuit["fg"] = "black"
        self.log.set("Loaded New Audio\n--> "+ file_path+"\nplease update the data!!")

            
    def QuitApp(self):
        print("quit this App")
        self.master.destroy()
        
    def PlayMovie(self):
        f_name = self.MovieFileName.get()
        self.log.set("Start video playing\n--> "+ f_name)
        cap =cv2.VideoCapture(self.MovieFileName.get())
        while True:
            ret, frame = cap.read()
            if not ret: 
                break
            cv2.imshow('frame',frame)
            if cv2.waitKey(23)&0xff==27: break
        cap.release()
        cv2.destroyWindow('frame')
        self.log.set("Finished video playing\n--> "+ f_name)
    
    def PlayAudio(self):
        f_name = self.AudioFileName.get()
        if self.audio_pre_playing:
            self.log.set("Stop audio\n--> "+ f_name)
            self.audio.stop()
            self.audio_pre_playing =False
        else:
            self.log.set("Audio playing\n--> "+ f_name)
            if(os.path.exists(f_name)):
                self.audio.play()
                self.audio_pre_playing = True

    def SaveData(self):
        if(os.path.exists(self.MovieFileName.get())):
            self.log.set("Saving Video Data\n-->"+str(self.group)+self.scene+".mp4")
            shutil.copyfile(self.MovieFileName.get(),"data/"+str(self.group)+self.scene+".mp4")
        if(os.path.exists(self.AudioFileName.get())):
            self.log.set("Saving Audio Data-->"+str(self.group)+self.scene+".wav")
            shutil.copyfile(self.AudioFileName.get(), "data/"+str(self.group)+self.scene+".wav")
        self.log.set("Finished Video Data")
        self.ButtonQuit["bg"] = "black"

        
def edit_one_scene(group, scene, iDir):
    root = tkinter.Tk()
    root.geometry("430x280")
    root.configure(bg='#000000')
    app = EditOneScene(root, group, scene, iDir)
    app.mainloop()
    if app.audio_pre_playing:
        app.audio.stop()
    audio_enable = app.val.get()
    if(app.audio == None): audio_enable = False
    return app.Movie, app.audio, audio_enable, app.iDir
    
    
    
if __name__ is "__main__":
     _,_,_,a = edit_one_scene(0,"a", "C:/Users/Owner/Pictures")
     print(a)
