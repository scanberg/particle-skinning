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
	Animation(unsigned int channelCount, unsigned int frameCount) :
	m_channelCount(channelCount),
	m_frameCount(frameCount)
	{
		m_channel = (AnimationChannel*) malloc(sizeof(AnimationChannel) * m_channelCount);
		for(unsigned int i=0; i<m_channelCount; ++i)
			m_channel[i] = AnimationChannel(m_frameCount);
	}

	~Animation()
	{
		free(m_channel);
	}

	AnimationChannel * 	getChannel(unsigned int i) { return m_channel+i; }
private:
	AnimationChannel *	m_channel;
	unsigned int		m_channelCount;

	unsigned int 		m_framesPerSecond;
	unsigned int		m_frameCount;
};
