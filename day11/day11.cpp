#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

template <size_t SIZE>
class Grid
{
public:
    Grid(const std::vector<std::string> &input)
    {
        for (int y = 0; const auto &line : input)
        {
            for (int x = 0; const auto c : line)
            {
                grid_[y * SIZE + x++] = c - '0';
            }
            y++;
        }
        original_grid_ = grid_;
    }

    uint64_t part1(uint64_t steps)
    {
        reset();
        uint64_t sum = 0;
        for (uint64_t i = 0; i < steps; i++)
        {
            sum += flash_count();
        }
        return sum;
    }

    uint64_t part2()
    {
        reset();
        uint64_t step = 1;
        while (flash_count() != SIZE * SIZE)
        {
            step++;
        }
        return step;
    }

private:
    void reset()
    {
        grid_ = original_grid_;
    }

    void increase_energy()
    {
        ranges::for_each(grid_, [](auto &v)
                         { v++; });
    }

    void iterate_flashes()
    {
        visit_.clear();
        for (unsigned int i = 0; i < grid_.size(); i++)
        {
            if (grid_[i] > 9)
            {
                visit_.push_back(i);
            }
        }

        const auto not_flashed = [this](const uint64_t v)
        { return !flashed_.contains(v); };

        while (!visit_.empty())
        {
            const auto visiting = visit_;
            visit_.clear();
            for (const auto i : visiting | ranges::views::filter(not_flashed))
            {
                flashed_.insert(i);
                for (const auto sur : surrounding_i(i))
                {
                    if (++grid_[sur] > 9)
                    {
                        visit_.push_back(sur);
                    }
                }
            }
        }

        ranges::for_each(flashed_, [this](auto &i)
                         { grid_[i] = 0; });
    }

    std::vector<uint64_t> surrounding_i(uint64_t i) const
    {
        std::vector<uint64_t> res = {i + SIZE, i - SIZE};
        if ((i + 1) % 10 != 0)
        {
            res.insert(res.end(), {i + 1, i + SIZE + 1, i - SIZE + 1});
        }
        if (i % 10 != 0)
        {
            res.insert(res.end(), {i - 1, i - SIZE - 1, i + SIZE - 1});
        }
        const auto [f, l] = ranges::remove_if(res, [this](const auto ind)
                                              { return ind >= grid_.size(); });
        res.erase(f, l);
        return res;
    }

    uint64_t flash_count()
    {
        flashed_.clear();
        increase_energy();
        iterate_flashes();
        return flashed_.size();
    }

    std::vector<uint64_t> visit_;
    std::unordered_set<uint64_t> flashed_;
    std::array<uint64_t, SIZE * SIZE> grid_;
    std::array<uint64_t, SIZE * SIZE> original_grid_;
};

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const std::vector<std::string> test = {
            "5483143223",
            "2745854711",
            "5264556173",
            "6141336146",
            "6357385478",
            "4167524645",
            "2176841721",
            "6882881134",
            "4846848554",
            "5283751526"};

        Grid<10> test_grid(test);
        aoc::assert_equal(test_grid.part1(100), 1656ul);
        aoc::assert_equal(test_grid.part2(), 195ul);
    }
    Grid<10> grid(aoc::get_lines("input.txt"));
    aoc::assert_equal(grid.part1(100), 1739ul);
    aoc::assert_equal(grid.part2(), 324ul);

    return 0;
}
