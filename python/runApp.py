# -*- coding: utf-8 -*-
"""
Created on Sun May  5 14:12:32 2019

@author: Owner
"""

import ctypes  # An included library with Python install.   
from mainApp import VJmain
from pygame import mixer

def check_msg(msg = "test"):
    a = ctypes.windll.user32.MessageBoxW(0, msg, "CAUTION", 4)
    return a==6

print("system start")
MainApp = None
if(check_msg("Connect Network (STP/IP Pro.)?")):
    if(check_msg("mode choose: yes->server, no->client")):
        MainApp = VJmain(check_msg("フルスクリーン、デュアルモニタで使用しますか？"), 1)  #server mode 
    else:
        MainApp = VJmain(check_msg("フルスクリーン、デュアルモニタで使用しますか？"), 2)  #client mode
else:
    MainApp = VJmain(check_msg("フルスクリーン、デュアルモニタで使用しますか？"), 0)
mixer.init(frequency = 44100)

MainApp.run()
MainApp.exitApp()
