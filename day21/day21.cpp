#include "aoc_library.hpp"

struct Player
{
    uint64_t pos;
    uint64_t score = 0;
    void move(uint64_t steps)
    {
        pos = (pos + steps) % 10;
        score += position();
    }
    uint64_t position()
    {
        return pos + 1;
    }
    bool won()
    {
        return score >= 1000;
    }
};

struct Die
{
    uint64_t internal_counter = 0;
    uint64_t roll()
    {
        return ++internal_counter;
    }
    uint64_t three_rolls()
    {
        uint64_t res = 0;
        for (int i = 0; i < 3; i++)
        {
            res += roll();
        }
        return res;
    }
};

uint64_t practice_game()
{
    Die die;
    Player p1 = {.pos = 8 - 1};
    Player p2 = {.pos = 9 - 1};

    while (true)
    {
        p1.move(die.three_rolls());
        if (p1.won())
        {
            aoc::print("p1 won, res: ", die.internal_counter * p2.score);
            break;
        }
        p2.move(die.three_rolls());
        if (p2.won())
        {
            aoc::print("p2 won, res: ", die.internal_counter * p1.score);
            break;
        }
    }
    return 0;
}

int main()
{
    const aoc::StopWatch stop_watch;
    practice_game();
    return 0;
}
