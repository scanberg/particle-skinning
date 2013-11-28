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
	Animation(unsigned int channelCount) :
	m_channelCount(channelCount)
	{
		m_channel = (AnimationChannel*) malloc(sizeof(AnimationChannel) * m_channelCount);
	}

	~Animation()
	{
		if(m_channel)
			free(m_channel);
	}

	void setChannel(size_t i, const AnimationChannel & ac) { assert(i < m_channelCount); m_channel[i] = ac; }
	AnimationChannel * 	getChannel(size_t i) { return m_channel+i; }
private:
	AnimationChannel *	m_channel;
	unsigned int		m_channelCount;

	float 				m_duration;
	float 				m_framesPerSecond;
};
