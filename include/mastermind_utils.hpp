#pragma once


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

}
