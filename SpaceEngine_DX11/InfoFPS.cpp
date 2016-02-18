#include "InfoFPS.h"

InfoFPS::InfoFPS()
{

}

InfoFPS::InfoFPS(const InfoFPS& other)
{

}

InfoFPS::~InfoFPS()
{

}

void InfoFPS::Initialize()
{
	m_FPS = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}

void InfoFPS::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_FPS = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

int* InfoFPS::GetFPS()
{
	return &m_FPS;
}