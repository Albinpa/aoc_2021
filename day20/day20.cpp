#include <unordered_set>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

struct Point
{
    int64_t x;
    int64_t y;

    auto operator<=>(const Point &) const = default;

    Point operator+(const Point &rhs) const
    {
        return {
            x + rhs.x,
            y + rhs.y};
    }

    Point operator-(const Point &rhs) const
    {
        return {
            x - rhs.x,
            y - rhs.y};
    }

    friend std::ostream &operator<<(std::ostream &stream, const Point &p)
    {
        stream << "{" << p.x << ", " << p.y << "}";
        return stream;
    };
};

struct point_hash
{
    std::size_t operator()(const Point &pos) const
    {
        return (pos.x * 0x1f1f1f1f) ^ pos.y;
    }
};

class Trench
{
public:
    Trench(const std::vector<std::string> &input)
    {
        image_enh_alg_ = input[0];
        for (int64_t y = 0; const auto &line : input | ranges::views::drop(2))
        {
            for (int64_t x = 0; const auto c : line)
            {
                if (c == '#')
                {
                    map_.insert({x, y});
                }
                x++;
            }
            y++;
        }
        smallest_ = {0, 0};
        largest_ = {(int64_t)input[2].size(), (int64_t)input.size() - 2};
    }

    void enhance(uint64_t times = 2)
    {
        for (uint64_t i = steps_; i < steps_ + times; i++)
        {
            const bool even = i % 2 == 0;

            Point new_smallest = {std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max()};
            Point new_largest = {std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min()};
            smallest_ = smallest_ - extend_with_;
            largest_ = largest_ + extend_with_;

            for (int64_t x = smallest_.x; x <= largest_.x; x++)
            {
                for (int64_t y = smallest_.y; y <= largest_.y; y++)
                {
                    Point p = {x, y};
                    for (uint64_t i = 0; const auto &trans : square3x3_)
                    {
                        if (map_.contains(p + trans))
                        {
                            bin_str_[i++] = even ? '1' : '0';
                        }
                        else
                        {
                            bin_str_[i++] = even ? '0' : '1';
                        }
                    }
                    const char cmp_for_insert = even ? '.' : '#';
                    const uint64_t index = std::stoul(bin_str_, nullptr, 2);
                    if (image_enh_alg_[index] == cmp_for_insert)
                    {
                        tmp_.insert(p);
                        new_smallest = {std::min(new_smallest.x, p.x), std::min(new_smallest.y, p.y)};
                        new_largest = {std::max(new_largest.x, p.x), std::max(new_largest.y, p.y)};
                    }
                }
            }
            map_ = tmp_;
            tmp_.clear();
            smallest_ = new_smallest;
            largest_ = new_largest;
        }
        steps_ += times;
    }

    void print()
    {
        std::vector<std::vector<char>> pmap =
            std::vector<std::vector<char>>(largest_.y + 1 - smallest_.y,
                                           std::vector<char>(largest_.x + 1 - smallest_.x, '.'));
        for (const auto &p : map_)
        {
            pmap.at(p.y - smallest_.y).at(p.x - smallest_.y) = '#';
        }
        aoc::print(pmap);
    }

    uint64_t count() const
    {
        return map_.size();
    }

private:
    uint64_t steps_ = 0;
    std::string image_enh_alg_;
    Point smallest_;
    Point largest_;
    std::unordered_set<Point, point_hash> map_;
    std::unordered_set<Point, point_hash> tmp_;
    std::vector<Point> square3x3_ = {
        {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    std::string bin_str_ = "000000000";
    Point extend_with_ = {1, 1};
};

int main()
{
    const aoc::StopWatch stop_watch;
    {
        const auto lines = aoc::get_lines("test.txt");
        Trench trench(lines);
        trench.enhance(1ul);
        aoc::assert_equal(trench.count(), 6ul);
        trench.enhance(1ul);
        aoc::assert_equal(trench.count(), 12ul);
    }

    const auto lines = aoc::get_lines("input.txt");
    Trench trench(lines);
    trench.enhance(2ul);
    aoc::assert_equal(trench.count(), 5563ul); // part 1
    trench.enhance(48ul);
    aoc::assert_equal(trench.count(), 19743ul); // part 2
    return 0;
}
