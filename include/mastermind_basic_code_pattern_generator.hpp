#pragma once

#include <algorithm>
#include <random>
#include <vector>


namespace mastermind {

    template<typename Item>
    class basic_code_pattern_generator {
    public:
        basic_code_pattern_generator(const std::vector<Item>& code_set);
        std::vector<Item> operator()(size_t pattern_size);
    private:
        std::vector<Item> m_code_set;
        std::random_device rd{};
        std::mt19937 g{ rd() };
    };

    template<typename Item>
    basic_code_pattern_generator<Item>::basic_code_pattern_generator(const std::vector<Item>& code_set)
        : m_code_set{ code_set } {
    };

    template<typename Item>
    std::vector<Item> basic_code_pattern_generator<Item>::operator()(size_t pattern_size) {
        std::shuffle(m_code_set.begin(), m_code_set.end(), g);
        return { m_code_set.begin(), m_code_set.begin() + pattern_size };
    }

}
