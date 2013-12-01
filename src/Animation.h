#pragma once

#include <vector>
#include "Transform.h"

class AnimationChannel
{
	public:
	AnimationChannel(size_t keyCount=0) :
	m_pose(keyCount)
	{}

	~AnimationChannel()
	{}

	const size_t & getKeyCount() { return m_pose.size(); }

	std::vector<Transform> m_pose;
};

class Animation
{
public:
	Animation(size_t channelCount = 0) :
	m_channel(channelCount)
	{}

	~Animation()
	{}

	void addChannel(const AnimationChannel & ac) { m_channel.push_back(ac); }
	const AnimationChannel & getChannel(size_t i) { return m_channel[i]; }

	Transform getPoseAtTime(size_t channel, float t);
private:
	std::vector<AnimationChannel>	m_channel;

	float 				m_duration;
	float 				m_framesPerSecond;
};