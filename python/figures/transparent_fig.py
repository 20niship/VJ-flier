#-*- coding:utf-8 -*-

from PIL import Image, ImageDraw
import numpy as np
import os
import glob


def select_color(color):
  mean = np.array(color).mean(axis=0)
  return (255,255,255,0) if mean >= 220 else color

def to_touka(img):
  w, h = img.size
  touka_img = Image.new('RGBA', (w, h))
  np.array([[touka_img.putpixel((x, y), select_color(img.getpixel((x,y)))) for x in range(w)] for y in range(h)])
  return touka_img

def main(file_name):
    original_img = Image.open("meter.png").convert("RGB")
    #透過画像へ変換して保存
    to_touka(original_img).save("2.png")

if __name__ is "__main__":    
    main("meter")