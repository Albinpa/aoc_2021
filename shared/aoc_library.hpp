#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <ranges>

namespace ranges = std::ranges;

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
        const auto to_int = [](const std::string &s)
        { return std::stoi(s); };
        const auto ints = strings | ranges::views::transform(to_int);
        return std::vector<int>(ints.begin(), ints.end());
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
    inline void print(const std::vector<T> &v)
    {
        std::cout << "[";
        ranges::for_each(v | ranges::views::take(v.size() - 1),
                         [](const auto &i)
                         { std::cout << i << ", "; });
        std::cout << v.back() << "]" << std::endl;
    }

    template <typename T>
    inline void print(const std::vector<std::vector<T>> &v)
    {
        std::cout << "[" << std::endl;
        ranges::for_each(v, [](const auto vi)
                         {
                             std::cout << " ";
                             print(vi);
                         });
        std::cout << "]" << std::endl;
    }

    template <typename T, typename K>
    inline void print(const std::unordered_map<T, K> &map)
    {
        const auto format = [](const auto &pair)
        { std::cout << "{" << pair.first << ", " << pair.second << "}, "; };

        std::cout << "{";
        ranges::for_each(map, format);
        std::cout << "}" << std::endl;
    }
}