#pragma once


#include "mastermind_exceptions.hpp"
#include "mastermind_utils.hpp"
#include <functional>
#include <optional>
#include <vector>


namespace mastermind {

    template <typename Item>
    class mastermind_engine {
    public:
        mastermind_engine() = delete;
        explicit mastermind_engine(std::function<std::vector<Item>(size_t)> pattern_generator);
        mastermind_engine(const mastermind_engine<Item>&) = delete;
        mastermind_engine<Item>& operator=(const mastermind_engine<Item>&) = delete;
        mastermind_engine(mastermind_engine<Item>&& me);
        mastermind_engine<Item>& operator=(mastermind_engine<Item>&& me);

        std::optional<std::reference_wrapper<const std::vector<Item>>> get_correct_solution() const;
        size_t get_tries_left() const { return m_tries_left; }
        game_status get_status() const { return m_status; }

        void start_game(size_t code_size, size_t max_tries);
        std::optional<game_result> check_solution(const std::vector<Item> s);

        ~mastermind_engine() = default;

    private:
        std::function<std::vector<Item>(size_t)> m_code_generator;
        game_status m_status{ game_status::NOT_INITIALIZED };
        std::vector<Item> m_solution{};
        size_t m_tries_left{ 0 };
        game_result m_current_result{ false, 0, 0 };

        std::optional<game_result> get_game_result(const std::vector<Item> &s);
        game_result compute_game_result(const std::vector<Item> &s) const;
    };

    template <typename Item>
    mastermind_engine<Item>::mastermind_engine(std::function<std::vector<Item>(size_t)> pattern_generator)
        : m_code_generator{ pattern_generator } {
    }

    template <typename Item>
    mastermind_engine<Item>::mastermind_engine(mastermind_engine&& me) {
        this->operator=(std::move(me));
    }

    template <typename Item>
    mastermind_engine<Item>& mastermind_engine<Item>::operator=(mastermind_engine&& me) {
        m_code_generator = me.m_code_generator;
        m_status = me.m_status;
        m_tries_left = me.m_tries_left;
        m_solution = std::move(me.m_solution);
        m_current_result = me.m_current_result;

        me.m_status = game_status::NOT_INITIALIZED;
        me.m_tries_left = 0;
        me.m_current_result = { false, 0, 0 };
        return *this;
    }

    template <typename Item>
    std::optional<std::reference_wrapper<const std::vector<Item>>> mastermind_engine<Item>::get_correct_solution() const {
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

        m_solution = std::move(m_code_generator(code_size));
        m_tries_left = max_tries;
        m_status = game_status::IN_GAME;
    }

    template <typename Item>
    std::optional<game_result> mastermind_engine<Item>::check_solution(const std::vector<Item> s) {
        if (s.size() != m_solution.size()) {
            throw mastermind::incorrect_solution_size_error{ s.size(), m_solution.size() };
        }

        return (m_status == game_status::NOT_INITIALIZED) ? std::nullopt : get_game_result(s);
    }

    template<typename Item>
    std::optional<game_result> mastermind_engine<Item>::get_game_result(const std::vector<Item> &s)
    {
        if (m_status == game_status::IN_GAME) {
            m_current_result = compute_game_result(s);

            if (m_current_result.valid) {
                m_tries_left = 0;
            }
            else {
                --m_tries_left;
            }
        }

        if (m_tries_left == 0) {
            m_status = game_status::ENDED;
        }

        return m_current_result;
    }

    template<typename Item>
    game_result mastermind_engine<Item>::compute_game_result(const std::vector<Item> &s) const {
        size_t in_place{ 0 };
        size_t in_color{ 0 };

        for (size_t i = 0; i < s.size(); ++i) {
            for (size_t j = 0; j < m_solution.size(); ++j) {
                if (s[i] == m_solution[j]) {
                    if (i == j) {
                        ++in_place;
                    }
                    else {
                        ++in_color;
                    }
                    break;
                }
            }
        }

        return game_result{ (in_place == m_solution.size()), in_place, in_color };
    }
}
