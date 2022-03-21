#!/bin/sh

module="Drvo_practica"
major=`awk "\\$2==\"$module\" {print \\$1}" /proc/devices`

echo $major

sudo mknod /dev/$module c $major 0 -m 0666