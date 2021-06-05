# -*- coding: utf-8 -*-
"""
Created on Thu May  2 20:07:35 2019

@author: Owner
"""

import os
import pickle


Pickle_file_dir = "data/"
Pickle_Params_fname = "parameters"

def SavePickleParams(A):
#    A = {}
#    A['LoadFileDir'] = "C:/Users/Owner/Pictures"
#    A['volume'] = 100
#    A['speed'] = 100.
#    A['contrast'] = 100
#    A['brightness'] = 100
#    A['FileName'] = '1d'

    f = open(Pickle_file_dir + Pickle_Params_fname + ".pickle",'wb')
    pickle.dump(A,f)
    f.close
    return 0
        
def getPickleParams():
    f = open(Pickle_file_dir + Pickle_Params_fname + ".pickle",'rb')
    A = pickle.load(f)
    f.close()
    return A
