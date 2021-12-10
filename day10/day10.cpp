#include <numeric>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

class Compiler
{
public:
    uint64_t check_syntax(const std::string &line)
    {
        stack_.clear();
        for (const auto c : line)
        {
            if (!lookup_.contains(c)) // if c is a opener, add to stack
            {
                stack_.push_back(c);
                continue;
            }

            const auto [expect, score] = lookup_.at(c);
            if (expect != stack_.back()) // c is closing, does it match stack?
            {
                return score;
            }

            stack_.pop_back();
        }
        return 0;
    }

    uint64_t stack_score() const
    {
        uint64_t score = 0;
        for (const auto c : stack_ | ranges::views::reverse)
        {
            score = score * 5 + stack_scoring_.at(c);
        }
        return score;
    }

private:
    const std::unordered_map<char, uint64_t> stack_scoring_ = {
        {'(', 1},
        {'{', 3},
        {'[', 2},
        {'<', 4},
    };
    const std::unordered_map<char, std::pair<char, uint64_t>> lookup_ = {
        {')', {'(', 3}},
        {']', {'[', 57}},
        {'}', {'{', 1197}},
        {'>', {'<', 25137}},
    };
    std::vector<char> stack_;
};

uint64_t part1(Compiler &comp, const std::vector<std::string> &input)
{
    return std::accumulate(input.begin(), input.end(), 0, [&comp](auto acc, const auto &s)
                           { return acc + comp.check_syntax(s); });
}

uint64_t part2(Compiler &comp, const std::vector<std::string> &input)
{
    std::vector<uint64_t> scores;
    for (const auto &line : input)
    {
        if (!comp.check_syntax(line))
        {
            scores.push_back(comp.stack_score());
        }
    }
    ranges::sort(scores);
    return scores[(scores.size() - 1) / 2];
}

int main()
{
    const aoc::StopWatch stop_watch;

    Compiler comp;
    aoc::assert_equal(comp.check_syntax("{([(<{}[<>[]}>{[]{[(<()>"), 1197ul);
    aoc::assert_equal(comp.check_syntax("[[<[([]))<([[{}[[()]]]"), 3ul);
    aoc::assert_equal(comp.check_syntax("<{([([[(<>()){}]>(<<{{"), 25137ul);

    const std::vector<std::string> test = {
        "[({(<(())[]>[[{[]{<()<>>",
        "[(()[<>])]({[<{<<[]>>(",
        "(((({<>}<{<{<>}{[]{[]{}",
        "{<[[]]>}<{[{[{[]{()[[[]",
        "<{([{{}}[<[[[<>{}]]]>[]]",
    };

    const auto lines = aoc::get_lines("input.txt");

    aoc::assert_equal(part1(comp, lines), 394647ul); // part 1
    aoc::assert_equal(part2(comp, test), 288957ul);
    aoc::assert_equal(part2(comp, lines), 2380061249ul); // part 2
    return 0;
}
