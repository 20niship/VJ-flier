# -*- coding: utf-8 -*-
"""
Created on Fri Apr 26 23:05:52 2019

@author: Owner
"""

import cv2  #pip install opencv-python
import sys
from pygame import mixer
import numpy as np
import time

from subWindow import subWin
from mainWindow import mainWin
from movie_load import *
from DataManager import *
from Network import Server, Client


class VJmain(mainWin, subWin):   
    logo_image = cv2.imread("figures/logo.jpg", 1)
    Movie_Thumbnail = np.array((12,18))
    black_img = cv2.imread("figures/black.jpg")
    bg_image = cv2.imread("figures/bg.jpg", 1)
    no_img = cv2.imread("figures/no_img.jpg", 1)
    meter_image = cv2.imread("figures/meter.png",1)
    MovieList_length = 26
    group_num = 10
    keyBoard_lists = [["q", "w", "e", "r","t","y"],["a","s","d","f","g","h"],["z","x","c","v","b","n"]]
    font = cv2.FONT_HERSHEY_SIMPLEX
    
    def __init__(self, dual, net_config):
        #net_config = (0-> no_network, 1->server, 2->client)

        p = getPickleParams()
        """
        main window
        """
        self.volume = p['volume']
        self.speed = p['speed']
        self.contrast = p['contrast']
        self.brightness = p['brightness']
        self.LoadFileDir = "C:/Users/Owner/Pictures"
                
        self.dual = dual
        self.sounds = []
        self.MovieCaps = []
        self.sound_enable = []
        for i in range(12):
            self.sounds.append({})
            self.MovieCaps.append({})
            self.sound_enable.append({})

        """
        sub window
        """
        self.current_monitor_img1 = self.bg_image  #再生する動画が変われば再描画（現在の画面そのまま）
        self.current_monitor_img2 = self.bg_image  #グループが変われば再描画（赤枠が描画されていない状態）
        self.Frame_numbers = []
        for i in range(10):
            self.Frame_numbers.append({})
        self.current_frame_number = 0
        self.current_all_frame_num = 0
        
        self.meter_move = 0

        """
        Setup
        """
        mixer.init(frequency = 44100)
        self.LoadFiles()
        self.subWindowSetup()
        self.current_frame = []
        self.black_movie = cv2.VideoCapture("data/black.mp4")
        self.show = False
        self.group_playing = 1 #Config by Function keys
        self.group_reserve = 1
        self.playing_scene = "d"
        self.Thumbnails = self.getFirstFrames()
        self.setupWindowDestroy()
        self.resetSubWindow()
        if self.dual: self.MainWindowSetup()
        self.start_time = 0.
        self.Processsing_ratio = 0.
        mixer.music.set_volume(1.0)
        cv2.setMouseCallback("sub_win", self.mouse_event)
        
        self.server = net_config
        if self.server == 2:
            print("This os client PC -> mute the audio")
            for i in range(12):
                for name in self.sound_enable[i].keys():
                    self.sound_enable[i][name] = False

        self.sock = None
        if self.server == 1:
            self.sock = Server()
            self.sock.SetupNetwork()
        elif self.server == 2:
            self.sock = Client()
            self.sock.SetupNetwork()

    #############################################################
    #                MAIN LOOP
    ###################a##########################################
    def run(self):
        self.start_time = time.perf_counter()
        while True:
            ret, img = self.MovieCaps[self.group_playing][self.playing_scene].read()
            if not ret:
                self.current_frame_number = 0
                self.start_time = time.perf_counter()
                self.MovieCaps[self.group_playing][self.playing_scene].set(cv2.CAP_PROP_POS_FRAMES, 0)
            else: 
                self.current_frame_number += 1
                self.current_frame = cv2.addWeighted(img, self.contrast/100, img, 0, self.brightness-100)
    
                if self.dual: self.update_mainWin()
                self.update_subWin()


            if (self.server==2):
                data, addr = self.sock.getMsg()
                if(data == 'exit'):
                    break
                elif(len(data)==2):
                    group = int(data[0])
                    scene = data[1]
                    if((group < 9) and (group > 0) and (scene in self.MovieCaps[group].keys())):
                            print("change movie by network! -->" + data)
                            self.group_reserve = group
                            self.resetSubWindow()
                            self.current_frame_number = 0
                            self.ChangeScene_mainWin(scene)
                            self.ChangeScene_subWin()
                            self.start_time = time.perf_counter()
             
            # qキーが押されたら途中終了
            wait_time = ((self.current_frame_number*100)/(30.*self.speed) - (time.perf_counter() - self.start_time))*1000
            key2 = cv2.waitKey(int(min(max(wait_time, 1), 50))) & 0xff
            self.Processsing_ratio = min(int((1 - wait_time/33.) * 100), 1000)

            if(key2 is not 255):
                if(47 < key2 and key2 < 58):
                        self.group_reserve = key2-48
                        print("group changed to ->" + str(key2-48))
                        self.resetSubWindow()
                
                elif(chr(key2) in self.MovieCaps[self.group_reserve].keys()):
                        print("scene changed to ->" + chr(key2))
                        self.current_frame_number = 0
                        self.ChangeScene_mainWin(chr(key2))
                        self.ChangeScene_subWin()
                        
                        if self.server==1:
                            msg = str(self.group_reserve) + self.playing_scene
                            print("scene change msg send! -->" + msg)
                            self.sock.sendMsg(msg)
                        self.start_time = time.perf_counter()
                
                elif key2 == 27: #ESC
                    if self.server==1:
                        self.sock.sendMsg('exit')
                    break
                elif key2 == 43: #Speed Up
                    self.speed = max(self.speed-1, 10)
                    self.resetSubWindow("movie play speed-->"+str(self.speed)+"%")
                elif key2 == 45:
                    self.speed = min(self.speed+1, 200)
                    self.resetSubWindow("movie play speed-->"+str(self.speed)+"%")
                    print("speed up")

    
    def exitApp(self):
        self.exitMainWin()
        self.ExitSubWin()
        p = {}
        p['speed'] = self.speed
        p['volume'] = self.volume
        p['brightness'] = self.brightness
        p['contrast'] = self.contrast
        p['LoadFileDir'] = self.LoadFileDir
        p['Thumbs'] =self.Thumbnailss
        SavePickleParams(p)
        sys.exit()

        
    def mouse_event(self, event, x, y, flags, param):
        # 左クリックで赤い円形を生成
        if(event == cv2.EVENT_RBUTTONUP):
            if(x<440 and y>390):
                print("movie choosed -> start configuration")
                width = int((x-35)/67)
                height = int((y-390)/60)
                key_char = self.keyBoard_lists[height][width]
                Movie, Audio, audio_enable, self.LoadFileDir = edit_one_scene(self.group_reserve, key_char, self.LoadFileDir)
                print(Movie, Audio, audio_enable)
                if(Movie is not None):
                    self.MovieCaps[self.group_reserve][key_char] = Movie
                    self.MovieCaps[self.group_reserve][key_char].set(cv2.CAP_PROP_POS_FRAMES, 0)
                    self.Frame_numbers[self.group_reserve][key_char] = self.MovieCaps[self.group_reserve][key_char].get(cv2.CAP_PROP_FRAME_COUNT)
                    print("finished loading video")
                if(Audio is not None):
                    self.sounds[self.group_reserve][key_char] = Audio
                self.sound_enable[self.group_reserve][key_char] = audio_enable
                self.Thumbnails[self.group_reserve][key_char] = self.getFirstFrame(self.group_reserve, key_char)
                self.resetSubWindow()
                    
        elif(event == cv2.EVENT_LBUTTONDOWN and 373<y and 562>y):
            if(480<x and x<510):self.meter_move = 1
            elif(550<x and x<580): self.meter_move = 2
            elif(750<x and x<790): self.meter_move = 3 #Contrast
            elif(830<x and x<870): self.meter_move = 4 #Brightness
            
            print("meter move" + str(self.meter_move))
        elif(self.meter_move>0 and event == cv2.EVENT_MOUSEMOVE):
            if self.meter_move == 2:
                self.speed = max(int((562-y)/0.945), 10)
                self.speed = min(200, self.speed)
                print("movie play speed-->"+str(self.speed)+"%")
                self.resetSubWindow("movie play speed-->"+str(self.speed)+"%")
                
            elif self.meter_move == 1:
                self.volume = max(int(307-y/1.8), 0)
                self.volume = min(100, self.volume)
                print("Audio Volume-->"+str(self.volume)+"%")
                self.resetSubWindow("Audio Volume-->"+str(self.volume)+"%")
                if self.sound_enable[self.group_playing][self.playing_scene]:
                    self.sounds[self.group_playing][self.playing_scene].set_volume(self.volume/100.)
                    
            elif self.meter_move == 3:
                self.contrast = max(int((562-y)/0.945), 10)
                self.contrast = min(200, self.contrast)
                print("Contrast-->"+str(self.contrast)+"%")
                self.resetSubWindow("Contrast-->"+str(self.contrast)+"%") 
                    
            elif self.meter_move == 4:
                self.brightness = max(int((562-y)/0.945), 10)
                self.brightness = min(200, self.brightness)
                print("Brightness-->"+str(self.brightness)+"%")
                self.resetSubWindow("Brightness-->"+str(self.brightness)+"%")    
                    
        if(event == cv2.EVENT_LBUTTONUP):
            self.meter_move=0
