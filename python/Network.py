# -*- coding: utf-8 -*-

import sys
from socket import *
import time

class Server:
    def __init__(self):
        pass
    
    def SetupNetwork(self, addr = ""):
        print("network setup started")
        self.port = 13000
        self.addr = ("", self.port)
        self.UDPSock = socket(AF_INET, SOCK_DGRAM)
        self.UDPSock.bind(self.addr)  #Serverの時のみ
        self.UDPSock.settimeout(0.0001)
        self.ClientAddr_list = []
        print("Connected (Server mode)!!")
        print("Network info -->" + str(addr) + ", " + str(self.port))
        setup = True
        while setup:
            data, addr = self.getMsg()
            if((data is not "0")and not (addr in self.ClientAddr_list)):
                print("new client found --"+str(data))
                self.ClientAddr_list.append(addr)
            if(len(self.ClientAddr_list) > 1):
                setup = False
        return True
        
    def getMsg(self):
        try:
            (data, addr) = self.UDPSock.recvfrom(1024)
        except timeout:
            return "0", 0
        str_data = data.decode('utf-8')
        return str_data, addr
        
    def sendMsg(self, msg):
        for i in self.ClientAddr_list:
            self.UDPSock.sendto(msg.encode('utf-8'), i)
        
    def CloseNetwork(self):
        self.UDPSock.close()
        print("This PC is disconnected successfully")

        
        
        
class Client:
    def __init__(self):
        pass
    
    def SetupNetwork(self, addr = ""):
        print("network setup started")
        self.port = 13000
        self.addr = ("169.254.154.114", self.port)
        self.UDPSock = socket(AF_INET, SOCK_DGRAM)
        self.UDPSock.settimeout(0.0001)
        self.ClientAddr_list = []
        print("Connected (Client)!!")
        print("Network info -->" + str(addr) + ", " + str(self.port))
        
        return True
        
    def getMsg(self):
        try:
            (data, addr) = self.UDPSock.recvfrom(1024)
        except timeout:
            return "0", 0
        str_data = data.decode('utf-8')
        return str_data, addr
        
    def sendMsg(self, msg):
        for i in self.ClientAddr_list:
            self.UDPSock.sendto(msg.encode('utf-8'), i)
        
    def CloseNetwork(self):
        self.UDPSock.close()
        print("This PC is disconnected successfully")

        
        
#        
#
##if __name__ is "__main__":
#print("start network")
#S = Server()
#S.SetupNetwork()
#print("setup finished")
#S.sendMsg("asdfghjk")
#S.CloseNetwork()
