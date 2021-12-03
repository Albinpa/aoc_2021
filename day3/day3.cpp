#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ranges>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

std::string most_common(const std::vector<std::string> &input)
{
    const int size = input.at(0).size();
    const float common_threshold = input.size() / 2.0;
    std::string res(size, '-');

    for (int i = 0; i < size; i++)
    {
        const uint64_t ones = ranges::count_if(input, [i](const auto &s)
                                               { return s[i] == '1'; });
        if (ones > common_threshold)
        {
            res[i] = '1';
        }
        else if (ones < common_threshold)
        {
            res[i] = '0';
        }
    }
    return res;
}

uint64_t binstr_to_int(const std::string &s, const char cnr = '1')
{
    uint64_t res = 0;
    for (int i = 0; const auto c : s)
    {
        const auto l = 1 << (s.size() - 1 - i++);
        if (c == cnr)
        {
            res += l;
        }
    }
    return res;
}

uint64_t calc_power_use(const std::string &most_common)
{
    uint64_t gamma_rate = binstr_to_int(most_common, '1');
    uint64_t epsilon_rate = binstr_to_int(most_common, '0');

    return gamma_rate * epsilon_rate;
}

void filter_bit_criteria(std::vector<std::string> &input, uint64_t at_i, bool oxygen)
{
    if (input.size() <= 1)
    {
        return;
    }
    const auto c = most_common(input)[at_i];
    const auto remove = [at_i, oxygen, &c](const auto &s)
    {
        return oxygen != (s[at_i] == c || (c == '-' && s[at_i] == '1'));
    };

    const auto [f, l] = ranges::remove_if(input, remove);
    input.erase(f, l);
}

uint64_t calc_lsr(const std::vector<std::string> &input)
{
    auto oxygen_view = input;
    auto co2_scrub_view = input;

    for (uint64_t i = 0; i < input.at(0).size(); i++)
    {
        filter_bit_criteria(oxygen_view, i, true);
        filter_bit_criteria(co2_scrub_view, i, false);
    }

    return binstr_to_int(oxygen_view[0]) * binstr_to_int(co2_scrub_view[0]);
}

int main()
{
    const auto lines = aoc::get_lines("input.txt");

    const std::vector<std::string> example_input = {"00100", "11110", "10110", "10111", "10101", "01111",
                                                    "00111", "11100", "10000", "11001", "00010", "01010"};

    aoc::assert_equal(calc_power_use(most_common(example_input)), 198ul);
    aoc::assert_equal(calc_power_use(most_common(lines)), 775304ul); // part 1

    aoc::assert_equal(calc_lsr(example_input), 230ul);
    aoc::assert_equal(calc_lsr(lines), 1370737ul); // part 2

    return 0;
}
