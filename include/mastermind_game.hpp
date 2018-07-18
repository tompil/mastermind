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

    }

}
