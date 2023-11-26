#!/usr/bin/env python

from PIL import Image
import numpy as np
import operator

raster = np.zeros((64, 80))
pix_map = np.loadtxt("pix_map.csv")

for r in range(0, 48):
    for c in range(0, 32):
        raster.flat[int(pix_map[r, c])] = 255

im = Image.fromarray(raster)
im.save("test.gif")
