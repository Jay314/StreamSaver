#!/bin/bash

###########################################################
#
# Download latest version of OpenCV
# http://opencv.org/
#
###########################################################

version="$(wget -q -O - http://sourceforge.net/projects/opencvlibrary/files/opencv-unix | egrep -m1 -o '\"[0-9](\.[0-9])+' | cut -c2-)"
mkdir -p OpenCV
echo "Downloading OpenCV" $version
wget -O OpenCV/downloaded_opencv-$version.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/$version/opencv-"$version".zip/download

