#pragma once

#include "mastermind_ui.hpp"
#include <functional>
#include <optional>
#include <vector>


namespace mastermind {

    template<typename GameEngine = mastermind_engine<typename Item>>
    class mastermind_game {
    public:
        using ui_t = mastermind_ui<typename GameEngine::value_type>;

        mastermind_game(ui_t& ui, typename GameEngine::code_gen_type code_gen);
        void run();
    private:
        mastermind_ui<typename GameEngine::value_type>& m_ui;
        GameEngine m_me;
    };

    template<typename GameEngine>
    mastermind_game<GameEngine>::mastermind_game(ui_t& ui, typename GameEngine::code_gen_type code_gen)
        : m_me{ code_gen }, m_ui{ ui } {

    }

    template<typename Item>
    void mastermind_game<Item>::run() {
        do {
            m_ui.show_board();
            auto start_params = m_ui.get_start_params();
            m_me.start_game(start_params);

            while (m_me.get_status() == game_status::IN_GAME) {

                auto tries_left = m_me.get_tries_left();
                m_ui.show_tries_left(tries_left);

                auto user_solution = m_ui.ask_for_solution();
                std::optional<game_result> result = m_me.check_solution(user_solution);

                if (result.value().valid) {
                    m_ui.show_winning_message();
                }
                else {
                    m_ui.show_game_result(result.value());

                    if (m_me.get_status() == game_status::ENDED) {
                        m_ui.show_lost_message();
                    }
                }
            }
        } while(m_ui.ask_play_again());
    }

}
