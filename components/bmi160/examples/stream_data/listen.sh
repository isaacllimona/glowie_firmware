#!/bin/bash

SERVER_IP=espressif  # if mDNS enabled
SERVER_PORT=3333     # default port
OUT_FILE=imu_out.txt
TIMEOUT=20

while getopts "i:p:o:t:" opt; do
    case $opt in
        i) SERVER_IP=$OPTARG;;
        p) SERVER_PORT=$OPTARG;;
        o) OUT_FILE=$OPTARG;;
        t) TIMEOUT=$OPTARG;;
    esac
done

timeout --foreground $TIMEOUT cat < /dev/tcp/$SERVER_IP/$SERVER_PORT > $OUT_FILE
