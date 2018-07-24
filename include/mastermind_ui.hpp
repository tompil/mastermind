#pragma once

#include "mastermind_utils.hpp"
#include <vector>


namespace mastermind {

    template <typename Item>
    class mastermind_ui {
    public:
        virtual void show_board() = 0;
        virtual void show_tries_left(size_t) = 0;
        virtual void show_game_result(game_result) = 0;
        virtual void show_lost_message() = 0;
        virtual void show_winning_message() = 0;
        virtual game_start_params get_start_params() = 0;
        virtual std::vector<Item> ask_for_solution() = 0;
        virtual bool ask_play_again() = 0;
    };

}
