#include "ProfilerInformation.h"

#include <algorithm>
#include <iostream>

namespace ECS::Benchmark
{
	void OnScopedTimerDestruction(const std::string& fn, const std::string& file, const double time)
	{
		ProfilerInformation::GetInstance().AddTime(fn, file, time);
	}

	void ProfilerInformation::AddTime(const std::string& fn, const std::string& file, const double time)
	{
		const ScopedTimerInformation info{ fn, file };

		auto it = std::find_if(m_Times.begin(), m_Times.end(), [&info](const auto& pair)->bool
			{
				return info == pair.first;
			});

		if (it == m_Times.cend())
		{
			m_Times.push_back(std::make_pair(info, time));
		}
		else
		{
			it->second += time;
		}
	}

	void ProfilerInformation::WriteTimesToOutput()
	{
		std::sort(m_Times.begin(), m_Times.end(), [](const auto& a, const auto& b)->bool
			{
				return a.second > b.second;
			});

		for (const auto& [info, time] : m_Times)
		{
			std::cout << "====================\n\n";

			std::cout << "File: " << info.File << ", Function: " << info.Function << "\n";
			std::cout << "Time spent in function: " << time << "\n\n";
		}
	}
}