#!/usr/bin/env python

from PIL import Image
import numpy as np
import operator

raster = np.zeros((48, 64))
pix_map_left = np.loadtxt("pix_map_left.csv", delimiter=',')
pix_map_right = np.loadtxt("pix_map_right.csv", delimiter=',')

for r in range(0, 48):
    for c in range(0, 32):
        raster.flat[int(pix_map_left[r, c])] = 255
    for c in range(32, 64):
        raster.flat[int(pix_map_right[r, c-32])] = 255

im = Image.fromarray(raster)
im.save("test.gif")
