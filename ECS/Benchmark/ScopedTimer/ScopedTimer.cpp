#include "ScopedTimer.h"

namespace ECS::Benchmark
{
	ScopedTimer::ScopedTimer(const std::string& fn, const std::string& file)
		: m_Start{ Time::Timer::Now() }
		, m_Function{ fn }
		, m_File{ file }
	{}

	ScopedTimer::~ScopedTimer()
	{
		const double time{ (Time::Timer::Now() - m_Start).Count<Time::TimeLength::MilliSeconds>() };

		if (m_OnDestruction)
			m_OnDestruction(m_Function, m_File, time);
	}
}