#pragma once

#include "Transform.h"

typedef struct
{
	Transform 		pose;
	unsigned int 	frameNumber;
}sAnimationKeyFrame;

typedef struct
{
	sAnimationKeyFrame *	keyFrame;
	unsigned int 			keyFrameCount;
}sAnimationChannel;

class Animation
{
public:
	Animation();
	~Animation();
private:
	sAnimationChannel * channel;
	unsigned int		channelCount;

	unsigned int 		framesPerSecond;
	unsigned int		frameCount;
};