particle-skinning
=================

Skinning and particle simulation using verlet integration

Capturing:
==========
Install [apitrace](https://github.com/apitrace/apitrace)

Generate the trace
	apitrace trace ./Program

Encode the video
	apitrace dump-images -o - Program.trace \
	| ffmpeg -r 60 -f image2pipe -vcodec ppm -i - -vcodec libx264 -preset veryslow  -y Program.mp4
