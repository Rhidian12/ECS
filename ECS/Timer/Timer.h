#pragma once

#include "TimeLength.h"
#include "Timepoint/Timepoint.h"

#include <memory> /* std::unique_ptr */

namespace ECS::Time
{
	class Timer final
	{
	public:
		~Timer() = default;

		static Timer& GetInstance();

		Timer(const Timer&) noexcept = delete;
		Timer(Timer&&) noexcept = delete;
		Timer& operator=(const Timer&) noexcept = delete;
		Timer& operator=(Timer&&) noexcept = delete;

		void Start();
		void Update();

		static Timepoint Now();
		double GetElapsedSeconds() const { return m_ElapsedSeconds; }
		double GetFixedElapsedSeconds() const { return m_TimePerFrame; }
		double GetTotalElapsedSeconds() const { return m_TotalElapsedSeconds; }
		int GetFPS() const { return m_FPS; }
		double GetTimePerFrame() const { return m_TimePerFrame; }

#pragma region GetElapsedTime
		template<TimeLength T>
		double GetElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return m_ElapsedSeconds * SecToNano;
			else if constexpr (T == TimeLength::MicroSeconds)
				return m_ElapsedSeconds * SecToMicro;
			else if constexpr (T == TimeLength::MilliSeconds)
				return m_ElapsedSeconds * SecToMilli;
			else if constexpr (T == TimeLength::Seconds)
				return m_ElapsedSeconds;
			else if constexpr (T == TimeLength::Minutes)
				return m_ElapsedSeconds * SecToMin;
			else /* Hours */
				return m_ElapsedSeconds * SecToHours;
		}
		template<TimeLength T, typename Ret>
		Ret GetElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return static_cast<Ret>(m_ElapsedSeconds * SecToNano);
			else if constexpr (T == TimeLength::MicroSeconds)
				return static_cast<Ret>(m_ElapsedSeconds * SecToMicro);
			else if constexpr (T == TimeLength::MilliSeconds)
				return static_cast<Ret>(m_ElapsedSeconds * SecToMilli);
			else if constexpr (T == TimeLength::Seconds)
				return static_cast<Ret>(m_ElapsedSeconds);
			else if constexpr (T == TimeLength::Minutes)
				return static_cast<Ret>(m_ElapsedSeconds * SecToMin);
			else /* Hours */
				return static_cast<Ret>(m_ElapsedSeconds * SecToHours);
		}
#pragma endregion

#pragma region GetFixedElapsedTime
		template<TimeLength T>
		double GetFixedElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return m_TimePerFrame * SecToNano;
			else if constexpr (T == TimeLength::MicroSeconds)
				return m_TimePerFrame * SecToMicro;
			else if constexpr (T == TimeLength::MilliSeconds)
				return m_TimePerFrame * SecToMilli;
			else if constexpr (T == TimeLength::Seconds)
				return m_TimePerFrame;
			else if constexpr (T == TimeLength::Minutes)
				return m_TimePerFrame * SecToMin;
			else /* Hours */
				return m_TimePerFrame * SecToHours;
		}
		template<TimeLength T, typename Ret>
		Ret GetFixedElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return static_cast<Ret>(m_TimePerFrame * SecToNano);
			else if constexpr (T == TimeLength::MicroSeconds)
				return static_cast<Ret>(m_TimePerFrame * SecToMicro);
			else if constexpr (T == TimeLength::MilliSeconds)
				return static_cast<Ret>(m_TimePerFrame * SecToMilli);
			else if constexpr (T == TimeLength::Seconds)
				return static_cast<Ret>(m_TimePerFrame);
			else if constexpr (T == TimeLength::Minutes)
				return static_cast<Ret>(m_TimePerFrame * SecToMin);
			else /* Hours */
				return static_cast<Ret>(m_TimePerFrame * SecToHours);
		}
#pragma endregion

#pragma region GetTotalElapsedTime
		template<TimeLength T>
		double GetTotalElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return m_TotalElapsedSeconds * SecToNano;
			else if constexpr (T == TimeLength::MicroSeconds)
				return m_TotalElapsedSeconds * SecToMicro;
			else if constexpr (T == TimeLength::MilliSeconds)
				return m_TotalElapsedSeconds * SecToMilli;
			else if constexpr (T == TimeLength::Seconds)
				return m_TotalElapsedSeconds;
			else if constexpr (T == TimeLength::Minutes)
				return m_TotalElapsedSeconds * SecToMin;
			else /* Hours */
				return m_TotalElapsedSeconds * SecToHours;
		}
		template<TimeLength T, typename Ret>
		Ret GetTotalElapsedTime() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return static_cast<Ret>(m_TotalElapsedSeconds * SecToNano);
			else if constexpr (T == TimeLength::MicroSeconds)
				return static_cast<Ret>(m_TotalElapsedSeconds * SecToMicro);
			else if constexpr (T == TimeLength::MilliSeconds)
				return static_cast<Ret>(m_TotalElapsedSeconds * SecToMilli);
			else if constexpr (T == TimeLength::Seconds)
				return static_cast<Ret>(m_TotalElapsedSeconds);
			else if constexpr (T == TimeLength::Minutes)
				return static_cast<Ret>(m_TotalElapsedSeconds * SecToMin);
			else /* Hours */
				return static_cast<Ret>(m_TotalElapsedSeconds * SecToHours);
		}
#pragma endregion

	private:
		Timer();

		inline static std::unique_ptr<Timer> m_pInstance{};

		const double m_MaxElapsedSeconds;
		double m_ElapsedSeconds;
		double m_TotalElapsedSeconds;
		int m_FPS;
		int m_FPSCounter;
		double m_FPSTimer;
		double m_TimePerFrame;

		Timepoint m_StartTimepoint;
		Timepoint m_PreviousTimepoint;
	};
}