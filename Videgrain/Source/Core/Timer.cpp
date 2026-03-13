#include "Timer.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

Timer::Timer() : m_prevTime(0), m_deltaTime(0.0f)
{
	long long countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

	m_secondsPerCount = 1.0f / (double)countsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTime);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	m_prevTime = m_startTime;
}

float Timer::Tick()
{
	long long currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_deltaTime = (float)((currentTime - m_prevTime) * m_secondsPerCount);
	m_prevTime = currentTime;

	if(m_deltaTime < 0.0f)
	{
		m_deltaTime = 0.0f;
	}

	return m_deltaTime;
}

float Timer::GetTotalTime() const
{
	long long currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	return (float)((currentTime - m_startTime) * m_secondsPerCount);
}
