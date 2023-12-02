#!/bin/bash

get_loopback() {
    loopback=`v4l2-ctl --list-devices | \
        grep "v4l2loopback" -A 1 | tail -1 | sed 's/[[:space:]]//g'`
}

play_gif() {
    get_loopback
    ffmpeg -re -default_delay 2 -stream_loop $1 \
        -i /home/upper/eye_test/$2 -s 64x48 -f v4l2 $loopback
}

while true ; do
    play_gif 3 MissBeehaveEyes2.gif
    play_gif 3 colour.gif
done
