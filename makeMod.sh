#!/bin/sh

make drvo_practica.ko
sudo insmod drvo_practica.ko

sudo mknod /dev/drvo_practica c 500 10 -m 0666