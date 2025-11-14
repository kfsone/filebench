#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

#if __has_include(<unistd.h>)
# include <unistd>h>
#endif

// iostreams are *slow* and we're trying to benchmark,
// so I'm just going to use printf and fopen.
#include <cstdio>

using std::chrono::steady_clock;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;


struct FileList
{
	std::vector<std::string> mFiles;

	const std::string& operator[](size_t index) const noexcept { return mFiles[index]; }
	bool is_valid() const noexcept { return !mFiles.empty(); }

	FileList(size_t count) : mFiles{}
	{
		mFiles.resize(count);
		for (size_t i = 0; i < count; ++i)
		{
			mFiles[i] = "testfile." + std::to_string(i) + ".txt";
			auto fp = fopen(mFiles[i].c_str(), "w");
			if (fp == nullptr)
			{
				fprintf(stderr, "ERROR: Couldn't create %s\n", mFiles[i].c_str());
				throw std::runtime_error("Unable to create files\n");
			}
			fprintf(fp, "-- this file intentionally blank[ish] --\n");
			fclose(fp);
		}
	}

	~FileList() noexcept
	{
		for (const auto& filename : mFiles)
		{
			unlink(filename.c_str());
		}
	}
};


constexpr auto percentile = [](const auto& timings, size_t percentile) -> size_t
{
	const size_t length = timings.size();
	const size_t point = length * percentile;
	// Simple path where the number of samples is an integer multiple of 100.
	if (((point) % 100) == 0)
	{
		return timings[(point / 100)];
	}

	// Otherwise take the average of the values either side of the sample point.
	const auto left = timings[size_t(std::floor(double(point) / 100.))];
	const auto right = timings[size_t(std::ceil(double(point) / 100.))];
	return (left + right) / 2;
};


int main()
{
	constexpr size_t NumIters{ 1000 };
	constexpr size_t NumFiles{ 4096 };
	constexpr size_t TotalOps{ NumIters * NumFiles };

	FileList fileList{ NumFiles };

	std::vector<uint64_t> iterTimings{};
	iterTimings.reserve(NumIters);

	constexpr size_t ReportEvery{ 100 };
	for (size_t iter = 0; iter < NumIters; ++iter)
	{
		if ((iter % ReportEvery) == 0)
		{
			printf("Iter %zu/%zu\n", iter, NumIters);
		}

		const auto start = steady_clock::now();
		for (const auto& filename : fileList.mFiles)
		{
			FILE* fp = fopen(filename.c_str(), "r");
			if (!fp)
			{
				fprintf(stderr, "Unable to open %s\n", filename.c_str());
				exit(1);
			}
			fclose(fp);
		}

		const auto end = steady_clock::now();
		const uint64_t nanos = duration_cast<nanoseconds>(end - start).count();
		iterTimings.push_back(nanos);
	}

	uint64_t total = std::accumulate(iterTimings.cbegin(), iterTimings.cend(), 0ULL);
	uint64_t avg = total / iterTimings.size();

	// Order them so we can print min, max, p90 and p95
	std::sort(iterTimings.begin(), iterTimings.end());
	uint64_t lowest = iterTimings.front();
	uint64_t p90 = percentile(iterTimings, 90);
	uint64_t p95 = percentile(iterTimings, 95);
	uint64_t p99 = percentile(iterTimings, 99);
	uint64_t highest = iterTimings.back();

	const char* unit = "ns";
	uint64_t divisor = 1;
	if (lowest > 10 * 1000 * 1000)
	{
		unit = "ms";
		divisor = 1000 * 1000;
	}
	else if (lowest > 10 * 1000)
	{
		unit = "us";
		divisor = 1000;
	}	

	printf("Iteration Stats (%zu files): min=%zu%s, avg=%zu%s, p90=%zu%s, p95=%zu%s, p99=%zu%s, max=%zu%s\n",
		NumFiles,
		lowest / divisor, unit, avg / divisor, unit, p90 / divisor, unit, p95 / divisor, unit, p99 / divisor, unit, highest / divisor, unit);
	printf("Per File: %zuns\n", (total / TotalOps));
}
