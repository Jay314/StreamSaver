@echo off
D:\VideoLAN\VLC\vlc.exe -I dummy --dummy-quiet rtsp://192.168.0.90:554/mpeg4/1/media.amp :sout=#transcode{vcodec=mp2v,vb=800,fps=60,scale=Automatisch,acodec=none}:rtp{dst=192.168.144.255,port=5004,mux=ts} :sout-keep
