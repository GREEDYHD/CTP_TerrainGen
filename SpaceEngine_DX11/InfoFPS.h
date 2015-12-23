#ifndef _InfoFPS_H_
#define _InfoFPS_H_

#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>

class InfoFPS
{
public:
	InfoFPS();
	InfoFPS(const InfoFPS&);
	~InfoFPS();

	void Initialize();
	void Frame();
	int GetFPS();

private:
	int m_FPS, m_count;
	unsigned long m_startTime;
};

#endif