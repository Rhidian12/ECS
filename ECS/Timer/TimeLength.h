#pragma once

namespace ECS::Time
{
	enum class TimeLength
	{
		NanoSeconds,
		MicroSeconds,
		MilliSeconds,
		Seconds,
		Minutes,
		Hours,
	};

	inline constexpr double SecToNano{ 1'000'000'000.0 };
	inline constexpr double SecToMicro{ 1'000'000.0 };
	inline constexpr double SecToMilli{ 1'000.0 };
	inline constexpr double SecToMin{ 1.0 / 60.0 };
	inline constexpr double SecToHours{ 1.0 / 3600.0 };

	inline constexpr double NanoToSec{ 1.0 / 1'000'000'000.0 };
	inline constexpr double MicroToSec{ 1.0 / 1'000'000.0 };
	inline constexpr double MilliToSec{ 1.0 / 1'000.0 };
	inline constexpr double MinToSec{ 60.0 };
	inline constexpr double HoursToSec{ 3600.0 };
}