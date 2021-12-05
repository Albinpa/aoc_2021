#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ranges>
#include <cassert>
#include <regex>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::pair<uint64_t, uint64_t> Point;

struct Line
{
    Line(const std::string &line)
    {
        const auto [x1, y1, x2, y2] = aoc::search<4>(line, "(\\d+),(\\d+) -> (\\d+),(\\d+)");
        p1_ = {std::stoull(x1), std::stoull(y1)};
        p2_ = {std::stoull(x2), std::stoull(y2)};
    }

    std::vector<Point> covers() const
    {
        std::vector<Point> res;
        if (diagonal())
        {
            Point smallest_y = p1_.second < p2_.second ? p1_ : p2_;
            const Point largest_y = smallest_y == p1_ ? p2_ : p1_;
            const uint64_t dir = smallest_y.first < largest_y.first ? 1 : -1;

            Point &i = smallest_y;
            while (i.second <= largest_y.second)
            {
                res.push_back(i);
                i.second++;
                i.first += dir;
            }
        }
        else
        {
            const Point start = {std::min(p1_.first, p2_.first),
                                 std::min(p1_.second, p2_.second)};
            const Point end = {std::max(p1_.first, p2_.first),
                               std::max(p1_.second, p2_.second)};

            for (uint64_t y = start.second; y <= end.second; y++)
            {
                for (uint64_t x = start.first; x <= end.first; x++)
                {
                    res.push_back({x, y});
                }
            }
        }
        return res;
    }

    bool diagonal() const
    {
        return !(p1_.first == p2_.first ||
                 p1_.second == p2_.second);
    }

    Point p1_;
    Point p2_;
};

struct OceanFloor
{
    OceanFloor(const Point &size)
    {
        m_.resize(size.second, std::vector<uint64_t>(size.second, 0));
    }

    void mark_line(const Line &line, bool diag)
    {
        if (!diag && line.diagonal())
            return;

        ranges::for_each(line.covers(), [&, this](const auto &p)
                         { m_[p.second][p.first]++; });
    }

    uint64_t count() const
    {
        uint64_t count = 0;
        for (const auto &row : m_)
        {
            count += ranges::count_if(row, [](uint64_t v)
                                      { return v >= 2; });
        }
        return count;
    }
    std::vector<std::vector<uint64_t>> m_;
};

uint64_t solve(const std::vector<std::string> lines, bool diag = true)
{

    std::vector<Line> structured_lines;
    Point max = {0, 0};

    for (const auto &line : lines)
    {
        const Line l(line);
        structured_lines.push_back(l);
        max = {std::max({l.p1_.first, l.p2_.first, max.first}),
               std::max({l.p1_.second, l.p2_.second, max.second})};
    }

    OceanFloor ocean({max.first + 1, max.second + 1});

    ranges::for_each(structured_lines, [&](const auto &l)
                     { ocean.mark_line(l, diag); });

    return ocean.count();
}

int main()
{
    const std::vector<std::string> test_input = {
        "0,9 -> 5,9",
        "8,0 -> 0,8",
        "9,4 -> 3,4",
        "2,2 -> 2,1",
        "7,0 -> 7,4",
        "6,4 -> 2,0",
        "0,9 -> 2,9",
        "3,4 -> 1,4",
        "0,0 -> 8,8",
        "5,5 -> 8,2"};

    const auto puzzle_input = aoc::get_lines("input.txt");

    // part 1
    aoc::assert_equal(solve(test_input, false), 5ul);
    aoc::assert_equal(solve(puzzle_input, false), 7414ul);

    // part 2
    aoc::assert_equal(solve(test_input), 12ul);
    aoc::assert_equal(solve(puzzle_input), 19676ul);

    return 0;
}
