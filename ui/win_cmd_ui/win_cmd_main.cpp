#include "mastermind_basic_code_pattern_generator.hpp"
#include "mastermind_engine.hpp"
#include "mastermind_game.hpp"
#include "win_cmd_ui.hpp"

#include <exception>
#include <iostream>
#include <numeric>


int main() {
    std::vector<int> code_set(8);
    std::iota(code_set.begin(), code_set.end(), 1);

    try {
        mastermind::basic_code_pattern_generator code_gen{ code_set };
        win_cmd_ui ui{ 8, 5 };
        mastermind::mastermind_game<mastermind::mastermind_engine<int>> game{ ui, std::bind(&mastermind::basic_code_pattern_generator<int>::operator(), &code_gen, std::placeholders::_1) };
        game.run();
    }
    catch (const std::logic_error& error) {
        std::cout << error.what() << std::endl;
    }
}