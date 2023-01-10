#include "Timer.h"

#pragma warning ( push )
#pragma warning ( disable : 4005 ) /* warning C4005: 'APIENTRY': macro redefinition */ 
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#pragma warning ( pop )

#ifdef min
#undef min
#endif

namespace ECS::Time
{
	Timer::Timer()
		: m_MaxElapsedSeconds{ 0.1 }
		, m_ElapsedSeconds{}
		, m_TotalElapsedSeconds{}
		, m_FPS{}
		, m_FPSCounter{}
		, m_FPSTimer{}
		, m_TimePerFrame{ 1.0 / 60.0 }
		, m_StartTimepoint{}
		, m_PreviousTimepoint{}
	{
		Start();
	}

	Timer& Timer::GetInstance()
	{
		if (!m_pInstance)
			m_pInstance.reset(new Timer{});

		return *m_pInstance.get();
	}


	void Timer::Start()
	{
		m_PreviousTimepoint = Now();
	}

	void Timer::Update()
	{
		m_StartTimepoint = Now();

		m_ElapsedSeconds = (m_StartTimepoint - m_PreviousTimepoint).Count();
		m_ElapsedSeconds = std::min(m_ElapsedSeconds, m_MaxElapsedSeconds);

		m_TotalElapsedSeconds += m_ElapsedSeconds;

		m_PreviousTimepoint = m_StartTimepoint;

		m_FPS = static_cast<int>(1.0 / m_ElapsedSeconds);
	}

	Timepoint Timer::Now()
	{
		const int64_t frequency{ _Query_perf_frequency() };
		const int64_t counter{ _Query_perf_counter() };

		// 10 MHz is a very common QPC frequency on modern PCs. Optimizing for
		// this specific frequency can double the performance of this function by
		// avoiding the expensive frequency conversion path.
		constexpr int64_t tenMHz = 10'000'000;

		if (frequency == tenMHz)
		{
			constexpr int64_t multiplier{ static_cast<int64_t>(SecToNano) / tenMHz };
			return Timepoint{ (counter * multiplier) * NanoToSec };
		}
		else
		{
			// Instead of just having "(counter * static_cast<int64_t>(SecToNano)) / frequency",
			// the algorithm below prevents overflow when counter is sufficiently large.
			// It assumes that frequency * static_cast<int64_t>(SecToNano) does not overflow, which is currently true for nano period.
			// It is not realistic for counter to accumulate to large values from zero with this assumption,
			// but the initial value of counter could be large.

			const int64_t whole = (counter / frequency) * static_cast<int64_t>(SecToNano);
			const int64_t part = (counter % frequency) * static_cast<int64_t>(SecToNano) / frequency;
			return Timepoint{ (whole + part) * NanoToSec };
		}
	}

}