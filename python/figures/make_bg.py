# -*- coding: utf-8 -*-
"""
Created on Thu May  2 16:09:35 2019

@author: Owner
"""
import cv2
import numpy as np
import pylab as plt



bg_image = cv2.imread("bg.jpg", 1)
no_img = cv2.imread("no_img.jpg", 1)

for i in range(3):
    for j in range(6):
        bg_image[410+i*60:450+i*60, 35+j*67:98+j*67] = cv2.imread("no_img.jpg", 1)

cv2.imwrite('bg2.png',bg_image) 

