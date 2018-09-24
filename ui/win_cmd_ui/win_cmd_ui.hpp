#pragma once

#include<mastermind_ui.hpp>


class win_cmd_ui : public mastermind::mastermind_ui<int> {
public:
    win_cmd_ui(size_t, size_t);
    virtual void show_board();
    virtual void show_tries_left(size_t);
    virtual void show_game_result(mastermind::game_result);
    virtual void show_lost_message();
    virtual void show_winning_message();
    virtual mastermind::game_start_params get_start_params();
    virtual std::vector<int> ask_for_solution();
    virtual bool ask_play_again();
    virtual ~win_cmd_ui();

private:
    size_t m_max_tries{ 8 };
    size_t m_code_size{ 5 };
};
