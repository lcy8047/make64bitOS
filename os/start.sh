#!/bin/bash

#$QEMU -L . -m 64 -drive file=$DISK,format=raw -M pc -curses
#qemu-system-x86_64 -m 64 -fda ./Disk.img -M pc -curses

DISK="./Disk.img"
QEMU=qemu-system-x86_64

echo "Selece run mode"

while : ; do
    echo "1 - Run, 2 - Debug, 3 - Build"
    echo "4 - ImageMaker Build, q - exit"
    echo -n "> "
    read INPUT
    case $INPUT in
        1)
            $QEMU -m 64 -fda $DISK -M pc -curses
            ;;
        2)
            $QEMU -m 64 -fda $DISK -M pc -curses -gdb tcp::1234 -S
            ;;
        3)
            make clean; make
            ;;
        4)
            cd ./04.Utility/00.ImageMaker
            make clean; make
            cd ../../
            cp ./04.Utility/00.ImageMaker/ImageMaker .
            ;;
        q)
            break
            ;;
        *)
            echo "Wrong input"
            ;;
    esac
    
done

echo "exit"
