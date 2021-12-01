#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <ranges>

namespace aoc
{
    namespace ranges = std::ranges;

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

    inline std::vector<uint64_t> transform_int(const std::vector<std::string> &strings)
    {
        const auto to_int = [](const std::string &s)
        { return std::stoull(s); };
        const auto ints = strings | ranges::views::transform(to_int);
        return std::vector<uint64_t>(ints.begin(), ints.end());
    }

    template <typename T>
    concept os_printable = requires(std::ostream os, T v)
    {
        os << v;
    };

    template <os_printable T>
    inline void assert_equal(const T actual, const T expected)
    {
        if (expected != actual)
        {
            std::stringstream ss;
            ss << "assert_equal failed, actual: " << actual << " expected: " << expected;
            throw std::runtime_error(ss.str());
        }
    }

    template <os_printable... Args>
    inline void printnl(Args &&...args)
    {
        (std::cout << ... << std::forward<Args>(args));
    }

    template <os_printable... Args>
    inline void print(Args &&...args)
    {
        printnl(args...);
        std::cout << std::endl;
    }

    template <os_printable T>
    inline void print(const std::vector<T> &v)
    {
        printnl("[");
        ranges::for_each(v | ranges::views::take(v.size() - 1),
                         [](const auto &i)
                         { printnl(i, ", "); });
        print(v.back(), "]");
    }

    template <os_printable T>
    inline void print(const std::vector<std::vector<T>> &v)
    {
        print("[");
        ranges::for_each(v, [](const auto &vi)
                         {
                             printnl(" ");
                             print(vi);
                         });
        print("]");
    }

    template <os_printable K, os_printable T>
    inline void print(const std::unordered_map<T, K> &map)
    {
        const auto format = [](const auto &pair)
        { printnl("{", pair.first, ", ", pair.second, "}, "); };

        printnl("{");
        ranges::for_each(map, format);
        print("}");
    }
}