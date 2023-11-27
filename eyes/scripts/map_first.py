#!/usr/bin/env python

from PIL import Image
import numpy as np
import operator

raster = np.zeros((48, 64))
pix_map = np.zeros((48, 64))

def add_location(raw, rast):
    pix_map[rast] = raw
    raster.flat[raw] = 255

def move(a, b):
    return tuple(map(operator.add, a, b))

location = (16, 47)
add_location( 647, location); location = move(location, ( 1, 0))
add_location( 646, location); location = move(location, ( 1, 0))
add_location( 645, location); location = move(location, ( 1, 0))
add_location( 644, location); location = move(location, ( 1, 0))
add_location( 643, location); location = move(location, ( 1, 0))
add_location( 642, location); location = move(location, ( 1, 0))
add_location( 641, location); location = move(location, ( 1, 0))
add_location( 640, location); location = move(location, ( 1, 0))
add_location( 903, location); location = move(location, ( 1, 0))
add_location( 902, location); location = move(location, ( 1, 0))
add_location( 901, location); location = move(location, ( 1, 0))
add_location( 900, location); location = move(location, ( 1, 0))
add_location( 899, location); location = move(location, ( 1, 0))
add_location( 898, location); location = move(location, ( 1, 0))
add_location( 897, location); location = move(location, ( 1, 0))
add_location( 896, location); location = move(location, ( 1, 0))
add_location(1159, location); location = move(location, ( 1, 0))
add_location(1158, location); location = move(location, ( 1, 0))
add_location(1157, location); location = move(location, ( 1, 0))
add_location(1156, location); location = move(location, ( 1, 0))
add_location(1155, location); location = move(location, ( 1, 0))
add_location(1154, location); location = move(location, ( 1, 0))
add_location(1153, location); location = move(location, ( 1, 0))
add_location(1152, location); location = move(location, ( 1, 0))
add_location(1415, location); location = move(location, ( 1, 0))
add_location(1414, location); location = move(location, ( 1, 0))
add_location(1413, location); location = move(location, ( 1, 0))
add_location(1412, location); location = move(location, ( 1, 0))
add_location(1411, location); location = move(location, ( 1, 0))
add_location(1410, location); location = move(location, ( 1, 0))
add_location(1409, location); location = move(location, ( 1, 0))
add_location(1408, location); location = move(location, ( 1, 0))

location = (15, 47)
add_location( 384, location); location = move(location, (-1, 0))
add_location( 385, location); location = move(location, (-1, 0))
add_location( 386, location); location = move(location, (-1, 0))
add_location( 387, location); location = move(location, (-1, 0))
add_location( 388, location); location = move(location, (-1, 0))
add_location( 389, location); location = move(location, (-1, 0))
add_location( 390, location); location = move(location, (-1, 0))
add_location( 391, location); location = move(location, (-1, 0))
add_location( 128, location); location = move(location, (-1, 0))
add_location( 129, location); location = move(location, (-1, 0))
add_location( 130, location); location = move(location, (-1, 0))
add_location( 131, location); location = move(location, (-1, 0))
add_location( 132, location); location = move(location, (-1, 0))
add_location( 133, location); location = move(location, (-1, 0))
add_location( 134, location); location = move(location, (-1, 0))
add_location( 135, location); location = move(location, (-1, 0))

location = (47, 15)
add_location(1664, location); location = move(location, (-1, 0))
add_location(1665, location); location = move(location, (-1, 0))
add_location(1666, location); location = move(location, (-1, 0))
add_location(1667, location); location = move(location, (-1, 0))
add_location(1668, location); location = move(location, (-1, 0))
add_location(1669, location); location = move(location, (-1, 0))
add_location(1670, location); location = move(location, (-1, 0))
add_location(1671, location); location = move(location, (-1, 0))
add_location(1920, location); location = move(location, (-1, 0))
add_location(1921, location); location = move(location, (-1, 0))
add_location(1922, location); location = move(location, (-1, 0))
add_location(1923, location); location = move(location, (-1, 0))
add_location(1924, location); location = move(location, (-1, 0))
add_location(1925, location); location = move(location, (-1, 0))
add_location(1926, location); location = move(location, (-1, 0))
add_location(1927, location); location = move(location, (-1, 0))
add_location(2688, location); location = move(location, (-1, 0))
add_location(2689, location); location = move(location, (-1, 0))
add_location(2690, location); location = move(location, (-1, 0))
add_location(2691, location); location = move(location, (-1, 0))
add_location(2692, location); location = move(location, (-1, 0))
add_location(2693, location); location = move(location, (-1, 0))
add_location(2694, location); location = move(location, (-1, 0))
add_location(2695, location); location = move(location, (-1, 0))
add_location(2944, location); location = move(location, (-1, 0))
add_location(2945, location); location = move(location, (-1, 0))
add_location(2946, location); location = move(location, (-1, 0))
add_location(2947, location); location = move(location, (-1, 0))
add_location(2948, location); location = move(location, (-1, 0))
add_location(2949, location); location = move(location, (-1, 0))
add_location(2950, location); location = move(location, (-1, 0))
add_location(2951, location); location = move(location, (-1, 0))
add_location(2176, location); location = move(location, (-1, 0))
add_location(2177, location); location = move(location, (-1, 0))
add_location(2178, location); location = move(location, (-1, 0))
add_location(2179, location); location = move(location, (-1, 0))
add_location(2180, location); location = move(location, (-1, 0))
add_location(2181, location); location = move(location, (-1, 0))
add_location(2182, location); location = move(location, (-1, 0))
add_location(2183, location); location = move(location, (-1, 0))
add_location(2432, location); location = move(location, (-1, 0))
add_location(2433, location); location = move(location, (-1, 0))
add_location(2434, location); location = move(location, (-1, 0))
add_location(2435, location); location = move(location, (-1, 0))
add_location(2436, location); location = move(location, (-1, 0))
add_location(2437, location); location = move(location, (-1, 0))
add_location(2438, location); location = move(location, (-1, 0))
add_location(2439, location); location = move(location, (-1, 0))

def raw_from_centre_left(r, c):
    centre = int(pix_map[r, 15])
    
    if r < 48:
        if c % 2: # Odd
            return centre - 16*((c-15)//2)
        else:
            return centre - 16*((c-15)//2) - 15 + 2*(r % 8)
    
    return 0

for c in range(0, 32):
    for r in range(0, 48):
        add_location(raw_from_centre_left(r, c), (r, c))

def raw_from_centre_right(r, c):
    centre = int(pix_map[r, 47])
    
    if r < 48:
        if c % 2: # Odd
            return centre - 16*((c-47)//2)
        else:
            return centre - 16*((c-47)//2) - 15 + 2*(r % 8)
    
    return 0

for c in range(32, 64):
    for r in range(0, 48):
        add_location(raw_from_centre_right(r, c), (r, c))

im = Image.fromarray(raster)
im.save("test.gif")
np.savetxt("pix_map_left.csv", pix_map[0:48, 0:32], fmt = "%4d")
np.savetxt("pix_map_right.csv", pix_map[0:48, 32:64], fmt = "%4d")
