#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <ranges>
#include <iterator>
#include <numeric>
#include <set>
#include <cassert>
#include "aoc_library.hpp"

namespace ranges = std::ranges;

class Board
{
public:
    Board(uint64_t id)
        : id_(id)
    {
    }

    void add(const std::vector<uint64_t> &row)
    {
        const auto v = row | ranges::views::transform([](uint64_t nr)
                                                      { return std::make_pair(nr, false); });
        rows_.push_back(std::vector<std::pair<uint64_t, bool>>(v.begin(), v.end()));
    }

    std::optional<uint64_t> mark(uint64_t mark_nr)
    {
        for (auto &row : rows_)
        {
            for (uint64_t i = 0; auto &[nr, marked] : row)
            {
                if (mark_nr == nr)
                {
                    marked = true;
                    if (bingo_row(row) || bingo_column(i))
                    {
                        return calc_score(mark_nr);
                    }
                }
                i++;
            }
        }
        return std::nullopt;
    }

    uint64_t id_;

private:
    bool bingo_row(const std::vector<std::pair<uint64_t, bool>> &row) const
    {
        return ranges::all_of(row, [](const auto el)
                              { return el.second; });
    }

    bool bingo_column(uint64_t col_nr) const
    {
        return ranges::all_of(rows_, [col_nr](const auto row)
                              { return row[col_nr].second; });
    }

    uint64_t calc_score(uint64_t called_nr) const
    {
        uint64_t score = 0;
        for (const auto row : rows_)
        {
            score += std::accumulate(row.begin(), row.end(), 0, [](uint64_t acc, const auto &el)
                                     {
                                         const auto &[nr, marked] = el;
                                         return acc += marked ? 0 : nr;
                                     });
        }
        return score * called_nr;
    }

    std::vector<std::vector<std::pair<uint64_t, bool>>> rows_;
};

std::vector<Board> build_boards(const std::vector<std::string> &lines)
{
    std::vector<Board> boards;
    uint64_t ids = 0;

    uint64_t last_empty_row = 1;
    for (uint64_t i = 2; i < lines.size(); i++)
    {
        if (lines[i] == "")
        {
            Board board(ids++);
            for (uint64_t row_i = last_empty_row + 1; row_i < i; row_i++)
            {
                board.add(aoc::transform_int(aoc::split(lines[row_i], " ")));
            }
            boards.push_back(board);
            last_empty_row = i;
        }
    }

    return boards;
}

int main()
{
    Board test_board(0);
    test_board.add({1, 2, 3});
    test_board.add({4, 5, 6});
    test_board.add({7, 8, 9});
    Board copy = test_board;

    assert(!test_board.mark(2));
    assert(!test_board.mark(5));
    assert(test_board.mark(8));

    assert(!copy.mark(2));
    assert(!copy.mark(1));
    assert(copy.mark(3));

    // Start playing bingo!
    const auto lines = aoc::get_lines("input.txt");
    auto boards = build_boards(lines);

    std::set<uint64_t> winners;
    std::vector<uint64_t> winner_scores;
    for (auto nr : aoc::transform_int(aoc::split(lines[0], ",")))
    {
        for (auto &board : boards)
        {
            if (winners.contains(board.id_))
                continue;

            if (const auto score = board.mark(nr); score)
            {
                winners.emplace(board.id_);
                winner_scores.push_back(score.value());
            }
        }
    }
    aoc::print("First winner: ", winner_scores.front()); // part 1
    aoc::print("Last winner: ", winner_scores.back());   // part 2

    return 0;
}
