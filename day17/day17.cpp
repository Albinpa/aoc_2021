#include <cmath>
#include <math.h>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct Target
{
    int64_t ymin;
    int64_t ymax;
    int64_t xmin;
    int64_t xmax;
};

inline uint64_t will_it_hit(const Target &target, int64_t vx, int64_t vy)
{
    int64_t step = 0;
    while (true)
    {
        step++;

        const int64_t vxms = std::max(vx - step, 0l);
        const int64_t xpos = (vx * (vx + 1) / 2) - (vxms * (vxms + 1) / 2);

        int64_t acc_ypos = 0;
        int64_t vy_stepper = vy;
        for (int64_t i = 0; i < step; i++)
        {
            acc_ypos += vy_stepper;
            vy_stepper--;
        }

        if (xpos >= target.xmin && xpos <= target.xmax &&
            acc_ypos >= target.ymin && acc_ypos <= target.ymax)
        {
            return step;
        }
        if (xpos > target.xmax || acc_ypos < target.ymin)
        {
            break;
        }
    }
    return 0;
}

std::vector<std::pair<int64_t, int64_t>> generate_velocities(const Target &target)
{
    std::vector<std::pair<int64_t, int64_t>> res;

    const int64_t start_x_it = -0.5 + std::sqrt(2 * target.xmin);
    const int64_t end_x = target.xmax;
    const int64_t start_y = -1 * target.ymin;

    for (int x = start_x_it; x <= end_x; x++)
    {
        for (int y = start_y; y >= target.ymin; y--)
        {
            res.push_back({x, y});
        }
    }

    return res;
}

uint64_t possible_initial_values(const Target &target)
{
    return ranges::count_if(generate_velocities(target), [&target](const auto &p)
                            { return will_it_hit(target, p.first, p.second); });
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        Target target = {-10, -5, 20, 30}; // test data

        aoc::assert_equal(will_it_hit(target, 6, 3), 9ul);
        aoc::assert_equal(will_it_hit(target, 9, 0), 4ul);
        aoc::assert_equal(will_it_hit(target, 20, -6), 1ul);
        aoc::assert_equal(will_it_hit(target, 20, -4), 0ul);

        aoc::assert_equal(possible_initial_values(target), 112ul);
    }

    Target target = {-100, -76, 144, 178}; // puzzle input
    aoc::assert_equal(possible_initial_values(target), 1477ul);

    return 0;
}
