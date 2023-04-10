#pragma once

#include "../TimeLength.h"
#include "../../Utils/Utils.h"

namespace ECS::Time
{
	/// <summary>
	/// A Timepoint is similar to std::chrono::time_point and should be viewed as it
	/// Timepoints can be initialised with a time value, received from Time::Timer
	/// They can later then be used to do arithmetics to calculate time
	/// </summary>
	class Timepoint final
	{
	public:
		constexpr Timepoint()
			: m_Time{}
		{}
		constexpr explicit Timepoint(const double time)
			: m_Time{ time }
		{}
		constexpr ~Timepoint() = default;

		constexpr Timepoint(const Timepoint&) noexcept = default;
		constexpr Timepoint(Timepoint&&) noexcept = default;
		constexpr Timepoint& operator=(const Timepoint&) noexcept = default;
		constexpr Timepoint& operator=(Timepoint&&) noexcept = default;

		template<TimeLength T = TimeLength::Seconds>
		constexpr double Count() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return m_Time * SecToNano;
			else if constexpr (T == TimeLength::MicroSeconds)
				return m_Time * SecToMicro;
			else if constexpr (T == TimeLength::MilliSeconds)
				return m_Time * SecToMilli;
			else if constexpr (T == TimeLength::Seconds)
				return m_Time;
			else if constexpr (T == TimeLength::Minutes)
				return m_Time * SecToMin;
			else /* Hours */
				return m_Time * SecToHours;
		}

		template<TimeLength T = TimeLength::Seconds, typename Ret>
		constexpr Ret Count() const
		{
			if constexpr (T == TimeLength::NanoSeconds)
				return static_cast<Ret>(m_Time * SecToNano);
			else if constexpr (T == TimeLength::MicroSeconds)
				return static_cast<Ret>(m_Time * SecToMicro);
			else if constexpr (T == TimeLength::MilliSeconds)
				return static_cast<Ret>(m_Time * SecToMilli);
			else if constexpr (T == TimeLength::Seconds)
				return static_cast<Ret>(m_Time);
			else if constexpr (T == TimeLength::Minutes)
				return static_cast<Ret>(m_Time * SecToMin);
			else /* Hours */
				return static_cast<Ret>(m_Time * SecToHours);
		}

#pragma region Operators
		friend constexpr Timepoint operator-(const Timepoint& a, const Timepoint& b);
		friend constexpr Timepoint operator+(const Timepoint& a, const Timepoint& b);

		friend constexpr Timepoint& operator+=(Timepoint& a, const Timepoint& b);
		friend constexpr Timepoint& operator-=(Timepoint& a, const Timepoint& b);

		friend constexpr bool operator==(const Timepoint& a, const Timepoint& b);
		friend constexpr bool operator!=(const Timepoint& a, const Timepoint& b);

		friend constexpr bool operator<(const Timepoint& a, const Timepoint& b);
		friend constexpr bool operator<=(const Timepoint& a, const Timepoint& b);
		friend constexpr bool operator>(const Timepoint& a, const Timepoint& b);
		friend constexpr bool operator>=(const Timepoint& a, const Timepoint& b);
#pragma endregion

	private:
		double m_Time; /* Stored in seconds */
	};

#pragma region Operators

	constexpr Timepoint operator-(const Timepoint& a, const Timepoint& b)
	{
		return Timepoint{ a.m_Time - b.m_Time };
	}
	constexpr Timepoint operator+(const Timepoint& a, const Timepoint& b)
	{
		return Timepoint{ a.m_Time + b.m_Time };
	}
	constexpr Timepoint& operator+=(Timepoint& a, const Timepoint& b)
	{
		a.m_Time += b.m_Time;

		return a;
	}
	constexpr Timepoint& operator-=(Timepoint& a, const Timepoint& b)
	{
		a.m_Time -= b.m_Time;

		return a;
	}
	constexpr bool operator==(const Timepoint& a, const Timepoint& b)
	{
		return Utils::AreEqual(a.m_Time, b.m_Time);
	}
	constexpr bool operator!=(const Timepoint& a, const Timepoint& b)
	{
		return !(a == b);
	}
	constexpr bool operator<(const Timepoint& a, const Timepoint& b)
	{
		return a.m_Time < b.m_Time;
	}
	constexpr bool operator<=(const Timepoint& a, const Timepoint& b)
	{
		return a.m_Time <= b.m_Time;
	}
	constexpr bool operator>(const Timepoint& a, const Timepoint& b)
	{
		return a.m_Time > b.m_Time;
	}
	constexpr bool operator>=(const Timepoint& a, const Timepoint& b)
	{
		return a.m_Time >= b.m_Time;
	}

#pragma endregion
}