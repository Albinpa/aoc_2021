#include <variant>
#include <math.h>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct SnailPair
{
    uint64_t id;
    std::weak_ptr<SnailPair> parent;
    std::variant<std::shared_ptr<SnailPair>, uint64_t> left;
    std::variant<std::shared_ptr<SnailPair>, uint64_t> right;

    void print()
    {
        const auto print_variant = [](const auto &variant)
        {
            if (std::holds_alternative<std::shared_ptr<SnailPair>>(variant))
            {
                std::get<std::shared_ptr<SnailPair>>(variant)->print();
            }
            else
            {
                aoc::printnl(std::get<uint64_t>(variant));
            }
        };
        aoc::printnl("[");
        print_variant(right);
        aoc::printnl(",");
        print_variant(left);
        aoc::printnl("]");
        if (!parent.lock()) [[unlikely]]
        {
            aoc::print("");
        }
    }
};

typedef std::shared_ptr<SnailPair> SnailPtr;

inline bool should_split(const std::variant<SnailPtr, uint64_t> &head)
{
    return std::holds_alternative<uint64_t>(head) && std::get<uint64_t>(head) >= 10ul;
}

inline std::optional<SnailPtr> search_split(const SnailPtr &snail)
{
    if (should_split(snail->left))
    {
        return snail;
    }
    else if (std::holds_alternative<SnailPtr>(snail->left))
    {
        const auto left = search_split(std::get<SnailPtr>(snail->left));
        if (left)
        {
            return left;
        }
    }
    if (should_split(snail->right))
    {
        return snail;
    }
    else if (std::holds_alternative<SnailPtr>(snail->right))
    {
        const auto right = search_split(std::get<SnailPtr>(snail->right));
        if (right)
        {
            return right;
        }
    }
    return std::nullopt;
}

struct Generator
{
    uint64_t unique_id_ = 10;

    inline SnailPtr parse_line(const std::string line)
    {
        std::string line_cp = line;
        std::unordered_map<uint64_t, SnailPtr> parse_map;

        while (true)
        {
            const static std::regex search_re("\\[(\\d+),(\\d+)\\]");
            const auto [l, r] = aoc::search<2>(line_cp, search_re);

            uint64_t left_nr = 0;
            uint64_t right_nr = 0;
            try
            {
                left_nr = std::stoul(l);
                right_nr = std::stoul(r);
            }
            catch (const std::exception &)
            {
                // Done parsing line
                break;
            }
            unique_id_++;

            auto pair = std::make_shared<SnailPair>();
            pair->id = unique_id_;
            if (left_nr < 10ul)
            {
                pair->left = left_nr;
            }
            else
            {
                auto kid = parse_map.at(left_nr);
                pair->left = kid;
                kid->parent = pair;
            }
            if (right_nr < 10ul)
            {
                pair->right = right_nr;
            }
            else
            {
                auto kid = parse_map.at(right_nr);
                pair->right = kid;
                kid->parent = pair;
            }
            parse_map.emplace(unique_id_, pair);

            const static std::regex replace_re("(\\[\\d+,\\d+\\])");
            line_cp = std::regex_replace(line_cp, replace_re, std::to_string(unique_id_),
                                         std::regex_constants::format_first_only);
        }
        return parse_map.at(std::stoul(line_cp));
    }

    inline SnailPtr add(const SnailPtr &lhs, const SnailPtr &rhs)
    {
        const auto new_pair = std::make_shared<SnailPair>();
        new_pair->id = unique_id_++;
        new_pair->left = lhs;
        lhs->parent = new_pair;
        new_pair->right = rhs;
        rhs->parent = new_pair;
        return new_pair;
    }

    inline bool split(const SnailPtr &snail)
    {
        const auto to_split = search_split(snail);
        if (!to_split.has_value())
        {
            return false;
        }
        const auto split_snail = to_split.value();

        const auto new_pair = std::make_shared<SnailPair>();
        new_pair->id = unique_id_++;
        new_pair->parent = split_snail;

        long double half = 0;
        if (should_split(split_snail->left))
        {
            half = (long double)std::get<uint64_t>(split_snail->left) / 2.0;
            split_snail->left = new_pair;
        }
        else
        {
            half = (long double)std::get<uint64_t>(split_snail->right) / 2.0;
            split_snail->right = new_pair;
        }
        new_pair->left = (uint64_t)std::floor(half);
        new_pair->right = (uint64_t)std::ceil(half);
        return true;
    }
};

inline uint64_t magnitude(const SnailPtr &snail)
{
    const auto get_magnitude = [](const auto &variant)
    {
        if (std::holds_alternative<SnailPtr>(variant))
        {
            return magnitude(std::get<SnailPtr>(variant));
        }
        else
        {
            return std::get<uint64_t>(variant);
        }
    };
    const uint64_t left_mag = get_magnitude(snail->left);
    const uint64_t right_mag = get_magnitude(snail->right);
    return left_mag * 3 + right_mag * 2;
}

typedef std::function<std::variant<SnailPtr, uint64_t>(SnailPtr)> SearchParentFunc;

inline void add_to_first_on_side(const SnailPtr &snail, uint64_t val,
                                 SearchParentFunc search_up_side, SearchParentFunc search_down_side)
{
    auto snail_it = snail;
    // search up
    while (true)
    {
        const auto parent = snail_it->parent.lock();
        if (!parent)
        {
            return;
        }
        const auto &search_up_child = search_up_side(parent);
        if (std::holds_alternative<SnailPtr>(search_up_child) &&
            std::get<SnailPtr>(search_up_child)->id != snail_it->id)
        {
            snail_it = std::get<SnailPtr>(search_up_child);
            break; // search down for right most of sibling
        }
        else if (std::holds_alternative<uint64_t>(search_up_child))
        {
            parent->left = std::get<uint64_t>(search_up_child) + val;
            return;
        }
        snail_it = parent;
    }
    // search down
    while (true)
    {
        const auto &search_down_child = search_down_side(snail_it);
        if (std::holds_alternative<SnailPtr>(search_down_child))
        {
            snail_it = std::get<SnailPtr>(search_down_child);
            continue; // keep searching
        }
        else
        {
            snail_it->right = std::get<uint64_t>(search_down_child) + val;
            return;
        }
    }
}

inline std::variant<SnailPtr, uint64_t> search_left(const SnailPtr &parent)
{
    return parent->left;
}

inline std::variant<SnailPtr, uint64_t> search_right(const SnailPtr &parent)
{
    return parent->right;
}

inline void add_to_first_left(const SnailPtr &snail, uint64_t val)
{
    add_to_first_on_side(snail, val, search_left, search_right);
}

inline void add_to_first_right(const SnailPtr &snail, uint64_t val)
{
    auto snail_it = snail;
    // search up
    while (true)
    {
        const auto parent = snail_it->parent.lock();
        if (!parent)
        {
            return;
        }
        if (std::holds_alternative<SnailPtr>(parent->right) &&
            std::get<SnailPtr>(parent->right)->id != snail_it->id)
        {
            snail_it = std::get<SnailPtr>(parent->right);
            break; // search down for right most of sibling
        }
        else if (std::holds_alternative<uint64_t>(parent->right))
        {
            parent->right = std::get<uint64_t>(parent->right) + val;
            return;
        }
        snail_it = parent;
    }
    // search down
    while (true)
    {
        if (std::holds_alternative<SnailPtr>(snail_it->left))
        {
            snail_it = std::get<SnailPtr>(snail_it->left);
            continue; // keep searching
        }
        else
        {
            snail_it->left = std::get<uint64_t>(snail_it->left) + val;
            return;
        }
    }
}

inline std::optional<SnailPtr> search_explode(const SnailPtr &snail, uint64_t nested_in)
{
    if (nested_in >= 4)
    {
        return snail;
    }
    if (std::holds_alternative<SnailPtr>(snail->left))
    {
        const auto left = search_explode(std::get<SnailPtr>(snail->left), nested_in + 1);
        if (left)
        {
            return left;
        }
    }
    if (std::holds_alternative<SnailPtr>(snail->right))
    {
        const auto right = search_explode(std::get<SnailPtr>(snail->right), nested_in + 1);
        if (right)
        {
            return right;
        }
    }
    return std::nullopt;
}

inline bool explode(const SnailPtr &snail)
{
    // search for explode criteria (a pair is nested inside four pairs)
    // not found? return false
    const auto to_explode = search_explode(snail, 0ul);
    if (!to_explode.has_value())
    {
        return false;
    }
    // found?
    // search for first number to the left and right to give add values
    const auto explode_snail = to_explode.value();
    add_to_first_left(explode_snail, std::get<uint64_t>(explode_snail->left));
    add_to_first_right(explode_snail, std::get<uint64_t>(explode_snail->right));
    // replace exploding with 0
    const auto parent = explode_snail->parent.lock();
    if (std::holds_alternative<SnailPtr>(parent->left) &&
        std::get<SnailPtr>(parent->left)->id == explode_snail->id)
    {
        parent->left = 0ul;
    }
    else
    {
        parent->right = 0ul;
    }

    return true;
}

inline SnailPtr addition(const SnailPtr &lhs, const SnailPtr &rhs, Generator &gen)
{
    const auto new_pair = gen.add(lhs, rhs);
    while (true)
    {
        if (explode(new_pair))
        {
            continue;
        }
        if (!gen.split(new_pair))
        {
            break;
        }
    }
    return new_pair;
}

uint64_t homework_p1(const std::vector<std::string> &lines)
{
    Generator gen;
    auto sum = gen.parse_line(lines[0]);
    for (const auto &line : lines | ranges::views::drop(1))
    {
        sum = addition(sum, gen.parse_line(line), gen);
    }
    return magnitude(sum);
}

uint64_t homework_p2(const std::vector<std::string> &lines)
{
    Generator gen;
    const auto parsed_lines = ranges::views::transform(lines, [&gen](const auto &line)
                                                       { return gen.parse_line(line); });
    uint64_t largest_mag = 0;
    for (uint64_t a = 0; a < parsed_lines.size(); a++)
    {
        for (uint64_t b = 0; b < parsed_lines.size(); b++)
        {
            if (a == b)
                continue;
            const auto mag = magnitude(addition(parsed_lines[a], parsed_lines[b], gen));
            largest_mag = std::max(mag, largest_mag);
        }
    }
    return largest_mag;
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        Generator test_gen;

        test_gen.parse_line("[[1,9],[8,5]]")->print();

        test_gen.parse_line("[[[[1,3],[5,3]],[[1,3],[8,7]]],[[[4,9],[6,9]],[[8,2],[7,3]]]]")->print();

        test_gen.add(test_gen.parse_line("[1,2]"), test_gen.parse_line("[[3,4],5]"))->print();

        aoc::assert_equal(magnitude(test_gen.parse_line("[[9,1],[1,9]]")), 129ul);
        aoc::assert_equal(magnitude(test_gen.parse_line("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]")), 3488ul);

        const auto test_split_snail = test_gen.parse_line("[[[[0,7],4],[1,[0,1]]],[1,1]]");
        aoc::assert_equal(search_split(test_split_snail).has_value(), false);
        aoc::assert_equal(test_gen.split(test_split_snail), false);

        const auto test_explode_snail = test_gen.parse_line("[[[[[4,3],4],4],[7,[[8,4],9]]],[1,1]]");
        const auto exploding_snail = search_explode(test_explode_snail, 0ul);
        aoc::assert_equal(exploding_snail.has_value(), true);
        aoc::assert_equal(explode(test_explode_snail), true);
        test_explode_snail->print();

        const auto test_snail1 = addition(test_gen.parse_line("[[[[4,3],4],4],[7,[[8,4],9]]]"),
                                          test_gen.parse_line("[1,1]"), test_gen);
        test_snail1->print();

        const auto test_snail2 = addition(test_gen.parse_line("[[[0,[4,5]],[0,0]],[[[4,5],[2,6]],[9,5]]]"),
                                          test_gen.parse_line("[7,[[[3,7],[4,3]],[[6,3],[8,8]]]]"), test_gen);
        test_snail2->print();

        const auto lines = aoc::get_lines("test.txt");
        aoc::assert_equal(homework_p1(lines), 4140ul);
        aoc::assert_equal(homework_p2(lines), 3993ul);
    }

    const auto lines = aoc::get_lines("input.txt");
    aoc::assert_equal(homework_p1(lines), 3816ul); // part 1
    aoc::assert_equal(homework_p2(lines), 4819ul); // part 2
    return 0;
}
