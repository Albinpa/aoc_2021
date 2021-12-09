#include "aoc_library.hpp"

typedef std::vector<std::vector<uint16_t>> Heatmap;
typedef std::pair<uint64_t, uint64_t> Point;

namespace ranges = std::ranges;

struct Basin
{
    Point max;
    Heatmap heatmap;
    std::vector<Point> visited = std::vector<Point>();
};

uint64_t sizeof_basin(Basin &basin, Point start)
{
    if (start.first > basin.max.first ||
        start.second > basin.max.second ||
        ranges::find(basin.visited, start) != basin.visited.end() ||
        basin.heatmap[start.second][start.first] >= 9)
    {
        // stop exploring;
        return 0;
    }
    basin.visited.push_back(start);
    const auto [x, y] = start;
    return 1 +
           sizeof_basin(basin, {x + 1, y}) +
           sizeof_basin(basin, {x - 1, y}) +
           sizeof_basin(basin, {x, y + 1}) +
           sizeof_basin(basin, {x, y - 1});
}

std::pair<uint64_t, uint64_t> find_basins(const std::vector<std::string> &input)
{
    Heatmap heatmap;
    for (const auto &line : input)
    {
        std::vector<uint16_t> row;
        for (const auto &nr : line)
        {
            row.push_back(nr - '0');
        }
        heatmap.push_back(row);
    }

    uint64_t count = 0;
    std::vector<uint64_t> basins;
    uint64_t ymax = heatmap.size() - 1;
    uint64_t xmax = heatmap[0].size() - 1;
    for (unsigned int y = 0; y <= ymax; y++)
    {
        for (unsigned int x = 0; x < heatmap[0].size(); x++)
        {
            const uint16_t ix = x < xmax ? heatmap[y][x + 1] : 10;
            const uint16_t xi = x > 0 ? heatmap[y][x - 1] : 10;
            const uint16_t iy = y < ymax ? heatmap[y + 1][x] : 10;
            const uint16_t yi = y > 0 ? heatmap[y - 1][x] : 10;

            if (heatmap[y][x] < std::min({ix, xi, iy, yi}))
            {
                count += heatmap[y][x] + 1;
                Basin basin = {.max = {xmax, ymax}, .heatmap = heatmap};
                basins.push_back(sizeof_basin(basin, {x, y}));
            }
        }
    }

    uint64_t basin_score = 1;
    ranges::sort(basins);
    for (const auto top3 : basins | ranges::views::reverse | ranges::views::take(3))
    {
        basin_score *= top3;
    }
    return {count, basin_score};
}

int main()
{
    const aoc::StopWatch stop_watch;
    const auto lines = aoc::get_lines("input.txt");

    const std::vector<std::string> testdata = {
        "2199943210",
        "3987894921",
        "9856789892",
        "8767896789",
        "9899965678",
    };
    const auto [count_lowpoints_test, basin_mul_test] = find_basins(testdata);
    aoc::assert_equal(count_lowpoints_test, 15ul);
    aoc::assert_equal(basin_mul_test, 1134ul);

    const auto [count_lowpoints, basin_mul] = find_basins(lines);
    aoc::assert_equal(count_lowpoints, 506ul);
    aoc::assert_equal(basin_mul, 931200ul);

    return 0;
}
