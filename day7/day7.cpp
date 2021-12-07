#include <numeric>
#include "aoc_library.hpp"

inline uint64_t distance(uint64_t from, uint64_t to)
{
    return std::abs((int)from - (int)to);
}

inline uint64_t triangular(uint64_t from, uint64_t to)
{
    const auto dist = distance(from, to);
    return (dist * (dist + 1)) / 2;
}

uint64_t fuel_needed(const std::vector<uint64_t> &crab_pos, bool part2 = false)
{
    uint64_t best_cost = std::numeric_limits<uint64_t>::max();
    const uint64_t largest_pos = *std::max_element(crab_pos.begin(), crab_pos.end());
    for (unsigned int i = 0; i < largest_pos; i++)
    {
        uint64_t cost = std::accumulate(crab_pos.begin(), crab_pos.end(), 0,
                                        [part2, i](uint64_t acc, uint64_t pos)
                                        { return acc + (part2 ? triangular(pos, i) : distance(pos, i)); });
        best_cost = std::min(best_cost, cost);
    }
    return best_cost;
}

int main()
{
    const aoc::StopWatch stop_watch;

    const std::vector<uint64_t> test_input = {16, 1, 2, 0, 4, 2, 7, 1, 2, 14};

    const auto crabs = aoc::transform_int(aoc::split(aoc::get_lines("input.txt")[0], ","));

    aoc::assert_equal(fuel_needed(test_input), 37ul);
    aoc::assert_equal(fuel_needed(crabs), 342730ul);

    aoc::assert_equal(fuel_needed(test_input, true), 168ul);
    aoc::assert_equal(fuel_needed(crabs, true), 92335207ul);

    return 0;
}
