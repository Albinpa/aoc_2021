#include <variant>
#include <unordered_set>
#include <functional>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct Var
{
    int64_t var1;
    int64_t var2;
    int64_t var3;
};

inline int64_t z_func(int64_t prev_z, int64_t inp, const Var& v)
{
    const int64_t tmp = prev_z / v.var1;
    if (inp == (prev_z % 26 + v.var2))
    {
        return tmp;
    }
    return tmp * 26 + inp + v.var3;
}

typedef std::function<bool(int64_t, int64_t)> Comparator;

int64_t solve(const std::vector<std::string>& lines, const Comparator& cmp)
{
    std::vector<Var> sections;
    static const std::regex regx("\\w+ \\w\\s*([-\\d\\w]+)*");

    auto start = lines.begin();
    while (true)
    {
        const auto target = std::find_if(start + 1, lines.end(), [](const auto& line)
                                            { return line.starts_with("inp"); });
        Var section;

        const auto [v1] = aoc::search<1>(*(start + 4), regx);
        section.var1 = std::stoll(v1);
        const auto [v2] = aoc::search<1>(*(start + 5), regx);
        section.var2 = std::stoll(v2);
        const auto [v3] = aoc::search<1>(*(start + 15), regx);
        section.var3 = std::stoll(v3);

        sections.push_back(section);
        if (target == lines.end())
            break;
        start = target;
    }

    std::unordered_map<int64_t, int64_t> prev_z = {{0, 0}};
    std::unordered_map<int64_t, int64_t> tmp_prev_z = {};

    for (const auto& v: sections)
    {
        tmp_prev_z.clear();
        for (const auto & [z, best] : prev_z)
        {
            for (int64_t inp = 1; inp < 10; inp++)
            {
                const auto nz = z_func(z, inp, v);
                const int64_t val = best * 10 + inp;
                if (tmp_prev_z.contains(nz))
                {
                    const auto prev_best = tmp_prev_z.at(nz);
                    if (cmp(prev_best, val))
                    {
                        tmp_prev_z.at(nz) = val;
                    }
                }
                else
                {
                    tmp_prev_z.emplace(nz, val);
                }
            }
        }
        prev_z = tmp_prev_z;
    }
    return prev_z.at(0);
}

int main()
{
    const aoc::StopWatch stop_watch;
    const auto lines = aoc::get_lines("input.txt");
    aoc::assert_equal(solve(lines, std::less<int64_t>()), 49917929934999l); // part 1
    aoc::assert_equal(solve(lines, std::greater<int64_t>()), 11911316711816l); // part 2

    return 0;
}
