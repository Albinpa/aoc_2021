#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace aoc
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

	inline std::vector<int> transform_int(const std::vector<std::string> &strings)
	{
		std::vector<int> ints;
		std::transform(strings.begin(), strings.end(), back_inserter(ints),
					   [](const std::string &s)
					   { return std::stoi(s); });
		return ints;
	}

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

	template <typename T>
	inline void print(const std::vector<T> &vector)
	{
		std::stringstream ss;
		ss << "[";
		for (const auto &i : vector) {
				ss << i;
				if (&i != &vector.back()) {
					ss << ",";
				}
		}
		std::cout << ss.str() << "]" << std::endl;
	}

	template <typename T, typename K>
	inline void print(const std::unordered_map<T,K> &map)
	{
		std::stringstream ss;
		ss << "{";
		for (const auto &i : map) {
			ss << "{" << i.first << "," << i.second << "},";
		}
		ss.seekp(-1, ss.cur);
		ss << "}";
		std::cout << ss.str() << std::endl;
	}
}