
#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::pair<uint64_t, uint64_t> Point;

struct point_hash
{
    std::size_t operator()(const Point &pair) const
    {
        const auto hash_fun = std::hash<uint64_t>();
        return hash_fun(pair.first) ^ hash_fun(pair.second);
    }
};

typedef std::unordered_set<Point, point_hash> PointMap;

struct FoldInstruction
{
    char direction_;
    uint64_t position_;
};

std::pair<PointMap, std::vector<FoldInstruction>>
parse(const std::vector<std::string> &input)
{
    const auto separator = ranges::find(input, "");
    PointMap points;
    ranges::for_each(input.begin(), separator, [&points](const auto &line)
                     {
                         const auto splitted = aoc::split(line, ",");
                         points.insert({std::stoull(splitted.at(0)), std::stoull(splitted.at(1))});
                     });

    const std::regex regex("fold along (\\w)=(\\d+)");
    std::vector<FoldInstruction> instructions;
    std::transform(separator + 1, input.end(), std::back_inserter(instructions),
                   [&regex](const auto &line)
                   {
                       const auto [direction, nr] = aoc::search<2>(line, regex);
                       const FoldInstruction ins = {direction[0], std::stoull(nr)};
                       return ins;
                   });

    return {points, instructions};
}

Point fold_point(const Point &point, const FoldInstruction &instruction)
{
    auto res = point;
    if (instruction.direction_ == 'x' &&
        point.first > instruction.position_)
    {
        res.first = instruction.position_ * 2 - point.first;
    }
    else if (instruction.direction_ == 'y' &&
             point.second > instruction.position_)
    {
        res.second = instruction.position_ * 2 - point.second;
    }
    return res;
}

PointMap visible_dots(const PointMap &og_points, const std::vector<FoldInstruction> &ins)
{
    auto points = og_points;
    PointMap after_ins = {};

    for (const auto &instruction : ins)
    {
        after_ins.clear();
        for (auto &point : points)
        {
            after_ins.insert(fold_point(point, instruction));
        }
        points = after_ins;
    }
    return points;
}

void print_visible_dots(const PointMap &points)
{
    Point max = {0, 0};
    for (const auto &point : points)
    {
        max = {std::max(point.first, max.first), std::max(point.second, max.second)};
    }
    auto pretty = std::vector<std::vector<char>>(max.second + 1, std::vector<char>(max.first + 1, ' '));
    for (const auto &point : points)
    {
        pretty.at(point.second).at(point.first) = '#';
    }
    aoc::print(pretty);
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const auto [map, instructions] = parse(aoc::get_lines("test.txt"));

        aoc::assert_equal(visible_dots(map, {instructions[0]}).size(), 17ul);
    }
    const auto [map, instructions] = parse(aoc::get_lines("input.txt"));

    aoc::assert_equal(visible_dots(map, {instructions[0]}).size(), 775ul); // part 1
    print_visible_dots(visible_dots(map, instructions));                   // part 2

    return 0;
}
