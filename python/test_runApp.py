# -*- coding: utf-8 -*-
"""
Created on Fri Apr 26 23:05:52 2019

@author: Owner
"""

import cv2  # pip install opencv-python
import sys
import pygame  # pip insstall pygame
import numpy as np
import os
from subWin2 import DnDFrame
import wx


class VJmain:
    def __init__(self, NameDict):
        self.NameDict = NameDict
        self.window_name = 'screen'
        self.blackImg = cv2.imread("figures/black.jpg")
        self.MovieList_length = 26
        self.show = False
        self.group_playing = 0  # Config by Function keys
        self.group_reserve = 0
        self.playing_scene = "d"

        self.sounds = []
        self.MovieCaps = []
        self.sound_enable = []
        for i in range(12):
            self.sounds.append({})
            self.MovieCaps.append({})
            self.sound_enable.append({})

        pygame.mixer.init(frequency=44100)  # 初期設定

        app = wx.App(False)
        self.subWin = DnDFrame()


        self.LoadFiles(NameDict)

        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)
        #cv2.setWindowProperty(self.window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

        print(self.sounds[0])
        print(self.MovieCaps[0])
        # sys.exit()

    #############################################################
    #                LOAD FILES
    #############################################################

    def LoadFiles(self, NameDict):
        # Audio
        for name in NameDict[0].keys():
            print("data/1"+self.NameDict[0][name] + ".wav")
            if (os.path.exists("data/"+self.NameDict[0][name] + ".wav")):
                self.sounds[0][name] = pygame.mixer.Sound("data/"+self.NameDict[0][name] + ".wav")
                self.sound_enable[0][name] = True
            else:
                self.sound_enable[0][name] = False

            # Video
            if (os.path.exists("data/"+self.NameDict[0][name] + ".mp4")):
                self.MovieCaps[0][name] = cv2.VideoCapture("data/"+self.NameDict[0][name] + ".mp4")
                print(self.MovieCaps[0][name].get(cv2.CAP_PROP_FPS))
                self.MovieCaps[0][name].set(cv2.CAP_PROP_FPS, 30)


    #############################################################
    #                MAIN LOOP
    #############################################################
    def ChangeScene(self, scene_str):
        if (self.sound_enable[self.group_playing][self.playing_scene] is True):
            self.sounds[self.group_playing][self.playing_scene].stop()
        self.MovieCaps[self.group_playing][self.playing_scene].set(cv2.CAP_PROP_POS_FRAMES, 0)

        self.playing_scene = scene_str
        print("scene changed to ->" + scene_str)
        if (self.sound_enable[self.group_playing][self.playing_scene] is True):
            self.sounds[self.group_playing][self.playing_scene].play()


    def run(self):
        while True:
            # フレームを取得
            _, frame = self.MovieCaps[self.group_playing][self.playing_scene].read()
            cv2.imshow(self.window_name, frame)
            self.subWin.updateImage(frame)

            # qキーが押されたら途中終了
            raw_key = cv2.waitKey(15)
            if (raw_key is not 255):
                key2 = raw_key & 0xff

                if (raw_key > 65000):
                    f_key = key2 - 144
                    if ((-1 < f_key) and (f_key < 10) and (f_key is not self.group_playing)):
                        self.ggroup_reserve = f_key

                else:
                    if (chr(key2) in self.MovieCaps[self.group_playing].keys()):
                        self.ChangeScene(chr(key2))

                    if key2 == 27:  # ESC
                        break

        cv2.destroyWindow(self.window_name)


    def exitApp(self):
        self.sounds[self.group_playing][self.playing_scene].stop()
        pygame.mixer.music.stop()

        for i in range(12):
            for name in self.MovieCaps[i].keys():
                self.MovieCaps[i][name].release()
        # ser.close()
        sys.exit()


if __name__ is "__main__":
    file_path = 'sample.mp4'
    delay = 15
    # ser=OpenPort()
    Dict = []
    for i in range(12):
        Dict.append({})
    Dict[0]["d"] = "1d"
    Dict[0]["a"] = "1a"
    Dict[0]["t"] = "1t"

    MainApp = VJmain(Dict)
    # MainApp.ChangeScene("a")a
    MainApp.run()

    MainApp.exitApp()
