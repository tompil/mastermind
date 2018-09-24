#include "win_cmd_ui.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>


win_cmd_ui::win_cmd_ui(size_t max_tries, size_t code_size)
    : m_max_tries{ max_tries }, m_code_size{ code_size } {
}

void win_cmd_ui::show_board() {
}

void win_cmd_ui::show_tries_left(size_t tries_left) {
    std::cout << "Tries left: " << tries_left << std::endl;
}

void win_cmd_ui::show_game_result(mastermind::game_result result) {
    std::cout << "CORRECT: " << result.pegs_in_right_place
        << " COLOR: " << result.pegs_in_right_color << std::endl;
}

void win_cmd_ui::show_lost_message() {
    std::cout << "GAME LOST!\n";
}

void win_cmd_ui::show_winning_message() {
    std::cout << "YOU WIN!\n";
}

mastermind::game_start_params win_cmd_ui::get_start_params() {
    return { m_code_size, m_max_tries };
}

std::vector<int> win_cmd_ui::ask_for_solution() {
    std::cout << "Please enter your solution below\n";
    std::vector<int> tmp{};
    tmp.reserve(m_code_size);

    std::string input{};
    std::getline(std::cin, input);
    std::stringstream stream{ input };
    for (int i; stream >> i;) {
        tmp.push_back(i);
    }

    return tmp;
}

bool win_cmd_ui::ask_play_again() {
    std::string input{};
    std::optional<bool> answer{};
    while (!answer.has_value()) {
        std::cout << "Play again? (T/N): ";
        std::cin >> input;
        std::transform(input.begin(), input.end(), input.begin(),
            [](unsigned char c) -> unsigned char { return std::tolower(c); });

        if (input == "y" || input == "yes") {
            answer = true;
        }
        else if (input == "n" || input == "no") {
            answer = false;
        }
        else {
            std::cout << "Please enter 'y' or 'yes' if you want play again or 'n' or 'no' if you don't.";
        }
    }

    return answer.value();
}

win_cmd_ui::~win_cmd_ui() {
}
