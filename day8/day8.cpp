#include <numeric>
#include <cmath>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

inline uint64_t count_unique_nr_segs(const std::string &note_line)
{
    return ranges::count_if(aoc::split(aoc::split(note_line, " | ")[1]), [](const auto &s)
                            {
                                const auto len = s.size();
                                return len == 2 || len == 3 || len == 4 || len == 7;
                            });
}

uint64_t unique_appears(const std::vector<std::string> &input)
{
    return std::accumulate(input.begin(), input.end(), 0, [](uint64_t acc, const auto &line)
                           { return acc + count_unique_nr_segs(line); });
}

inline bool compare_size(const std::string &first, const std::string &second)
{
    return first.size() < second.size();
};

inline uint8_t to_bits(const std::string &seq)
{
    static const std::unordered_map<char, uint8_t> mapping =
        {
            {'a', 1 << 6},
            {'b', 1 << 5},
            {'c', 1 << 4},
            {'d', 1 << 3},
            {'e', 1 << 2},
            {'f', 1 << 1},
            {'g', 1},
        };
    return std::accumulate(seq.begin(), seq.end(), 0, [](uint8_t acc, const auto c)
                           { return acc + mapping.at(c); });
}

inline uint64_t output_value(const std::string &line)
{
    const auto split = aoc::split(line, " | ");
    auto uniques = aoc::split(split[0]);
    const auto output = aoc::split(split[1]);

    ranges::sort(uniques, compare_size);

    const uint8_t cf = to_bits(uniques[0]);
    const uint8_t acf = to_bits(uniques[1]);
    const uint8_t bcdf = to_bits(uniques[2]);
    const uint8_t adg = to_bits(uniques[3]) & to_bits(uniques[4]) & to_bits(uniques[5]);
    const uint8_t cde = (to_bits(uniques[6]) & to_bits(uniques[7]) & to_bits(uniques[8])) ^ 0x7F;

    const uint8_t a = cf ^ acf;
    const uint8_t d = a ^ ((a | bcdf) & adg);
    const uint8_t g = adg ^ (a | d);
    const uint8_t c = cf & cde;
    const uint8_t e = cde ^ (c | d);
    const uint8_t f = cf ^ c;
    const uint8_t b = bcdf ^ (cf | d);

    const std::unordered_map<uint8_t, uint64_t> seq_to_nr =
        {
            {a | b | c | e | f | g, /*    */ 0},
            {c | f, /*                    */ 1},
            {a | c | d | e | g, /*        */ 2},
            {a | c | d | f | g, /*        */ 3},
            {b | c | d | f, /*            */ 4},
            {a | b | d | f | g, /*        */ 5},
            {a | b | d | e | f | g, /*    */ 6},
            {a | c | f, /*                */ 7},
            {a | b | c | d | e | f | g, /**/ 8},
            {a | b | c | d | f | g, /*    */ 9},
        };
    uint64_t sum = 0;
    for (uint64_t i = 1; const auto &seq : output)
    {
        sum += seq_to_nr.at(to_bits(seq)) * std::pow(10, output.size() - i++);
    }
    return sum;
}

uint64_t added_output_values(const std::vector<std::string> &input)
{
    return std::accumulate(input.begin(), input.end(), 0,
                           [](uint64_t acc, const auto &line)
                           { return acc + output_value(line); });
}

int main()
{
    const aoc::StopWatch stop_watch;

    aoc::assert_equal(
        count_unique_nr_segs("be cfbegad cbdgef fgaecd cgeb fdcge agebfd fecdb fabcd edb | fdgacbe cefdb cefbgd gcbe"),
        2ul);

    const auto input = aoc::get_lines("input.txt");

    aoc::assert_equal(unique_appears(input), 237ul); // part 1

    aoc::assert_equal(
        added_output_values({"acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf"}),
        5353ul);

    aoc::assert_equal(added_output_values(aoc::get_lines("test.txt")), 61229ul);
    aoc::assert_equal(added_output_values(input), 1009098ul); // part 2

    return 0;
}
