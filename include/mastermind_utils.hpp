#pragma once


#include <vector>


namespace mastermind {

    enum class game_status {
        NOT_INITIALIZED,
        IN_GAME,
        ENDED
    };

    struct game_result {
        bool valid;
        size_t pegs_in_right_place;
        size_t pegs_in_right_color;
    };

    template<typename Item>
    bool are_all_values_different(const std::vector<Item>& v) {
        for (size_t i{ 0 }; i < v.size(); ++i) {
            for (size_t j{ i + 1 }; j < v.size(); ++j) {
                if (v[i] == v[j]) {
                    return false;
                }
            }
        }
        return true;
    }

}
