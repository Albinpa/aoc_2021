#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "aoc_library.hpp"

std::pair<std::string, std::string> split(const std::string &input, const std::string &delim)
{
    const auto i = input.find(delim);
    if (i == std::string::npos)
    {
        throw;
    }

    return std::make_pair(input.substr(0, i), input.substr(i, input.length()));
}

struct Position
{
    uint64_t horizontal = 0;
    uint64_t depth = 0;
    uint64_t aim = 0;

    uint64_t final_pos() const
    {
        return horizontal * depth;
    }
};

int main()
{
    const auto lines = aoc::get_lines("input.txt");

    Position part1;
    Position part2;

    for (const auto &command : lines)
    {
        const auto [first, second] = split(command, " ");
        const uint64_t num = std::stoull(second);
        if (first == "up")
        {
            part1.depth -= num;
            part2.aim -= num;
        }
        else if (first == "down")
        {
            part1.depth += num;
            part2.aim += num;
        }
        else if (first == "forward")
        {
            part1.horizontal += num;
            part2.horizontal += num;
            part2.depth += num * part2.aim;
        }
    }

    aoc::assert_equal(part1.final_pos(), uint64_t(1746616));

    aoc::assert_equal(part2.final_pos(), uint64_t(1741971043));

    return 0;
}
