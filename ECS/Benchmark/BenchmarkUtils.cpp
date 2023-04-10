#include "BenchmarkUtils.h"
#include "../Timer/Timer.h"

#include <algorithm>

namespace ECS::Benchmark
{
	static void TrimVector(const int nrOfIterations, std::vector<double>& times)
	{
		std::sort(times.begin(), times.end());

		// Remove top 10% bottom and top elements
		const int nrOfElementsToRemove{ static_cast<int>(nrOfIterations * 0.1f) };

		if (times.size() > nrOfElementsToRemove)
			times.erase(times.begin(), times.begin() + nrOfElementsToRemove);

		if (times.size() > nrOfElementsToRemove)
			times.erase(times.begin() + times.size() - nrOfElementsToRemove, times.end());
	}

	std::vector<double> BenchmarkUtils::BenchmarkFunction(const int nrOfIterations, const std::function<void()>& fn)
	{
		using namespace Time;

		std::vector<double> times{};
		times.reserve(nrOfIterations);

		for (int i{}; i < nrOfIterations; ++i)
		{
			if (m_OnFunctionStart)
				m_OnFunctionStart();

			const Timepoint t1{ Timer::Now() };

			fn();

			const Timepoint t2{ Timer::Now() };

			times.push_back((t2 - t1).Count<TimeLength::MilliSeconds>());
		}

		TrimVector(nrOfIterations, times);

		return times;
	}
}