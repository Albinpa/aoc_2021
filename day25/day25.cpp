#include "aoc_library.hpp"

struct Ocean
{
    Ocean(const std::vector<std::string> input)
    {
        rows = input.size();
        cols = input.at(0).size();
        map = std::vector<std::vector<char>>(rows, std::vector<char>(cols, '.'));
        for (int y = 0; auto const & line: input)
        {
            for (int x = 0; auto const &c: line)
            {
                map[y][x++] = c;
            }
            y++;
        }
    }

    uint64_t step()
    {
        tmp_map = map;
        uint64_t moved = 0;
        for (int y = 0; auto const & row: map)
        {
            for (int x = 0; auto const &c: row)
            {
                if (c == '>' && row.at((x + 1) % cols) == '.')
                {
                    tmp_map[y][x] = '.';
                    tmp_map[y][(x + 1) % cols] = '>';
                    moved++;
                }
                x++;
            }
            y++;
        }
        map = tmp_map;
        for (int y = 0; auto const & row: map)
        {
            for (int x = 0; auto const &c: row)
            {
                if (c == 'v' && map.at((y + 1) % rows).at(x) == '.')
                {
                    tmp_map[y][x] = '.';
                    tmp_map[(y + 1) % rows][x] = 'v';
                    moved++;
                }
                x++;
            }
            y++;
        }
        map = tmp_map;
        return moved;
    }

    uint64_t find_stale()
    {
        uint64_t counter = 0;
        while (true)
        {
            counter++;
            if(step() == 0)
            {
                break;
            }
        }
        return counter;
    }

    uint64_t rows;
    uint64_t cols;
    std::vector<std::vector<char>> map;
    std::vector<std::vector<char>> tmp_map;
};

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const auto lines = aoc::get_lines("test.txt");
        Ocean ocean(lines);
        aoc::print(ocean.find_stale());
    }
    const auto lines = aoc::get_lines("input.txt");
    Ocean ocean(lines);
    aoc::print(ocean.find_stale());

    return 0;
}
