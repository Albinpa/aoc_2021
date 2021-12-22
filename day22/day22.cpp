#include <numeric>
#include <algorithm>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct Cube
{
    int64_t x;
    int64_t y;
    int64_t z;
};

inline Cube get_closest_point(const std::pair<Cube, Cube> &line, const Cube &cube)
{
    return {
        .x=std::min(std::max(cube.x, line.first.x), line.second.x),
        .y=std::min(std::max(cube.y, line.first.y), line.second.y),
        .z=std::min(std::max(cube.z, line.first.z), line.second.z)
    };
}

struct Cubeoid
{
    int64_t x_min;
    int64_t x_max;
    int64_t y_min;
    int64_t y_max;
    int64_t z_min;
    int64_t z_max;

    bool is_cube_inside(const Cube &cube) const
    {
        return cube.x <= x_max && cube.x >= x_min &&
               cube.y <= y_max && cube.y >= y_min &&
               cube.z <= z_max && cube.z >= z_min;
    }

    bool is_inside_cubeoid(const Cubeoid &c) const
    {
        return ranges::all_of(corners(), [&c, this](const auto &corner)
                              { return c.is_cube_inside(corner); });
    }

    std::vector<Cube> corners() const
    {
        return {
            {x_min, y_min, z_min},
            {x_min, y_max, z_min},
            {x_min, y_min, z_max},
            {x_min, y_max, z_max},
            {x_max, y_min, z_min},
            {x_max, y_max, z_min},
            {x_max, y_min, z_max},
            {x_max, y_max, z_max},
        };
    }

    std::vector<std::pair<Cube, Cube>> outer_lines() const
    {
        return {
            {{x_min, y_min, z_min}, {x_max, y_min, z_min}},
            {{x_min, y_max, z_max}, {x_max, y_max, z_max}},
            {{x_min, y_min, z_max}, {x_max, y_min, z_max}},
            {{x_min, y_max, z_min}, {x_max, y_max, z_min}},

            {{x_min, y_min, z_min}, {x_min, y_max, z_min}},
            {{x_max, y_min, z_max}, {x_max, y_max, z_max}},
            {{x_max, y_min, z_min}, {x_max, y_max, z_min}},
            {{x_min, y_min, z_max}, {x_min, y_max, z_max}},

            {{x_min, y_min, z_min}, {x_min, y_min, z_max}},
            {{x_max, y_max, z_min}, {x_max, y_max, z_max}},
            {{x_max, y_min, z_min}, {x_max, y_min, z_max}},
            {{x_min, y_max, z_min}, {x_min, y_max, z_max}},
        };
    }

    Cube middle() const
    {
        return {(x_max - x_min) / 2 + x_min,
                (y_max - y_min) / 2 + y_min,
                (z_max - z_min) / 2 + z_min};
    }

    bool intersected_by(const std::vector<std::pair<Cube, Cube>> &lines) const
    {
        for (const auto &line : lines)
        {
            Cube closest = get_closest_point(line, middle());
            if (is_cube_inside(closest))
            {
                return true;
            }
        }
        return false;
    }

    bool overlaps(const Cubeoid &c) const
    {
        return intersected_by(c.outer_lines()) || c.intersected_by(outer_lines());
    }

    uint64_t count() const
    {
        return (x_max - x_min + 1) * (y_max - y_min + 1) * (z_max - z_min + 1);
    }
};

inline std::vector<Cubeoid> split(const Cubeoid &c1, const Cubeoid &c2)
{
    std::vector<Cubeoid> res;

    Cubeoid to_split = c1;
    while (true)
    {
        if (to_split.is_inside_cubeoid(c2))
        {
            break;
        }

        Cubeoid copy_non_overlap = to_split;
        Cubeoid copy_to_split = to_split;

        if (c2.x_min <= to_split.x_max && c2.x_min > to_split.x_min)
        {
            copy_non_overlap.x_max = c2.x_min - 1;
            copy_to_split.x_min = c2.x_min;
        }
        else if (c2.x_max < to_split.x_max && c2.x_max >= to_split.x_min)
        {
            copy_non_overlap.x_min = c2.x_max + 1;
            copy_to_split.x_max = c2.x_max;
        }
        else if (c2.y_min <= to_split.y_max && c2.y_min > to_split.y_min)
        {
            copy_non_overlap.y_max = c2.y_min - 1;
            copy_to_split.y_min = c2.y_min;
        }
        else if (c2.y_max < to_split.y_max && c2.y_max >= to_split.y_min)
        {
            copy_non_overlap.y_min = c2.y_max + 1;
            copy_to_split.y_max = c2.y_max;
        }
        else if (c2.z_min <= to_split.z_max && c2.z_min > to_split.z_min)
        {
            copy_non_overlap.z_max = c2.z_min - 1;
            copy_to_split.z_min = c2.z_min;
        }
        else if (c2.z_max < to_split.z_max && c2.z_max >= to_split.z_min)
        {
            copy_non_overlap.z_min = c2.z_max + 1;
            copy_to_split.z_max = c2.z_max;
        }

        res.push_back(copy_non_overlap);
        to_split = copy_to_split;
    }

    return res;
}

struct Space
{
    void insert(const std::pair<bool, Cubeoid> &insert)
    {
        const auto [on, cubeoid] = insert;

        std::vector<Cubeoid> to_split;
        const auto [rem_f, rem_l] = ranges::remove_if(on_cubeoids, [&cubeoid, &to_split](const auto &c)
                                                      {
                                                          const bool overlaps = c.overlaps(cubeoid);
                                                          if (overlaps)
                                                          {
                                                              to_split.push_back(c);
                                                          }
                                                          return overlaps;
                                                      });
        on_cubeoids.erase(rem_f, rem_l);
        for (const auto &ts : to_split)
        {
            const auto splitted = split(ts, cubeoid);
            on_cubeoids.insert(on_cubeoids.end(), splitted.begin(), splitted.end());
        }

        if (on)
        {
            on_cubeoids.push_back(cubeoid);
        }
    }

    uint64_t count() const
    {
        return std::accumulate(on_cubeoids.begin(), on_cubeoids.end(), 0ul, [](auto acc, const auto &cubeoid)
                               { return acc + cubeoid.count(); });
    }

    std::vector<Cubeoid> on_cubeoids;
};

std::pair<bool, Cubeoid> parse_line(const std::string &line)
{
    static const std::regex regx("(\\w+) x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)");
    const auto [status, x1, x2, y1, y2, z1, z2] = aoc::search<7>(line, regx);
    Cubeoid cubeoid = {
        .x_min = std::stoll(x1),
        .x_max = std::stoll(x2),
        .y_min = std::stoll(y1),
        .y_max = std::stoll(y2),
        .z_min = std::stoll(z1),
        .z_max = std::stoll(z2)};
    return {status == "on", cubeoid};
}

uint64_t solve(const std::vector<std::string> &input, bool part2 = false)
{
    const Cubeoid p1_region = {-50, 50, -50, 50, -50, 50};

    Space space;
    for (const auto &line : input)
    {
        const auto c = parse_line(line);
        if (!part2 && !c.second.is_inside_cubeoid(p1_region))
        {
            continue;
        }
        space.insert(c);
    }
    return space.count();
}

int main()
{
    const aoc::StopWatch stop_watch;

    {
        std::vector<std::string> test_input = {
            "on x=10..12,y=10..12,z=10..12",
            "on x=11..13,y=11..13,z=11..13",
            "off x=9..11,y=9..11,z=9..11",
            "on x=10..10,y=10..10,z=10..10"};
        aoc::assert_equal(solve(test_input), 39ul);
    }
    {
        std::vector<std::string> test_input = {
            "on x=10..12,y=10..12,z=1..1",
            "on x=1..2,y=10..12,z=1..1",
            "on x=1..1,y=10..12,z=1..1",
            "on x=-1..1,y=10..12,z=1..1",
            "off x=12..12,y=12..12,z=1..1",
            "off x=12..12,y=12..12,z=2..3",
            "on x=12..12,y=12..12,z=0..1",
            "off x=0..1,y=11..12,z=-1..1",
            "on x=0..8,y=0..3,z=0..3",
            "off x=3..5,y=0..3,z=0..3",
            "off x=0..8,y=0..3,z=0..3",
            "on x=0..8,y=0..3,z=0..3",
            "off x=-10..20,y=1..2,z=1..2",
        };
        aoc::assert_equal(solve(test_input), 126ul);
    }
    {
        const auto lines = aoc::get_lines("test.txt");
        aoc::assert_equal(solve(lines), 590784ul);
    }

    const auto lines = aoc::get_lines("input.txt");
    aoc::assert_equal(solve(lines), 567496ul);
    aoc::assert_equal(solve(lines, true), 1355961721298916ul);
    return 0;
}
