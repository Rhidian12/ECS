#if defined __has_include
#	if __has_include (<vld.h>)
#		include <vld.h>
#	endif
#endif

// #define BENCHMARKS
#define UNIT_TESTS

#ifdef BENCHMARKS

int RunBenchmarks(const int iterations, const size_t amountOfEntities);

#elif defined UNIT_TESTS

int RunUnitTests(int argc, char* argv[]);

#endif


#ifdef BENCHMARKS

int main(int, char*[])

#elif defined UNIT_TESTS

int main(int argc, char* argv[])

#endif

{
#ifdef BENCHMARKS

	constexpr int iterations{ 100 };
	constexpr size_t nrOfEntities{ 1'000'000 };

	return RunBenchmarks(iterations, nrOfEntities);

#elif defined UNIT_TESTS

	return RunUnitTests(argc, argv);

#endif
}