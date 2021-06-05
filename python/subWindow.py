# -*- coding: utf-8 -*-
"""
Created on Thu May  2 10:13:46 2019

@author: Owner
"""

import cv2
import numpy as np


class subWin():
    def __init__(self):
        pass
        
    def subWindowSetup(self):
        cv2.namedWindow("setup", cv2.WINDOW_AUTOSIZE)
        #cv2.resizeWindow("setup", 540, 300)
        #cv2.moveWindow("setup", 30, 0)
        cv2.imshow("setup", self.logo_image)
        cv2.waitKey(1)
        
    def setupWindowDestroy(self):
        cv2.destroyWindow("setup")
        cv2.namedWindow("sub_win", cv2.WINDOW_NORMAL)
        cv2.resizeWindow("sub_win", 1072, 602)
        cv2.moveWindow("sub_win", 30, 0)
        
        
    def resetSubWindow(self, label_text="", label_text2 = ""):
        self.current_monitor_img2 = np.array(self.bg_image)
        for name in self.Thumbnails[self.group_reserve].keys():
            key_pos =np.argwhere(np.array(self.keyBoard_lists)==name)
           
            self.current_monitor_img2[410+key_pos[0][0]*60:450+key_pos[0][0]*60, 35+key_pos[0][1]*67:98+key_pos[0][1]*67] = \
                                    self.Thumbnails[self.group_reserve][name]
        cv2.putText(self.current_monitor_img2,label_text,(680,280), self.font, 0.5,(255,255,255),0,cv2.LINE_AA)
        self.current_monitor_img2[543-int(self.speed*0.9)     :580-int(self.speed*0.9),     552 :577]  = self.meter_image
        self.current_monitor_img2[523-int(self.volume*1.8)    :560-int(self.volume*1.8),    480 :505]  = self.meter_image
        self.current_monitor_img2[543-int(self.contrast*0.9)  :580-int(self.contrast*0.9),  765 :790]  = self.meter_image
        self.current_monitor_img2[543-int(self.brightness*0.9):580-int(self.brightness*0.9),838:863] = self.meter_image
        self.ChangeScene_subWin()
        
        
    def update_subWin(self):
        ####################################################
        #       MOVIE FRAMES
        ####################################################
        img = np.array(self.current_monitor_img1)
        percentage = int(self.current_frame_number * 103 / self.current_all_frame_num)
        cv2.rectangle(img,(590, 404), (592+percentage, 412),(0,0,255),-1) 
        cv2.putText(img,str(self.current_frame_number)+"/"+ str(self.current_all_frame_num),(580,430), self.font, 0.5,(255,255,255),0,cv2.LINE_AA)
        cv2.putText(img,"P-R : "+ str(self.Processsing_ratio) + "%",(580,470), self.font, 0.5,(255,255,255),0,cv2.LINE_AA)
        
        img[6:341,23:491] = cv2.resize(self.current_frame, (468,335))
        cv2.imshow("sub_win", img)


    def ChangeScene_subWin(self):
        key_pos =np.argwhere(np.array(self.keyBoard_lists)==self.playing_scene)
        self.current_monitor_img1 = np.array(self.current_monitor_img2)
        cv2.rectangle(self.current_monitor_img1,(35+key_pos[0][1]*67, 410+key_pos[0][0]*60), (98+key_pos[0][1]*67, 450+key_pos[0][0]*60),(255,255,0),2)        
        cv2.rectangle(self.current_monitor_img1,(123+self.group_reserve*30, 354), (147+self.group_reserve*30, 376),(0,255,255),2)
        cv2.putText(self.current_monitor_img2,"Playing->"+str(self.group_playing)+self.playing_scene,(600,250), self.font, 0.8,(255,255,255),2,cv2.LINE_AA)
        self.current_all_frame_num = self.Frame_numbers[self.group_playing][self.playing_scene]


    def ExitSubWin(self):
        print("sub window destroy")
        cv2.destroyWindow("sub_win")
        