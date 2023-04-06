#pragma once

#include "../Timer/Timer.h"
#include "ProfilerInformation/ProfilerInformation.h"
#include "ScopedTimer/ScopedTimer.h"

#include <functional>
#include <vector>

// #define ENABLE_PROFILING

#ifdef ENABLE_PROFILING

#define PROFILE_SCOPE_IMPL(function, file)	ECS::Benchmark::ScopedTimer scopedTimer{ function, file }; \
											scopedTimer.RegisterDestructionCallback(&ECS::Benchmark::OnScopedTimerDestruction);

#define PROFILE_SCOPE() PROFILE_SCOPE_IMPL(__FUNCTION__, __FILE__)

#else

#define PROFILE_SCOPE() 

#endif

namespace ECS::Benchmark
{
	class BenchmarkUtils final
	{
	public:
		std::vector<double> BenchmarkFunction(const int nrOfIterations, const std::function<void()>& fn);

		void SetOnFunctionStart(const std::function<void()>& fn) { m_OnFunctionStart = fn; }

	private:
		std::function<void()> m_OnFunctionStart;
	};
}