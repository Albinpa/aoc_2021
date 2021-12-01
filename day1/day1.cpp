#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ranges>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

int count_increased(const std::vector<int> &depths)
{
    int increased = 0;
    for (unsigned int i = 1; i < depths.size(); i++)
    {
        if (depths[i - 1] < depths[i])
        {
            increased++;
        }
    }
    return increased;
}

int main()
{
    const auto depths = aoc::transform_int(aoc::get_lines("input.txt"));

    aoc::assert_equal(count_increased(depths), 1266); // part 1

    std::vector<int> three_meas;
    for (unsigned int i = 2; i < depths.size(); i++)
    {
        three_meas.push_back(depths[i] + depths[i - 1] + depths[i - 2]);
    }
    aoc::assert_equal(count_increased(three_meas), 1217); // part 2

    return 0;
}
