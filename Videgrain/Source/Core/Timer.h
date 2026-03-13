#pragma once
class Timer
{
public:
	Timer();

	float Tick();

	float GetDeltaTime() const { return m_deltaTime; }
	float GetTotalTime() const;

private:
	double m_secondsPerCount;
	long long m_prevTime;
	long long m_startTime;
	float m_deltaTime;
};
