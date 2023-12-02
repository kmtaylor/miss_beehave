#!/bin/bash

get_loopback() {
    loopback=`v4l2-ctl --list-devices | \
        grep "v4l2loopback" -A 1 | tail -1 | sed 's/[[:space:]]//g'`
}

sudo insmod /lib/modules/6.1.21-v7+/extra/v4l2loopback.ko.xz
get_loopback
sudo /home/upper/v4l2loopback/utils/v4l2loopback-ctl set-fps $loopback 50

get_led_raster() {
    led_raster=`v4l2-ctl --list-devices | grep "LED Raster" -A 1`
    status=$?
    led_raster=`echo $led_raster | sed 's/.*\/dev/\/dev/g'`
    return $status
}

check_gst-launch() {
    if [ "`pidof gst-launch-1.0`" = "" ] ; then
        get_led_raster || return -1
        gst-launch-1.0 -v \
            v4l2src device=$loopback ! \
            videoconvert ! \
            v4l2sink device=$led_raster &
        return -1
    fi
    return 0;
}

/home/upper/eye_test/run_ffmpeg.sh &

while true ; do
    check_gst-launch
    sleep 1
done
