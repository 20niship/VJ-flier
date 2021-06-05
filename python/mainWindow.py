# -*- coding: utf-8 -*-
"""
Created on Fri Apr 26 23:05:52 2019

@author: Owner
"""

import cv2  #pip install opencv-python
import sys    
from pygame import mixer  #pip insstall pygame
import numpy as np
import os

class mainWin():
    def __init__(self):
        pass
        #sys.exit()
        
    #############################################################
    #                LOAD FILES
    #############################################################
    
    def LoadFiles(self):
        #Audio
        for i in range(10):
            for name in ["q", "w", "e", "r","t","y","a","s","d","f","g","h","z","x","c","v","b","n"]:
                if(os.path.exists("data/"+str(i)+name+".wav")):
                    self.sounds[i][name] = mixer.Sound("data/"+str(i)+name+".wav")
                    print("audio loaded --> "+str(i)+name+".wav")
                    self.sound_enable[i][name] = True
                else:
                    self.sound_enable[i][name] = False
                #Video
                if(os.path.exists("data/"+str(i)+name+".mp4")):
                    self.MovieCaps[i][name] = cv2.VideoCapture("data/"+str(i)+name+".mp4")
                    self.MovieCaps[i][name].set(cv2.CAP_PROP_FPS, 30)
                    print("video loaded --> "+str(i)+name+".mp4")
                    self.Frame_numbers[i][name] = self.MovieCaps[i][name].get(cv2.CAP_PROP_FRAME_COUNT)
                    
    def MainWindowSetup(self):
        cv2.namedWindow("screen", cv2.WINDOW_NORMAL)
        cv2.moveWindow("screen", 1925, 0)
        cv2.setWindowProperty("screen", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)


    def getFirstFrames(self):
        #各動画の一枚目の画像を取り込む
        frames= []
        for i in range(self.group_num):
            frames.append({})
            
        for g in range(self.group_num):
            for i in range(26):
                if(chr(i+97) in self.MovieCaps[g].keys()):
                    frames[g][chr(i+97)] = self.getFirstFrame(g, chr(i+97))
        return frames
        
    def getFirstFrame(self, group, scene):
        self.MovieCaps[group][scene].set(cv2.CAP_PROP_POS_FRAMES, 20) 
        _, frame = self.MovieCaps[group][scene].read()
        self.MovieCaps[group][scene].set(cv2.CAP_PROP_POS_FRAMES, 0) 
        return cv2.resize(frame, (63,40))
            
            
    #############################################################
    #                MAIN LOOP
    #############################################################
    def ChangeScene_mainWin(self, scene_str):
        if(self.sound_enable[self.group_playing][self.playing_scene]):
            self.sounds[self.group_playing][self.playing_scene].stop()
        self.MovieCaps[self.group_playing][self.playing_scene].set(cv2.CAP_PROP_POS_FRAMES, 0) 
        
        self.playing_scene = scene_str
        self.group_playing = self.group_reserve
        if(self.sound_enable[self.group_playing][self.playing_scene]):
            self.sounds[self.group_playing][self.playing_scene].set_volume(self.volume/100.)
            self.sounds[self.group_playing][self.playing_scene].play()
    
            
    def update_mainWin(self):
        cv2.imshow("screen", self.current_frame)
        
    def exitMainWin(self):
        if self.sound_enable[self.group_playing][self.playing_scene]:
            self.sounds[self.group_playing][self.playing_scene].stop()
        mixer.music.stop()
        
        for i in range(self.group_num):
            for name in self.MovieCaps[i].keys():
                self.MovieCaps[i][name].release()
        cv2.destroyWindow("screen")
