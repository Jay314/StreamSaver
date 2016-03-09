# StreamSaver
StreamSaver is a tool to save a network transported video stream (RTP) in local RAM to be able to save via UDP command the last few seconds of this stream on a local hard disk in a video file.

## Installation
There are compiled versions of the three tools (StreamSaver, send_UDP, receive_UDP) in the bin folder for linux x64 architecture.
To compile the project on any other architecture, the libraries of OpenCV (version >= 3.0) are required.

## Configuration
### Requirements
* Libraries of OpenCV (version >= 3.0)
* broadcastet video stream in network (e.g. VLC Player run with example calling script)
* required RAM on machine where StreamSaver running greater than 2 GB

### Config File
Category | Entry | Value | Description
--- | --- | --- | ---
`StreamingSource` |`RTP_IP` | 192.168.1.9 | IP of the interface, where the RTP video stream comes in
|`RTP_PORT` | 5004 | port of the incoming video stream
`StreamSavings` |`duration_sec` | 20 | video buffer in seconds - attention: depending on RAM!
|`show_time` | 1 | overlay the current time in video stream
|`time_font_size` | 0.6 | size of overlaying time
`FileOutput` |`output_folder` | /home/eapps4production/Videos/ | output folder of the video file on local machine; video is coded in H264
|`fps` | 30 | framerate of the incoming and saving video stream
|`file_container` | mkv | file container of the video file
`UDPTrigger` |`listen_IP` | 192.168.1.10 | IP of the interface, where StreamSaver is listening for incoming UDP commands
|`listen_PORT` | 1234 | port for incoming UDP commands
|`answer_IP` | 192.168.1.10 | IP of the remote machine, where the answer should be send to
|`answer_PORT` | 1235 | port of the remote machine
`RunningOptions` |`show_image` | false | for debugging only - shows the current video stream on local machine

## Applications
### StreamSaver
Main application which receive and save the video stream from network on a local harddisk. The config file, including all settings, is passed by command line, e.g. `./streamsaver ../config/settings.ini`.

### UDP Tools
For debugging and testing the UDP functionality, there are two tools:
#### send_UDP
Sends an UDP packet to an IP on specific port, e.g. `./send_UDP 192.168.1.10 1234 test_message`.
#### receive_UDP
Waits for received UDP packets at an IP on specific port, e.g. `./receive_UDP localhost 1234`

## Content
Folder | Content
--- | ---
`bin` | compiled versions for x64 architecture
`config` | config files for calling *streamsaver* and example Node-RED
`docs` | doxygen generated source code documentation
`lib` | folder to save the OpenCV libraries
`scripts` | **scripts for VLC calling under windows and linux**
`scripts/opencv_install` | **download and install scripts for OpenCV**
`src` | **src file of StreamSaver**
`tools` | **UDP tools for sending and receiving UDP packets**