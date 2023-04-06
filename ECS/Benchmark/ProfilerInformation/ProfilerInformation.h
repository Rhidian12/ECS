#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ECS::Benchmark
{
	void OnScopedTimerDestruction(const std::string& fn, const std::string& file, const double time);

	struct ScopedTimerInformation final
	{
		std::string Function;
		std::string File;

		bool operator==(const ScopedTimerInformation& info) const
		{
			return Function == info.Function && File == info.File;
		}
	};

	class ProfilerInformation final
	{
	public:
		__forceinline static ProfilerInformation& GetInstance()
		{
			if (!m_pInstance)
				m_pInstance.reset(new ProfilerInformation{});

			return *m_pInstance.get();
		}

		void AddTime(const std::string& fn, const std::string& file, const double time);

		void WriteTimesToOutput();


	private:
		ProfilerInformation() = default;

		std::vector<std::pair<ScopedTimerInformation, double>> m_Times;

		inline static std::unique_ptr<ProfilerInformation> m_pInstance{};
	};
}