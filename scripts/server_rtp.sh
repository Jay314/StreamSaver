#!/bin/bash
#tut so: vlc -vvv v4l2:// --sout '#duplicate{dst=display,dst={transcode{vcodec=mp4v,acodec=mpga,vb=800,ab=128,deinterlace}:rtp{mux=ts,dst=192.168.179.56,sdp=sap,name="TestStream"}}}'
#working: cvlc v4l2:// --sout '#duplicate{dst={transcode{vcodec=mp4v,acodec=mpga,vb=800,ab=128,deinterlace}:rtp{mux=ts,dst=192.168.179.56,sdp=sap,name="test"}}}'

#working with name: cvlc v4l2:// --sout '#transcode{vcodec=mp2v,vb=800,scale=Automatisch,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap,name="inner_cam"}'

#cvlc v4l2:// --sout '#transcode{vcodec=mp2v,vb=800,scale=Automatisch,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap}'

#not working: cvlc v4l2:// --sout '#transcode{vcodec=h264,vb=800,scale=1,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap,name="test"}'

cvlc v4l2:// --sout '#transcode{vcodec=mp2v,vb=800,scale=Automatisch,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap}'

#not working: cvlc v4l2:// --sout '#transcode{vcodec=h264,vb=800,scale=1,acodec=none}:rtp{dst=192.168.179.56,port=5004,sap,name="te$


