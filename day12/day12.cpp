#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::unordered_map<std::string, std::vector<std::string>> NodeConnections;

inline constexpr char start[] = "start";
inline constexpr char end[] = "end";

inline bool small(const std::string &name)
{
    return (ranges::any_of(name, [](const auto c)
                           { return !isupper(c); }));
};

NodeConnections parse(const std::vector<std::string> &input)
{
    std::unordered_map<std::string, std::vector<std::string>> nodes;
    for (const auto &line : input)
    {
        const auto ns = aoc::split(line, "-");
        for (int i = 0; const auto &el : ns)
        {
            if (!nodes.contains(el))
            {
                nodes.insert({el, {}});
            }
            auto &neighbours = nodes.at(el);
            const std::string &connection = ns[((i++) + 1) % 2];
            if (ranges::find(neighbours, connection) == neighbours.end())
            {
                neighbours.push_back(connection);
            }
        }
    }
    return nodes;
}

class Path
{
public:
    Path(bool part2 = false)
        : visited_small_twice_(!part2)
    {
    }
    bool ok_to_add(const std::string &node) const
    {
        if (node == start)
        {
            return false;
        }
        if (nodes_.contains(node) && small(node))
        {
            return !visited_small_twice_;
        }
        return true;
    }

    void push_back(const std::string &node)
    {
        if (nodes_.contains(node) && small(node))
        {
            visited_small_twice_ = true;
        }
        nodes_.insert(node);
        latest_ = node;
    }

    std::string latest_ = start;

private:
    bool visited_small_twice_ = false;
    std::unordered_set<std::string> nodes_ = {start};
};

std::vector<Path> find_all_paths(const NodeConnections &map, bool part2)
{
    std::vector<Path> searching = {{part2}};
    std::vector<Path> complete;
    while (!searching.empty())
    {
        const auto check = searching;
        searching.clear();
        for (unsigned int i = 0; i < check.size(); i++)
        {
            const auto endpoint = check[i].latest_;
            for (const auto &next : map.at(endpoint))
            {
                if (!check[i].ok_to_add(next))
                {
                    continue;
                }
                auto copy = check[i];
                copy.push_back(next);
                if (next == end)
                {
                    complete.push_back(copy);
                }
                else
                {
                    searching.push_back(copy);
                }
            }
        }
    }
    return complete;
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const std::vector<std::string> test1 = {"start-A",
                                                "start-b",
                                                "A-c",
                                                "A-b",
                                                "b-d",
                                                "A-end",
                                                "b-end"};
        const auto parsed = parse(test1);

        const auto paths_p1 = find_all_paths(parsed, false);
        aoc::assert_equal(paths_p1.size(), 10ul);

        const auto paths_p2 = find_all_paths(parsed, true);
        aoc::assert_equal(paths_p2.size(), 36ul);
    }
    const auto node_map = parse(aoc::get_lines("input.txt"));

    const auto paths_p1 = find_all_paths(node_map, false);
    aoc::assert_equal(paths_p1.size(), 3292ul);

    const auto paths_p2 = find_all_paths(node_map, true);
    aoc::assert_equal(paths_p2.size(), 89592ul);
    return 0;
}
