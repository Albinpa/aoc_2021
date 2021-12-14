#include "aoc_library.hpp"

namespace ranges = std::ranges;

typedef std::unordered_map<std::string, uint64_t> ElementCount;
typedef std::unordered_map<std::string, ElementCount> PairToElementCount;
typedef std::unordered_map<std::string, std::string> InsertRules;

struct Problem
{
    std::string polymer_;
    std::unordered_map<std::string, std::string> pair_insert_rules_;
};

Problem parse(const std::vector<std::string> &input)
{
    InsertRules insert_rules;
    ranges::for_each(input | ranges::views::drop(2), [&insert_rules](const auto &s)
                     {
                         const auto splitted = aoc::split(s, " -> ");
                         insert_rules.emplace(splitted[0], splitted[1]);
                     });
    return {input[0], insert_rules};
}

inline void update(ElementCount &map, std::pair<std::string, uint64_t> update)
{
    if (map.contains(update.first))
    {
        map.at(update.first) += update.second;
    }
    else
    {
        map.insert(update);
    }
}

inline void update(ElementCount &count1, const ElementCount &count2)
{
    for (const auto &el : count2)
    {
        update(count1, el);
    }
}

PairToElementCount expand(const InsertRules &insert_rules, uint64_t times)
{
    PairToElementCount count_map;
    for (const auto &[pair, element] : insert_rules)
    {
        ElementCount count;
        update(count, {pair.substr(0, 1), 1});
        update(count, {element, 1});
        update(count, {pair.substr(1, 1), 1});
        count_map.emplace(pair, count);
    }

    for (uint64_t i = 1; i < times; i++)
    {
        auto copy = count_map;
        for (auto &[pair, element_count] : copy)
        {
            const auto to_insert = insert_rules.at(pair);
            const auto left = pair.substr(0, 1) + to_insert;
            const auto right = to_insert + pair.substr(1, 1);

            element_count = count_map.at(left);
            update(element_count, count_map.at(right));
            element_count.at(to_insert)--;
        }
        count_map = copy;
    }
    return count_map;
}

uint64_t solve(const PairToElementCount &lookup, const std::string &polymer)
{
    ElementCount resulting = {{polymer.substr(0, 1), 1}};
    for (uint64_t i = 0; i < polymer.size() - 1; i++)
    {
        const auto pair = polymer.substr(i, 2);
        update(resulting, lookup.at(pair));
        resulting.at(pair.substr(0, 1))--;
    }

    uint64_t min = std::numeric_limits<uint64_t>::max();
    uint64_t max = 0;
    for (const auto &[c, nr] : resulting)
    {
        min = std::min(min, nr);
        max = std::max(max, nr);
    }
    return max - min;
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const std::vector<std::string> test_input = {"NNCB",
                                                     "",
                                                     "CH -> B",
                                                     "HH -> N",
                                                     "CB -> H",
                                                     "NH -> C",
                                                     "HB -> C",
                                                     "HC -> B",
                                                     "HN -> C",
                                                     "NN -> C",
                                                     "BH -> H",
                                                     "NC -> B",
                                                     "NB -> B",
                                                     "BN -> B",
                                                     "BB -> N",
                                                     "BC -> B",
                                                     "CC -> N",
                                                     "CN -> C"};

        const auto [polymer, pair_lookup] = parse(test_input);
        aoc::assert_equal(solve(expand(pair_lookup, 10), polymer), 1588ul);          // test part 1
        aoc::assert_equal(solve(expand(pair_lookup, 40), polymer), 2188189693529ul); // test part 2
    }

    const auto lines = aoc::get_lines("input.txt");
    const auto [polymer, pair_lookup] = parse(lines);

    aoc::assert_equal(solve(expand(pair_lookup, 10), polymer), 3247ul);          // part 1
    aoc::assert_equal(solve(expand(pair_lookup, 40), polymer), 4110568157153ul); // part 2

    return 0;
}
