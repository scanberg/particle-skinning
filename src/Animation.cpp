#include "Animation.h"

Transform Animation::getPoseAtTime(size_t channel, float t)
{
	if (channel > m_channel.size())
		return Transform();

	t = glm::clamp(t, 0.0f, m_duration);

	//return m_channel[channel].m_pose[0];
	
	int frameLast = m_channel[channel].getKeyCount()-1;
	int frame0 = (int)((t / m_duration) * frameLast);
	int frame1 = (frame0 + 1) > frameLast ? 0 : (frame0 + 1);

	//if(channel == 0)
	//	printf("time: %.2f, frame0: %i, frame1: %i, lastFrame %i \n", t, frame0, frame1, frameLast);

	float k = t - ((float)frame0 / (float)frameLast) * m_duration;

	return Transform::interpolate(m_channel[channel].m_pose[frame0], m_channel[channel].m_pose[frame1], k);
}