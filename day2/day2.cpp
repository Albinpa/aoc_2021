#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "aoc_library.hpp"

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
        const auto splitted = aoc::split(command, " ");
        aoc::assert_equal(splitted.size(), (size_t)2);

        const std::string &cmd = splitted[0];
        const uint64_t num = std::stoull(splitted[1]);

        if (cmd == "up")
        {
            part1.depth -= num;
            part2.aim -= num;
        }
        else if (cmd == "down")
        {
            part1.depth += num;
            part2.aim += num;
        }
        else if (cmd == "forward")
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
