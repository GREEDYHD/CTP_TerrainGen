#ifndef _InfoCPU_H_
#define _InfoCPU_H_

#pragma comment(lib, "pdh.lib")

#include <pdh.h>

class InfoCPU
{
public:
	InfoCPU();
	InfoCPU(const InfoCPU&);
	~InfoCPU();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

#endif