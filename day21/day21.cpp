#include "aoc_library.hpp"


inline std::pair<uint64_t, uint64_t> step_player(uint64_t position, uint64_t score, uint64_t steps)
{
    const uint64_t new_pos = (position + steps - 1) % 10 + 1;
    return { new_pos, score + new_pos };
}

struct Answer
{
    uint64_t player_wins;
    uint64_t opponent_wins;

    void operator+=(const Answer& rhs)
    {
        player_wins += rhs.player_wins;
        opponent_wins += rhs.opponent_wins;
    }
};

struct Question
{
    uint64_t player_pos;
    uint64_t player_score;
    uint64_t opponent_pos;
    uint64_t opponent_score;

    auto operator<=>(const Question &) const = default;
};

struct question_hash
{
    std::size_t operator()(const Question &q) const
    {
        return (q.player_pos << 48) + (q.player_score << 32) +
               (q.opponent_pos << 16) + q.opponent_score;
    }
};

inline std::vector<uint64_t> all_universes()
{
    static const std::vector<uint64_t> res =
        { 3, 4, 5, 4, 5, 6, 5, 6, 7, 4, 5, 6, 5, 6, 7, 6, 7, 8, 5, 6, 7, 6, 7, 8, 7, 8, 9 };
    return res;
}

typedef std::unordered_map<Question, Answer, question_hash> SolutionMemo;

Answer universe_wins(const Question& quest, SolutionMemo& solutions)
{
    if (solutions.contains(quest))
    {
        return solutions.at(quest);
    }

    Answer accumulated_wins = {0,0};

    for (const uint64_t steps : all_universes())
    {
        const auto [player_pos, player_score] = step_player(quest.player_pos, quest.player_score, steps);
        if (player_score >= 21)
        {
            accumulated_wins += {1, 0};
        }
        else {
            const auto [opponent_wins, player_wins] = universe_wins(
                {quest.opponent_pos, quest.opponent_score, player_pos, player_score}, solutions);
            accumulated_wins += {player_wins, opponent_wins};
        }
    }
    solutions.emplace(quest, accumulated_wins);
    return accumulated_wins;
}

int main()
{
    const aoc::StopWatch stop_watch;

    SolutionMemo solutions;
    {
        const auto [p1_wins, p2_wins] = universe_wins({4, 0, 8, 0}, solutions);
        aoc::assert_equal(std::max(p1_wins, p2_wins), 444356092776315ul); // test example
    }

    const auto [p1_wins, p2_wins] = universe_wins({8, 0, 9, 0}, solutions);
    aoc::assert_equal(std::max(p1_wins, p2_wins), 346642902541848ul); // part 2 solution

    return 0;
}
