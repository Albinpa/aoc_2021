#include <numeric>
#include <algorithm>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

std::string transform_to_bin(const std::string &hex)
{
    static const std::unordered_map<char, std::string> m =
        {{'0', "0000"},
         {'1', "0001"},
         {'2', "0010"},
         {'3', "0011"},
         {'4', "0100"},
         {'5', "0101"},
         {'6', "0110"},
         {'7', "0111"},
         {'8', "1000"},
         {'9', "1001"},
         {'A', "1010"},
         {'B', "1011"},
         {'C', "1100"},
         {'D', "1101"},
         {'E', "1110"},
         {'F', "1111"}};

    return std::accumulate(hex.begin(), hex.end(), std::string(),
                           [](auto acc, const auto c)
                           { return acc + m.at(c); });
}

struct Packet
{
    uint64_t length;
    uint64_t version_accumulaed;
    uint64_t value;
};

uint64_t operate(uint64_t type, const std::vector<uint64_t> &values)
{
    if (type == 0)
    {
        return std::accumulate(values.begin(), values.end(), 0ul);
    }
    else if (type == 1)
    {
        return std::accumulate(values.begin(), values.end(), 1ul, std::multiplies<uint64_t>());
    }
    else if (type == 2)
    {
        return std::accumulate(values.begin(), values.end(), std::numeric_limits<uint64_t>::max(),
                               [](const auto min, const auto el)
                               { return el < min ? el : min; });
    }
    else if (type == 3)
    {
        return std::accumulate(values.begin(), values.end(), 0ul,
                               [](const auto max, const auto el)
                               { return el < max ? max : el; });
    }
    else if (type == 5)
    {
        return values.at(0) > values.at(1) ? 1 : 0;
    }
    else if (type == 6)
    {
        return values.at(0) < values.at(1) ? 1 : 0;
    }
    else if (type == 7)
    {
        return values.at(0) == values.at(1) ? 1 : 0;
    }
    return 0;
}

Packet parse_literal(const std::string &s)
{
    const uint64_t version = std::stoul(s.substr(0, 3), nullptr, 2);

    uint64_t p = 6;
    bool done = false;
    std::vector<std::string> groups;
    while (!done)
    {
        std::string group = s.substr(p, 5);
        groups.push_back(group.substr(1));
        p += 5;
        done = group[0] == '0';
    }

    uint64_t value = 0;
    for (int i = 0; const auto &group : groups | ranges::views::reverse)
    {
        value += std::stoul(group, nullptr, 2) << 4 * i++;
    }

    return {p, version, value};
}

Packet parse_packet(const std::string &s)
{
    const uint64_t version = std::stoul(s.substr(0, 3), nullptr, 2);
    const uint64_t type = std::stoul(s.substr(3, 3), nullptr, 2);

    if (type == 4ul)
    {
        return parse_literal(s);
    }

    uint64_t version_acc = version;
    std::vector<uint64_t> values;
    uint64_t packet_length = 0;

    if (s[6] == '0')
    {
        const uint64_t initial_offset = 7 + 15;
        const uint64_t expected_length = std::stoul(s.substr(7, 15), nullptr, 2) + initial_offset;
        packet_length = initial_offset;
        while (packet_length < expected_length)
        {
            const auto [l, v, x] = parse_packet(s.substr(packet_length));
            packet_length += l;
            version_acc += v;
            values.push_back(x);
        }
    }
    else
    {
        const uint64_t nr_sub_packets = std::stoul(s.substr(7, 11), nullptr, 2);
        packet_length = 7 + 11;
        for (uint64_t i = 0; i < nr_sub_packets; i++)
        {
            const auto [l, v, x] = parse_packet(s.substr(packet_length));
            packet_length += l;
            version_acc += v;
            values.push_back(x);
        }
    }
    return {packet_length, version_acc, operate(type, values)};
}

int main()
{
    const aoc::StopWatch stop_watch;
    {
        auto [length, version_acc, value] = parse_packet(transform_to_bin("D2FE28"));
        aoc::assert_equal(length, 21ul);
        aoc::assert_equal(value, 2021ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("38006F45291200"));
        aoc::assert_equal(version_acc, 9ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("EE00D40C823060"));
        aoc::assert_equal(version_acc, 14ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("8A004A801A8002F478"));
        aoc::assert_equal(version_acc, 16ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("620080001611562C8802118E34"));
        aoc::assert_equal(version_acc, 12ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("C0015000016115A2E0802F182340"));
        aoc::assert_equal(version_acc, 23ul);
    }
    {
        const auto [length, version_acc, x] = parse_packet(transform_to_bin("A0016C880162017C3686B18A3D4780"));
        aoc::assert_equal(version_acc, 31ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("C200B40A82"));
        aoc::assert_equal(value, 3ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("04005AC33890"));
        aoc::assert_equal(value, 54ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("880086C3E88112"));
        aoc::assert_equal(value, 7ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("CE00C43D881120"));
        aoc::assert_equal(value, 9ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("D8005AC2A8F0"));
        aoc::assert_equal(value, 1ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("F600BC2D8F"));
        aoc::assert_equal(value, 0ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("9C005AC2F8F0"));
        aoc::assert_equal(value, 0ul);
    }
    {
        const auto [length, version_acc, value] = parse_packet(transform_to_bin("9C0141080250320F1802104A08"));
        aoc::assert_equal(value, 1ul);
    }

    aoc::assert_equal(operate(0, {9}), 9ul);
    aoc::assert_equal(operate(1, {9}), 9ul);
    aoc::assert_equal(operate(5, {9, 9}), 0ul);
    aoc::assert_equal(operate(6, {9, 9}), 0ul);
    aoc::assert_equal(operate(7, {9, 9}), 1ul);

    const auto lines = aoc::get_lines("input.txt");

    const auto [length, version_acc, value] = parse_packet(transform_to_bin(lines[0]));
    aoc::assert_equal(version_acc, 877ul);    // part 1
    aoc::assert_equal(value, 194435634456ul); // part 2

    return 0;
}
