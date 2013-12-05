#pragma once

#include <vector>
#include "Transform.h"

class AnimationChannel
{
	public:
	AnimationChannel(size_t keyCount=0)
	{
		m_pose.reserve(keyCount);
	}

	~AnimationChannel()
	{}

	size_t getKeyCount() { return m_pose.size(); }

	std::vector<Transform> m_pose;
};

class Animation
{
public:
	Animation(size_t channelCount, float duration, float fps) :
	m_duration(duration),
	m_framesPerSecond(fps)
	{
		m_channel.reserve(channelCount);
	}

	~Animation()
	{}

	void addChannel(const AnimationChannel & ac) { m_channel.push_back(ac); }
	const AnimationChannel & getChannel(size_t i) { return m_channel[i]; }

	Transform getPoseAtTime(size_t channel, float t);

	float getDuration() { return m_duration; }
	float getFramesPerSecond() { return m_framesPerSecond; }

private:
	std::vector<AnimationChannel>	m_channel;

	float 				m_duration;
	float 				m_framesPerSecond;
};