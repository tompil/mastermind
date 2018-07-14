#pragma once

#include <stdexcept>
#include <string>


namespace mastermind {

    class incorrect_code_size_error : public std::logic_error {
    public:
        incorrect_code_size_error(size_t solution_size, size_t correct_size)
            : ::std::logic_error{ "Solution\'s size is " + std::to_string(solution_size) + " instead of " + std::to_string(correct_size) }
        {}
    };

    class zero_max_tries_value_error : public std::logic_error {
    public:
        zero_max_tries_value_error()
            : ::std::logic_error{ "Max tries value must be greater than zero" }
        {}
    };

    class indistinct_values_error : public std::logic_error {
    public:
        indistinct_values_error()
            : ::std::logic_error{ "Values of the code must be distinct" }
        {}
    };

}
