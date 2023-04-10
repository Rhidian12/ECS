#pragma once

#include <functional>
#include <vector>

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