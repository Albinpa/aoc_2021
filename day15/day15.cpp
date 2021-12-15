#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::vector<std::vector<uint64_t>> Grid;

struct Point
{
    uint64_t x;
    uint64_t y;

    bool operator==(const Point &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
};

struct point_hash
{
    std::size_t operator()(const Point &pair) const
    {
        const auto hash_fun = std::hash<uint64_t>();
        return hash_fun(pair.x) ^ hash_fun(pair.y);
    }
};

typedef std::unordered_set<Point, point_hash> PointSet;

Grid parse(const std::vector<std::string> &input, bool part2 = false)
{
    Grid res;
    for (const auto &line : input)
    {
        std::vector<uint64_t> row;
        for (const auto c : line)
        {
            row.push_back(c - '0');
        }
        res.push_back(row);
    }

    if (part2)
    {
        uint64_t orig_row_size = res[0].size();
        for (uint64_t row_i = 0; row_i < res.size(); row_i++)
        {
            auto &row = res[row_i];
            for (uint64_t i = orig_row_size; i < orig_row_size * 5; i++)
            {
                row.push_back(row[i - orig_row_size] % 9 + 1);
            }
        }

        uint64_t orig_col_size = res.size();
        for (uint64_t i = orig_col_size; i < orig_col_size * 5; i++)
        {
            std::vector<uint64_t> row;
            for (const auto &nr : res[i - orig_col_size])
            {
                row.push_back(nr % 9 + 1);
            }
            res.push_back(row);
        }
    }
    return res;
}

inline std::vector<Point> neighbours(const Grid &grid, const Point &at)
{
    std::vector<Point> res = {{at.x - 1, at.y}, {at.x + 1, at.y}, {at.x, at.y - 1}, {at.x, at.y + 1}};

    const auto [f, l] = ranges::remove_if(res, [&grid](const auto p)
                                          { return p.x >= grid[0].size() || p.y >= grid.size(); });
    res.erase(f, l);
    return res;
}

uint64_t dijkstra(const Grid &cost_grid)
{
    Point start = {0, 0};
    Point end = {cost_grid[0].size() - 1, cost_grid.size() - 1};
    PointSet visited;
    PointSet neighbouring_unvisited = {start};

    Grid distance_map = Grid(cost_grid.size(),
                             std::vector<uint64_t>(cost_grid[0].size(),
                                                   std::numeric_limits<uint64_t>::max()));

    distance_map[0][0] = 0;

    Point current;
    while (true)
    {
        uint64_t min_cost_calc = std::numeric_limits<uint64_t>::max();
        for (const auto &u : neighbouring_unvisited)
        {
            if (distance_map[u.y][u.x] < min_cost_calc)
            {
                current = u;
                min_cost_calc = distance_map[u.y][u.x];
            }
        }
        for (const auto &neighbour : neighbours(cost_grid, current))
        {
            if (!visited.contains(neighbour))
            {
                uint64_t cost = distance_map[current.y][current.x] + cost_grid[neighbour.y][neighbour.x];
                if (cost < distance_map[neighbour.y][neighbour.x])
                {
                    distance_map[neighbour.y][neighbour.x] = cost;
                }
                neighbouring_unvisited.insert(neighbour);
            }
        }
        visited.insert(current);
        neighbouring_unvisited.erase(current);
        if (current == end)
        {
            return distance_map[current.y][current.x];
        }
    }
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const std::vector<std::string> test_input = {
            "1163751742",
            "1381373672",
            "2136511328",
            "3694931569",
            "7463417111",
            "1319128137",
            "1359912421",
            "3125421639",
            "1293138521",
            "2311944581"};
        const auto grid1 = parse(test_input);
        aoc::assert_equal(dijkstra(grid1), 40ul);

        const auto grid2 = parse(test_input, true);
        aoc::assert_equal(dijkstra(grid2), 315ul);
    }
    const auto lines = aoc::get_lines("input.txt");

    const auto grid1 = parse(lines);
    aoc::assert_equal(dijkstra(grid1), 429ul); // part 1

    const auto grid2 = parse(lines, true);
    aoc::assert_equal(dijkstra(grid2), 2844ul); // part 2

    return 0;
}
