#include <algorithm>
#include <iostream>
#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct Position
{
    int64_t x;
    int64_t y;
    int64_t z;

    auto operator<=>(const Position &) const = default;

    Position operator+(const Position &rhs) const
    {
        return {
            x + rhs.x,
            y + rhs.y,
            z + rhs.z,
        };
    }

    Position operator-(const Position &rhs) const
    {
        return {
            x - rhs.x,
            y - rhs.y,
            z - rhs.z,
        };
    }

    uint64_t manhattan(const Position &rhs) const
    {
        return std::abs(x - rhs.x) +
               std::abs(y - rhs.y) +
               std::abs(z - rhs.z);
    }

    void print() const
    {
        aoc::print("{", x, ",", y, ",", z, "}");
    }
};

struct position_hash
{
    std::size_t operator()(const Position &pos) const
    {
        return std::hash<uint64_t>()(pos.x) ^
               std::hash<uint64_t>()(pos.y) ^
               std::hash<uint64_t>()(pos.z);
    }
};

typedef std::function<Position(const Position &)> Rotation;

std::vector<Rotation> orientations()
{
    return {
        // facing x
        [](const Position &p) -> Position
        { return {p.x, p.y, p.z}; }, // facing x, y is up
        [](const Position &p) -> Position
        { return {p.x, -p.y, -p.z}; }, // facing x, -y is up
        [](const Position &p) -> Position
        { return {p.x, -p.z, p.y}; }, // facing x, z is up
        [](const Position &p) -> Position
        { return {p.x, p.z, -p.y}; }, // facing x, -z is up

        // facing -x
        [](const Position &p) -> Position
        { return {-p.x, p.y, -p.z}; }, // facing -x, y is up
        [](const Position &p) -> Position
        { return {-p.x, -p.y, p.z}; }, // facing -x, -y is up
        [](const Position &p) -> Position
        { return {-p.x, p.z, p.y}; }, // facing -x, z is up
        [](const Position &p) -> Position
        { return {-p.x, -p.z, -p.y}; }, // facing -x, -z is up

        // facing y
        [](const Position &p) -> Position
        { return {p.y, p.x, -p.z}; }, // x is up
        [](const Position &p) -> Position
        { return {-p.y, p.x, p.z}; }, // -x is up
        [](const Position &p) -> Position
        { return {p.z, p.x, p.y}; }, // z is up
        [](const Position &p) -> Position
        { return {-p.z, p.x, -p.y}; }, // -z is up

        // facing -y
        [](const Position &p) -> Position
        { return {p.y, -p.x, p.z}; }, // x is up
        [](const Position &p) -> Position
        { return {-p.y, -p.x, -p.z}; }, // -x is up
        [](const Position &p) -> Position
        { return {-p.z, -p.x, p.y}; }, // z is up
        [](const Position &p) -> Position
        { return {p.z, -p.x, -p.y}; }, // -z is up

        // facing z
        [](const Position &p) -> Position
        { return {p.y, p.z, p.x}; }, // x is up
        [](const Position &p) -> Position
        { return {-p.y, -p.z, p.x}; }, // -x is up
        [](const Position &p) -> Position
        { return {-p.z, p.y, p.x}; }, // y is up
        [](const Position &p) -> Position
        { return {p.z, -p.y, p.x}; }, // -y is up

        // facing -z
        [](const Position &p) -> Position
        { return {p.y, -p.z, -p.x}; }, // x is up
        [](const Position &p) -> Position
        { return {-p.y, p.z, -p.x}; }, // -x is up
        [](const Position &p) -> Position
        { return {p.z, p.y, -p.x}; }, // y is up
        [](const Position &p) -> Position
        { return {-p.z, -p.y, -p.x}; }, // -y is up
    };
}

struct Scanner
{
    std::string id_;
    std::vector<std::vector<Position>> beacons_;
    bool locked_ = false;
    Position relative_scanner0_;
    std::unordered_set<Position, position_hash> locked_set_;

    void lock(uint64_t locked_orientation, Position relative_scanner0)
    {
        locked_ = true;
        relative_scanner0_ = relative_scanner0;
        for (const auto &beacon : beacons_)
        {
            locked_set_.insert(beacon[locked_orientation]);
        }
    }

    bool overlaps(Scanner &sc, std::unordered_set<Position, position_hash> &list_of_global_beacons)
    {
        // for each orientation in input
        const auto locked = locked_set_;
        for (uint64_t orientation = 0; orientation < 24ul; orientation++)
        {
            // for each position in input
            // for each position in this
            for (const auto &beacon_variations : sc.beacons_)
            {
                const auto &beacon = beacon_variations.at(orientation);
                for (const auto &locked_beacon : locked)
                {
                    // translate to match
                    Position translation = locked_beacon - beacon;
                    // check if 12 others match with this translation
                    std::unordered_set<Position, position_hash> matched;
                    for (uint64_t i = 0; i < sc.beacons_.size(); i++)
                    {
                        const Position &without = sc.beacons_.at(i).at(orientation);
                        const Position translated = without + translation;
                        if (locked.contains(translated) &&
                            !matched.contains(translated))
                        {
                            matched.insert(translated);
                        }
                    }
                    if (matched.size() >= 12ul)
                    {
                        // if yes, lock down orientation in this
                        // save translated points in this orientation to a global map
                        // return true
                        const Position trans = relative_scanner0_ + translation;
                        sc.lock(orientation, trans);

                        // update global map
                        for (uint64_t ig = 0; ig < sc.beacons_.size(); ig++)
                        {
                            const auto trans_to_glob = sc.beacons_[ig][orientation] + trans;
                            if (!list_of_global_beacons.contains(trans_to_glob))
                            {
                                list_of_global_beacons.insert(trans_to_glob);
                            }
                        }

                        return true;
                    }
                }
            }
        }
        return false;
    }
};

std::vector<Scanner> parse(const std::vector<std::string> &input)
{
    const auto orients = orientations();

    std::vector<Scanner> res;
    auto start = input.begin();
    while (true)
    {
        const auto end = std::find(start, input.end(), "");
        static const std::regex regx("scanner (\\d+)");
        const auto [id] = aoc::search<1>(*start, regx);
        Scanner sc;
        sc.id_ = id;
        for (auto it = start + 1; it != end; it++)
        {
            const auto splitted = aoc::split(*it, ",");
            Position p = {
                std::stoll(splitted[0]),
                std::stoll(splitted[1]),
                std::stoll(splitted[2]),
            };
            std::vector<Position> pos_options;
            for (const auto &ori : orients)
            {
                pos_options.push_back(ori(p));
            }
            sc.beacons_.push_back(pos_options);
        }
        res.push_back(sc);

        if (end == input.end())
        {
            break;
        }
        start = end + 1;
    }
    return res;
}

std::pair<uint64_t, uint64_t> reduce(std::vector<Scanner> &scanners)
{
    std::unordered_set<Position, position_hash> list_of_beacons;
    scanners[0].lock(0, {0, 0, 0});

    for (const auto &beacon : scanners[0].beacons_)
    {
        list_of_beacons.insert(beacon[0]);
    }

    while (true)
    {
        uint64_t nr_locked = 0;
        for (uint64_t i = 0; i < scanners.size(); i++)
        {
            if (scanners[i].locked_)
            {
                nr_locked++;
                auto &locked = scanners[i];
                for (auto &sc : scanners)
                {
                    if (sc.id_ == locked.id_ || sc.locked_)
                    {
                        continue;
                    }

                    locked.overlaps(sc, list_of_beacons);
                }
            }
        }
        if (nr_locked == scanners.size())
        {
            break;
        }
    }
    uint64_t largest_manhattan = 0;
    for (const auto &scanner1 : scanners)
    {
        for (const auto &scanner2 : scanners)
        {
            if (scanner1.id_ == scanner2.id_)
            {
                continue;
            }
            largest_manhattan = std::max(largest_manhattan,
                                         scanner1.relative_scanner0_.manhattan(scanner2.relative_scanner0_));
        }
    }
    return {list_of_beacons.size(), largest_manhattan};
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        std::unordered_set<Position, position_hash> list_of_beacons;
        auto parsed = parse(aoc::get_lines("test.txt"));
        parsed[0].lock(0, {0, 0, 0});
        aoc::assert_equal(parsed[0].overlaps(parsed[1], list_of_beacons), true);
        aoc::assert_equal(parsed[1].overlaps(parsed[4], list_of_beacons), true);
    }
    {
        auto parsed = parse(aoc::get_lines("test.txt"));
        const auto [nr_beacons, manhattan] = reduce(parsed);
        aoc::assert_equal(nr_beacons, 79ul);
        aoc::assert_equal(manhattan, 3621ul);
    }

    auto parsed = parse(aoc::get_lines("input.txt"));
    const auto [nr_beacons, manhattan] = reduce(parsed);
    aoc::assert_equal(nr_beacons, 342ul);
    aoc::assert_equal(manhattan, 9668ul);

    return 0;
}
