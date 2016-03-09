@echo off
"c:\Program Files (x86)\VideoLAN\VLC\vlc.exe" dshow:// :sout=#transcode{vcodec=mp2v,vb=800,scale=Automatisch,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap,name="test"}
