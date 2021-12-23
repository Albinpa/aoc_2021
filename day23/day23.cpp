#include "aoc_library.hpp"


namespace ranges = std::ranges;

struct Amiphod
{
    char type;
    uint64_t at_pos;
    
    auto operator<=>(const Amiphod &) const = default;
};

struct State
{
    State(const std::vector<Amiphod>& amis)
    {
        blocked_pos = std::vector<bool>(27, false);
        for (const auto& ami : amis)
        {
            unfinished_ami.push_back(ami);
            blocked_pos.at(ami.at_pos) = true;
        }
    }

    std::vector<bool> blocked_pos;
    uint64_t total_cost = 0;
    std::vector<Amiphod> unfinished_ami;
};

struct Rules
{
    std::vector<std::vector<uint64_t>> graph = {
        {1},
        {0, 2},
        {1, 3, 11},
        {2, 4},
        {3, 5, 15},
        {4, 6},
        {5, 7, 19},
        {6, 8},
        {7, 9, 23},
        {8, 10},
        {9},
        {2, 12},
        {11, 13},
        {12, 14},
        {13},
        {4, 16},
        {15, 17},
        {16, 18},
        {17},
        {6, 20},
        {19, 21},
        {20, 22},
        {21},
        {8, 24},
        {23, 25},
        {24, 26},
        {25},
    };

    std::unordered_map<char, uint64_t> step_cost = {
        {'A', 1ul},
        {'B', 10ul},
        {'C', 100ul},
        {'D', 1000ul},
    };

    std::vector<std::vector<uint64_t>> groups = {
        {11, 12, 13, 14},
        {15, 16, 17, 18},
        {19, 20, 21, 22},
        {23, 24, 25, 26},
    };

    std::unordered_map<char, std::vector<uint64_t>> home = {
        {'A', groups.at(0)},
        {'B', groups.at(1)},
        {'C', groups.at(2)},
        {'D', groups.at(3)},
    };

    std::vector<uint64_t> belonging_group(uint64_t place) const
    {
        if (place < 11)
        {
            return {};
        }
        return groups.at((place - 11) / 4);
    }

    bool is_hallway(uint64_t pos) const
    {
        return pos < 11;
    }

    bool ok_to_stop(uint64_t pos) const
    {
        return pos != 2 && pos != 4 && pos != 6 && pos != 8;
    }

    std::optional<uint64_t>
    found_home(const Amiphod& ami, const std::vector<uint64_t> room, const State& state) const
    {
        const auto free_room_spots = ranges::count_if(room, [&state](const auto place)
                                                            { return !state.blocked_pos[place]; });
        const auto unfinished_my_type = ranges::count_if(state.unfinished_ami, [&ami](const auto & ami2)
                                                                { return ami2.type == ami.type; });
        if (free_room_spots == unfinished_my_type)
        {
            return free_room_spots;
        }
        return std::nullopt;
    }

    std::vector<std::pair<uint64_t, uint64_t>>
    search_move(const Amiphod& ami, const State& state)
    {
        const bool start_in_hallway = is_hallway(ami.at_pos);
        const auto& exiting_group = belonging_group(ami.at_pos);
        std::vector<std::pair<uint64_t, uint64_t>> to_visit = {{ami.at_pos, 0}};
        std::vector<std::pair<uint64_t, uint64_t>> ok_pos = {};
        std::vector<uint64_t> checked;

        while(!to_visit.empty())
        {
            const auto [check, steps] = to_visit.back();
            to_visit.pop_back();
            checked.push_back(check);

            const auto possible_next = graph[check];
            for (const auto & next : possible_next)
            {
                if (state.blocked_pos[next])
                {
                    continue;
                }
                if (ranges::find(checked, next) != checked.end())
                {
                    continue;
                }
                if (!is_hallway(next) && // is room and not on initial exit
                    ranges::find(exiting_group, next) == exiting_group.end())
                {
                    const auto my_room = home.at(ami.type);
                    if (ranges::find(my_room, next) == my_room.end())
                    {
                        continue;
                    }
                    const auto found = found_home(ami, my_room, state);
                    if (!found)
                    {
                        continue;
                    }
                    return {{my_room[found.value() - 1], steps + found.value()}};
                }

                to_visit.push_back({next, steps + 1});
                if (ok_to_stop(next) && 
                    !(start_in_hallway && is_hallway(next)) &&
                    is_hallway(next))
                {
                    ok_pos.push_back({next, steps + 1});
                }
            }
        }
        return ok_pos;
    }

    void best_solution(const State& state, uint64_t& best_cost)
    {
        if (state.unfinished_ami.empty())
        {
            best_cost = state.total_cost;
        }
        // for each unfinished amiphod and for each res from search_move()
            // recurse on updated state if cost is less than prev found lowest cost
        for (const auto& ami : state.unfinished_ami)
        {
            for (const auto& move : search_move(ami, state))
            {
                auto state_copy = state;
                state_copy.total_cost += step_cost.at(ami.type) * move.second;
                if (state_copy.total_cost >= best_cost)
                {
                    continue;
                }
                auto me = ranges::find(state_copy.unfinished_ami, ami);
                state_copy.blocked_pos.at(me->at_pos) = false;
                me->at_pos = move.first;
                state_copy.blocked_pos.at(me->at_pos) = true;

                const auto my_home = home.at(ami.type);
                if (ranges::find(my_home, move.first) != my_home.end())
                {
                    state_copy.unfinished_ami.erase(me);
                }
                best_solution(state_copy, best_cost);
            }
        }
        return;
    }
};

int main()
{
    const aoc::StopWatch stop_watch;
    {
        Rules rules;
        State test_state({{'A', 18}, {'A', 17}, {'A', 16}, {'A', 15}});
        uint64_t best_cost = std::numeric_limits<uint64_t>::max();
        rules.best_solution(test_state, best_cost);
        aoc::assert_equal(best_cost, 28ul);
    }
    
    Rules rules;
    State p2_state({{'D', 11}, {'D', 12}, {'D', 13}, {'B', 14},
                    {'A', 15}, {'C', 16}, {'B', 17}, {'C', 18},
                    {'D', 19}, {'B', 20}, {'A', 21}, {'B', 22},
                    {'C', 23}, {'A', 24}, {'C', 25}, {'A', 26}});
    uint64_t best_cost = std::numeric_limits<uint64_t>::max();
    rules.best_solution(p2_state, best_cost);
    aoc::assert_equal(best_cost, 40954ul); // part 2 solution

    return 0;
}
