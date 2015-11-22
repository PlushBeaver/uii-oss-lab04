#pragma once

#include <random>

class RandomGenerator
{
public:
	RandomGenerator(size_t from, size_t to)
		: algorithm_{entropy_source_()}
		, generator_{from, to}
	{
	}

	size_t next()
	{
		return generator_(algorithm_);
	}

private:
	std::default_random_engine algorithm_;
    std::uniform_int_distribution<size_t> generator_;

private:
	static std::random_device entropy_source_;
};

std::random_device RandomGenerator::entropy_source_;
