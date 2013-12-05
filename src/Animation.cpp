#include "Animation.h"

Transform Animation::getPoseAtTime(size_t channel, float t)
{
	if (channel > m_channel.size())
		return Transform();
	t = glm::clamp(t, 0.0f, m_duration);
	
	size_t frameLast = m_channel[channel].getKeyCount()-1;
	size_t frame0 = (size_t)(t / m_duration) * frameLast;
	size_t frame1 = (frame0 + 1) > frameLast ? 0 : (frame0 + 1);

	float k = t - ((float)frame0 / (float)frameLast) * m_duration;

	return Transform::interpolate(m_channel[channel].m_pose[frame0], m_channel[channel].m_pose[frame1], k);
}