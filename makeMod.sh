#!/bin/sh

make clean

make drvo_practica.ko
sudo insmod drvo_practica.ko

sudo mknod /dev/drvo_practica c 500 10 -m 0666

cat /dev/drvo_practica
echo "ABCDR" > /dev/drvo_practica

sudo rmmod drvo_practica