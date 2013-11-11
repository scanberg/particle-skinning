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
	sAnimationChannel * m_channel;
	unsigned int		m_channelCount;

	unsigned int 		m_framesPerSecond;
	unsigned int		m_frameCount;
};