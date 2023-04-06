#pragma once

#include "../../Timer/Timer.h"

#include <functional>
#include <string>

namespace ECS::Benchmark
{
	class ScopedTimer final
	{
		using Callback = std::function<void(const std::string&, const std::string&, const double)>;

	public:
		ScopedTimer(const std::string& fn, const std::string& file);
		~ScopedTimer();

		__forceinline void RegisterDestructionCallback(const Callback& fn) { m_OnDestruction = fn; }

	private:
		Time::Timepoint m_Start;
		std::string m_Function;
		std::string m_File;
		Callback m_OnDestruction;
	};
}