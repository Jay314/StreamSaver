#!/bin/bash

###########################################################
#
# Download used version of OpenCV
# http://opencv.org/
#
###########################################################

version="3.1.0"
mkdir -p OpenCV
echo "Downloading OpenCV" $version
wget -O OpenCV/downloaded_opencv-$version.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/$version/opencv-"$version".zip/download

