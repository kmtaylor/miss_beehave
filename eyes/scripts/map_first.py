#!/usr/bin/env python

from PIL import Image
import numpy as np
import operator

raster = np.zeros((64, 80))
pix_map = np.zeros((64, 80))

def add_location(raw, rast):
    pix_map[rast] = raw
    raster.flat[raw] = 255

def move(a, b):
    return tuple(map(operator.add, a, b))

location = (16, 16)
add_location( 768, location); location = move(location, (-1, 0))
add_location( 135, location); location = move(location, (-1, 0))
add_location( 134, location); location = move(location, (-1, 0))
add_location( 133, location); location = move(location, (-1, 0))
add_location( 132, location); location = move(location, (-1, 0))
add_location( 131, location); location = move(location, (-1, 0))
add_location( 130, location); location = move(location, (-1, 0))
add_location( 129, location); location = move(location, (-1, 0))
add_location( 128, location); location = move(location, (-1, 0))
add_location( 376, location); location = move(location, (-1, 0))
add_location( 377, location); location = move(location, (-1, 0))
add_location( 378, location); location = move(location, (-1, 0))
add_location( 379, location); location = move(location, (-1, 0))
add_location( 380, location); location = move(location, (-1, 0))
add_location( 381, location); location = move(location, (-1, 0))
add_location( 382, location); location = move(location, (-1, 0))
add_location( 383, location); location = move(location, (-1, 0))

location = (17, 16)
add_location( 769, location); location = move(location, (1, 0))
add_location( 770, location); location = move(location, (1, 0))
add_location( 771, location); location = move(location, (1, 0))
add_location( 772, location); location = move(location, (1, 0))
add_location( 773, location); location = move(location, (1, 0))
add_location( 774, location); location = move(location, (1, 0))
add_location( 775, location); location = move(location, (1, 0))
add_location( 776, location); location = move(location, (1, 0))
add_location( 777, location); location = move(location, (1, 0))
add_location( 778, location); location = move(location, (1, 0))
add_location( 779, location); location = move(location, (1, 0))
add_location( 780, location); location = move(location, (1, 0))
add_location( 781, location); location = move(location, (1, 0))
add_location( 782, location); location = move(location, (1, 0))
add_location( 783, location); location = move(location, (1, 0))
add_location(1408, location); location = move(location, (1, 0))
add_location(1409, location); location = move(location, (1, 0))
add_location(1410, location); location = move(location, (1, 0))
add_location(1411, location); location = move(location, (1, 0))
add_location(1412, location); location = move(location, (1, 0))
add_location(1413, location); location = move(location, (1, 0))
add_location(1414, location); location = move(location, (1, 0))
add_location(1415, location); location = move(location, (1, 0))
add_location(1152, location); location = move(location, (1, 0))
add_location(1153, location); location = move(location, (1, 0))
add_location(1154, location); location = move(location, (1, 0))
add_location(1155, location); location = move(location, (1, 0))
add_location(1156, location); location = move(location, (1, 0))
add_location(1157, location); location = move(location, (1, 0))
add_location(1158, location); location = move(location, (1, 0))
add_location(1159, location); location = move(location, (1, 0))

def raw_from_centre(r, c):
    centre = int(pix_map[r, 16])
    
    if r < 8:
        if c % 2: # Odd
            return centre - 16*((c-16)//2) - 15 + 2*(r % 8)
        else:
            return centre - 16*((c-16)//2)
    
    if r < 16:
        if c % 2: # Odd
            return centre + 16*((c-16)//2) + 15 - 2*(r % 8)
        else:
            return centre + 16*((c-16)//2)
    
    if r < 32:
        if c % 2: # Odd
            return centre + 32*((c-16)//2) + 31 - 2*(r % 16)
        else:
            return centre + 32*((c-16)//2)

    if r < 40:
        if c % 2: # Odd
            return centre + 16*((c-16)//2) + 15 - 2*(r % 8)
        else:
            return centre + 16*((c-16)//2)

    if r < 48:
        if c % 2: # Odd
            return centre + 16*((c-16)//2) + 15 - 2*(r % 8)
        else:
            return centre + 16*((c-16)//2)

    return 0

for c in range(0, 32):
    for r in range(0, 48):
        add_location(raw_from_centre(r, c), (r, c))

im = Image.fromarray(raster)
im.save("test.gif")
np.savetxt("pix_map.csv", pix_map[0:48, 0:32], fmt = "%4d")
