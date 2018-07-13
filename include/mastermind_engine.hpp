#pragma once

#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace mastermind {

    class incorrect_solution_size_error : public std::logic_error {
    public:
        incorrect_solution_size_error(size_t solution_size, size_t correct_size)
            : ::std::logic_error{ "Solution\'s size is " + std::to_string(solution_size) + " instead of " + std::to_string(correct_size)} 
        {}
    };

    class zero_max_tries_value_error : public std::logic_error {
    public:
        zero_max_tries_value_error()
            : ::std::logic_error{ "Max tries value must be greater than zero" }
        {}
    };

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

    template <typename Item>
    class mastermind_engine {
    public:
        mastermind_engine() = delete;
        explicit mastermind_engine(std::function<std::vector<Item>(size_t)> pattern_generator);
        mastermind_engine(const mastermind_engine<Item>&) = delete;
        mastermind_engine<Item>& operator=(const mastermind_engine<Item>&) = delete;
        mastermind_engine(mastermind_engine<Item>&& me);
        mastermind_engine<Item>& operator=(mastermind_engine<Item>&& me);

        std::optional<std::reference_wrapper<const std::vector<Item>>> correct_answer() const;
        size_t get_tries_left() const { return m_tries_left; }
        game_status get_status() const { return m_status; }

        void start_game(size_t code_size, size_t max_tries);
        std::optional<game_result> check_solution(std::vector<Item> s);

        ~mastermind_engine() = default;

    private:
        std::function<std::vector<Item>(size_t)> m_pattern_generator;
        game_status m_status{ game_status::NOT_INITIALIZED };
        std::vector<Item> m_solution{};
        size_t m_tries_left{ 0 };
        bool m_valid{ false };
        size_t m_in_place{ 0 };
        size_t m_in_color{ 0 };
    };

    template <typename Item>
    mastermind_engine<Item>::mastermind_engine(std::function<std::vector<Item>(size_t)> pattern_generator)
        : m_pattern_generator{ pattern_generator } {
    }

    template <typename Item>
    mastermind_engine<Item>::mastermind_engine(mastermind_engine&& me)
        : m_pattern_generator{ me.m_pattern_generator },
        m_status{ me.m_status },
        m_tries_left{ me.m_tries_left },
        m_solution{ std::move(me.m_solution) },
        m_valid{ me.m_valid },
        m_in_place{ me.m_in_place },
        m_in_color{ me.m_in_color }
    {
        me.m_status = game_status::NOT_INITIALIZED;
        me.m_tries_left = 0;
        me.m_valid = false;
        me.m_in_place = 0;
        me.m_in_color = 0;
    }

    template <typename Item>
    mastermind_engine<Item>& mastermind_engine<Item>::operator=(mastermind_engine&& me) {
        m_pattern_generator = me.m_pattern_generator;
        m_status = me.m_status;
        m_tries_left = me.m_tries_left;
        m_solution = std::move(me.m_solution);
        m_valid = me.m_valid;
        m_in_place = me.m_in_place;
        m_in_color = me.m_in_color;

        me.m_status = game_status::NOT_INITIALIZED;
        me.m_tries_left = 0;
        me.m_valid = false;
        me.m_in_place = 0;
        me.m_in_color = 0;
        return *this;
    }

    template <typename Item>
    std::optional<std::reference_wrapper<const std::vector<Item>>> mastermind_engine<Item>::correct_answer() const {
        if (m_status == game_status::NOT_INITIALIZED) {
            return std::nullopt;
        }
        else {
            return std::cref(m_solution);
        }
    }

    template <typename Item>
    void mastermind_engine<Item>::start_game(size_t code_size, size_t max_tries) {
        if (max_tries == 0) {
            throw zero_max_tries_value_error();
        }

        m_solution = std::move(m_pattern_generator(code_size));
        m_tries_left = max_tries;
        m_status = game_status::IN_GAME;
    }

    template <typename Item>
    std::optional<game_result> mastermind_engine<Item>::check_solution(std::vector<Item> s) {
        if (s.size() != m_solution.size()) {
            throw mastermind::incorrect_solution_size_error{ s.size(), m_solution.size() };
        }

        if (m_status == game_status::NOT_INITIALIZED) {
            return std::nullopt;
        }


        if (m_status == game_status::IN_GAME) {
            m_in_place = 0;
            m_in_color = 0;

            for (size_t i = 0; i < s.size(); ++i) {
                for (size_t j = 0; j < m_solution.size(); ++j) {
                    if (s[i] == m_solution[j]) {
                        if (i == j) {
                            ++m_in_place;
                        }
                        else {
                            ++m_in_color;
                        }
                        break;
                    }
                }
            }

            m_valid = (m_in_place == m_solution.size());
            --m_tries_left;
        }

        if (m_valid) {
            m_tries_left = 0;
        }

        if (m_tries_left == 0) {
            m_status = game_status::ENDED;
        }

        return game_result{
            m_valid,
            m_in_place,
            m_in_color
        };
    }

}
