#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>

namespace AocInput
{
	inline std::vector<std::string> get_lines(const std::string &filename)
	{
		std::vector<std::string> res = {};
		std::string line;
		std::ifstream file(filename);
		assert(file.is_open());

		while (getline(file, line))
		{
			res.push_back(line);
		}
		file.close();

		return res;
	}
}

namespace AocVector
{
	template <typename T>
	inline void print_list(const std::vector<T> &vector)
	{
		for (const auto &i : vector)
		{
			std::cout << i << std::endl;
		}
	}

	inline std::vector<int> transform_int(const std::vector<std::string> &strings)
	{
		std::vector<int> ints;
		std::transform(strings.begin(), strings.end(), back_inserter(ints),
					   [](const std::string &s)
					   { return std::stoi(s); });
		return ints;
	}
}

namespace AocAssert
{
	template <typename T>
	inline void assert_equal(const T actual, const T expected)
	{
		if (expected != actual)
		{
			std::cout << "assert_equal failed, actual: ";
			std::cout << actual << " expected: " << expected << std::endl;
			throw;
		}
	}
}