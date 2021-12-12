#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::unordered_map<std::string, std::vector<std::string>> NodeConnections;

inline constexpr char start[] = "start";
inline constexpr char end[] = "end";

inline bool small(const std::string &name)
{
    return ranges::any_of(name, [](const auto c)
                          { return !isupper(c); });
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
    Path(const std::string &last_node_name, const std::shared_ptr<Path> &tail, bool part2 = false)
        : last_node_name_(last_node_name),
          tail_(tail)
    {
        if (!part2 ||
            (tail && tail->visited_small_twice_) ||
            (small(last_node_name) && contains(last_node_name)))
        {
            visited_small_twice_ = true;
        }
    }

    bool ok_to_add(const std::string &node) const
    {
        if (node == start)
        {
            return false;
        }
        if (small(node) && contains(node))
        {
            return !visited_small_twice_;
        }
        return true;
    }

    std::string last_node_name_;

private:
    bool contains(const std::string &name) const
    {
        if (!tail_)
        {
            return false;
        }
        if (tail_->last_node_name_ == name)
        {
            return true;
        }
        return tail_->contains(name);
    }

    bool visited_small_twice_ = false;
    std::shared_ptr<Path> tail_;
};

uint64_t find_all_paths(const NodeConnections &map, bool part2)
{
    std::vector<std::shared_ptr<Path>> searching = {std::make_shared<Path>(start, nullptr, part2)};
    uint64_t complete = 0;
    while (!searching.empty())
    {
        const auto tail = searching.back();
        searching.pop_back();
        for (const auto &next : map.at(tail->last_node_name_))
        {
            if (next == end)
            {
                complete++;
                continue;
            }
            if (tail->ok_to_add(next))
            {
                searching.push_back(std::make_shared<Path>(next, tail, part2));
            }
        }
    }
    return complete;
}

int main()
{
    const aoc::StopWatch stop_watch;

    const std::vector<std::string> test1 = {"start-A",
                                            "start-b",
                                            "A-c",
                                            "A-b",
                                            "b-d",
                                            "A-end",
                                            "b-end"};
    const auto parsed = parse(test1);

    aoc::assert_equal(find_all_paths(parsed, false), 10ul);
    aoc::assert_equal(find_all_paths(parsed, true), 36ul);

    const auto node_map = parse(aoc::get_lines("input.txt"));

    aoc::assert_equal(find_all_paths(node_map, false), 3292ul);
    aoc::assert_equal(find_all_paths(node_map, true), 89592ul);

    return 0;
}
