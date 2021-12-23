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
        blocked_pos = std::vector<bool>(19, false);
        for (const auto& ami : amis)
        {
            unfinished_ami.push_back(ami);
            blocked_pos.at(ami.at_pos) = true;
        }
        total_cost = 0;
    }

    // blocked positions
    std::vector<bool> blocked_pos;

    uint64_t total_cost;

    // list of unfinished amiphods
    std::vector<Amiphod> unfinished_ami;
};

struct Rules
{
    // where can i go from each spot
    std::vector<std::vector<uint64_t>> graph = {
        {1},
        {0, 2},
        {1, 3, 11},
        {2, 4},
        {3, 5, 13},
        {4, 6},
        {5, 7, 15},
        {6, 8},
        {7, 9, 17},
        {8, 10},
        {9},
        {2, 12},
        {11},
        {4, 14},
        {13},
        {6, 16},
        {15},
        {8, 18},
        {17}
    };

    // cost of moving per type
    std::unordered_map<char, uint64_t> step_cost = {
        {'A', 1ul},
        {'B', 10ul},
        {'C', 100ul},
        {'D', 1000ul},
    };

    std::unordered_map<char, std::vector<uint64_t>> home = {
        {'A', {11, 12}},
        {'B', {13, 14}},
        {'C', {15, 16}},
        {'D', {17, 18}},
    };

    // is this spot a corridor or hallway
    bool is_hallway(uint64_t pos) const
    {
        return pos < 11;
    }

    bool ok_to_stop(uint64_t pos) const
    {
        return pos != 2 && pos != 4 && pos != 6 && pos != 8;
    }
    
    std::vector<std::pair<uint64_t, uint64_t>> search_move(const Amiphod& ami, const State& state)
    {
        const bool start_in_hallway = is_hallway(ami.at_pos);
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
                if (!is_hallway(next) && steps != 0) // room and not on initial exit
                {
                    const auto my_room = home.at(ami.type);
                    if (ranges::find(my_room, next) == my_room.end())
                    {
                        continue;
                    }
                    if (state.blocked_pos.at(my_room[1]))
                    {
                        const auto unfinished_my_type = 
                            ranges::count_if(state.unfinished_ami, [&ami](const auto & ami2)
                            { return ami2.type == ami.type; });
                        if (unfinished_my_type == 2)
                        {
                            continue;
                        }
                        // filled room!
                        return {{next, steps + 1}};
                    }
                    else
                    {   // found path to deep inside my room!
                       // aoc::print("found path to deep inside my room! ", ami.type, " ", next);
                        return {{my_room[1], steps + 2}};
                    }
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

    uint64_t best_cost = std::numeric_limits<uint64_t>::max();

    void best_solution(const State& state)
    {
        if (state.unfinished_ami.empty())
        {
            aoc::print("updating best cost ", state.total_cost);
            best_cost = state.total_cost;
        }
        // for each unfinished amiphod
        // for each res from search_move()
        // recurse on updated state if cost is less than prev found lowest cost
        for (const auto& ami : state.unfinished_ami)
        {
            for (const auto& move : search_move(ami, state))
            {
                //aoc::print("moving ", ami.type, " steps: ", move.second, " to: ", move.first);
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
                    //aoc::print("found home for ", me->type);
                    state_copy.unfinished_ami.erase(me);
                }
                best_solution(state_copy);
            }
        }
        //aoc::print("unsuccessfully finished search. Best cost: ", best_cost);
        return;
    }
};


int main()
{
    const aoc::StopWatch stop_watch;
    {
        Rules rules;
        State test_state({{'B', 11}, {'A', 18}, {'D', 5}, {'D', 17}});
        test_state.blocked_pos[12] = true;
        test_state.blocked_pos[14] = true;
        //rules.best_solution(test_state);
    }
    
    Rules rules;
    State p1_state({{'D', 11},{'B', 12}, {'A', 13},{'C', 14},{'D', 15},{'B', 16},{'C', 17},{'A', 18}});
    rules.best_solution(p1_state);

    // things that do not work for p2:
        // find home is hardcoded to depth 2
        // unfinished ami which is starting in his own room will get stuck?
            // hardcoded initial exit with step != 0 

    return 0;
}
