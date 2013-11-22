#pragma once

#include "Transform.h"

class AnimationChannel
{
	public:
	AnimationChannel(unsigned int frameCount)
	{
		m_frameCount = frameCount;
		m_pose = new Transform[frameCount];
	}
	~AnimationChannel()
	{
		delete[] m_pose;
	}

	Transform * 	m_pose;
	unsigned int 	m_frameCount;
};

class Animation
{
public:
	Animation();
	~Animation();
private:
	AnimationChannel *	m_channel;
	unsigned int		m_channelCount;

	unsigned int 		m_framesPerSecond;
	unsigned int		m_frameCount;
};