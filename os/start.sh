#!/bin/bash
DISK="./Disk.img"
QEMU=qemu-system-x86_64
#$QEMU -L . -m 64 -drive file=$DISK,format=raw -M pc -curses
$QEMU -m 64 -fda $DISK -M pc -curses
