#!/bin/bash

get_loopback() {
    loopback=`v4l2-ctl --list-devices | \
        grep "v4l2loopback" -A 1 | tail -1 | sed 's/[[:space:]]//g'`
}

get_loopback

play_gif() {
    ffmpeg -re -default_delay 2 -stream_loop $1 \
        -i /home/upper/eye_test/$2 -s 64x48 -r 50 -f v4l2 $loopback
}

while true ; do
    play_gif 3 MissBeehaveEyes2black.gif
    play_gif 3 xr_symbol_morph.gif
    play_gif 3 MissBeehaveEyes2black.gif
    play_gif 3 colour.gif
done
