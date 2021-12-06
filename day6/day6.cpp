#include <numeric>
#include "aoc_library.hpp"

uint64_t spawns(const std::vector<uint64_t> &pre_calcs, uint64_t timer, uint64_t days)
{
    uint64_t next_spawn = timer + 1;
    if (days < next_spawn)
        return 0;

    days -= next_spawn;
    return 1 + spawns(pre_calcs, 6, days) + pre_calcs[days];
}

std::vector<uint64_t> pre_calc(uint64_t days)
{
    std::vector<uint64_t> pre_calcs(days, 0);
    for (int days_left = 0; auto &pre_calc : pre_calcs)
    {
        pre_calc = spawns(pre_calcs, 8, days_left++);
    }
    return pre_calcs;
}

uint64_t solve(const std::vector<uint64_t> &nearby, uint64_t days)
{
    const auto pre_calcs = pre_calc(days);
    return std::accumulate(nearby.begin(), nearby.end(), nearby.size(),
                           [=, &pre_calcs](uint64_t acc, uint64_t f)
                           { return acc + spawns(pre_calcs, f, days); });
}

int main()
{
    aoc::StopWatch s;

    const std::vector<uint64_t> test_input = {3, 4, 3, 1, 2};
    aoc::assert_equal(solve(test_input, 80), 5934ul);

    const auto input = aoc::transform_int(aoc::split(aoc::get_lines("input.txt")[0], ","));
    aoc::assert_equal(solve(input, 80), 373378ul);
    aoc::assert_equal(solve(input, 256), 1682576647495ul);

    return 0;
}
