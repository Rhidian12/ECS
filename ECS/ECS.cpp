#include <iostream>
#include <numeric>
#include <vector>
#include <string>
#include <fstream>

#if defined __has_include
#	if __has_include (<vld.h>)
#		include <vld.h>
#	endif
#endif

#define BENCHMARKS
// #define UNIT_TESTS

#ifdef BENCHMARKS

int RunBenchmarks();

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

	return RunBenchmarks();

#elif defined UNIT_TESTS

	return RunUnitTests(argc, argv);

#endif
}