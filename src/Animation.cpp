#include "Animation.h"

Transform Animation::getPoseAtTime(size_t channel, float t)
{
	if (channel > m_channel.size())
		return Transform();

	t = glm::clamp(t, 0.0f, m_duration);

	//return m_channel[channel].m_pose[0];
	
	int frameCount = m_channel[channel].getKeyCount() - 1;
	int frame0 = (int)((t / m_duration) * frameCount);
	int frame1 = (frame0 + 1) >= frameCount ? 0 : (frame0 + 1);

	float t0 = ((float)frame0 / (float)frameCount) * m_duration;
	float dt = 1.0f / m_framesPerSecond;

	float k = (t - t0) / dt;

	//if(channel == 0)
	//	printf("time: %.2f, frame0: %i, frame1: %i, frameCount %i, k %.2f \n", t, frame0, frame1, frameCount, k);

	return Transform::interpolate(m_channel[channel].m_pose[frame0], m_channel[channel].m_pose[frame1], k);
}