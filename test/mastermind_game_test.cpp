#include "../include/mastermind_game.hpp"
#include "../include/mastermind_ui.hpp"
#include "../include/mastermind_utils.hpp"
#include "gmock/gmock.h"

using ::testing::_;
using ::testing::Expectation;
using ::testing::Return;

class MockGenerator {
public:
    MOCK_CONST_METHOD0(function_operator, std::vector<int>());
    std::vector<int> operator()(size_t i) const { return function_operator(); }
};

class MastermindUiMock : public mastermind::mastermind_ui<int> {
public:
    MOCK_METHOD0(show_board, void());
    MOCK_METHOD1(show_tries_left, void(size_t));
    MOCK_METHOD1(show_game_result, void(mastermind::game_result));
    MOCK_METHOD0(show_lost_message, void());
    MOCK_METHOD0(show_winning_message, void());
    MOCK_METHOD0(get_start_params, mastermind::game_start_params());
    MOCK_METHOD0(ask_for_solution, std::vector<int>());
    MOCK_METHOD0(ask_play_again, bool());
};

class MastermindEngineHelperMock {
public:
    MOCK_METHOD1(constructor, void(std::function<std::vector<int>(size_t)>));
    MOCK_METHOD0(get_correct_solution, std::optional<std::reference_wrapper<const std::vector<int>>>());
    MOCK_METHOD0(get_tries_left, size_t());
    MOCK_METHOD0(get_status, mastermind::game_status());
    MOCK_METHOD1(start_game, void(mastermind::game_start_params));
    MOCK_METHOD1(check_solution, std::optional<mastermind::game_result>(const std::vector<int>&));
};

class MastermindEngineMock {
public:
    static MastermindEngineHelperMock* mock;

    typedef int value_type;
    typedef std::function<std::vector<int>(size_t)> code_gen_type;

    MastermindEngineMock(code_gen_type gen) { mock->constructor(gen); }
    static std::optional<std::reference_wrapper<const std::vector<int>>> get_correct_solution() { return mock->get_correct_solution(); }
    static size_t get_tries_left() { return mock->get_tries_left(); }
    static mastermind::game_status get_status() { return mock->get_status(); }
    static void start_game(mastermind::game_start_params p) { mock->start_game(p); }
    static std::optional<mastermind::game_result> check_solution(const std::vector<int>& s) { return mock->check_solution(s); }
};

MastermindEngineHelperMock* MastermindEngineMock::mock = nullptr;

class MastermindGameTest : public ::testing::Test {
public:
    std::vector<int> test_solution{ { 1, 2, 3, 4, 5 } };
    MockGenerator generator_mock{};
    MastermindEngineMock::code_gen_type test_generator{ std::bind(&MockGenerator::operator(), &generator_mock, std::placeholders::_1) };
    MastermindUiMock ui_mock{};
    MastermindEngineHelperMock me_helper{};
    

    MastermindGameTest() {
        MastermindEngineMock::mock = &me_helper;
        using ::testing::Return;
        ON_CALL(generator_mock, function_operator()).WillByDefault(Return(test_solution));
    }
};

TEST_F(MastermindGameTest, ShouldCallTheEngineConstructorOnlyOnce) {
    using ::testing::Exactly;
    using ::testing::_;

    EXPECT_CALL(me_helper, constructor(_)).Times(Exactly(1));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };
}
