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
    const std::vector<int> TEST_CORRECT_SOLUTION{ { 1, 2, 3, 4, 5 } };
    const std::vector<int> TEST_INCORRECT_SOLUTION{ { 1, 2, 6, 7, 4 } };
    const mastermind::game_start_params TEST_PARAMS{ 8, 5 };
    const mastermind::game_result TEST_NEGATIVE_GAME_RESULT{ false, 2, 1 };
    const mastermind::game_result TEST_POSITIVE_GAME_RESULT{ true, 5, 0 };
    const size_t TEST_TRIES_LEFT{ 3 };

    MockGenerator generator_mock{};
    MastermindEngineMock::code_gen_type test_generator{ std::bind(&MockGenerator::operator(), &generator_mock, std::placeholders::_1) };
    MastermindUiMock ui_mock{};
    MastermindEngineHelperMock me_helper{};
    

    MastermindGameTest() {
        MastermindEngineMock::mock = &me_helper;

        using ::testing::Exactly;

        ON_CALL(ui_mock, ask_play_again()).WillByDefault(Return(false));
        ON_CALL(ui_mock, get_start_params()).WillByDefault(Return(TEST_PARAMS));
        ON_CALL(me_helper, get_tries_left()).WillByDefault(Return(TEST_TRIES_LEFT));
        ON_CALL(generator_mock, function_operator()).WillByDefault(Return(TEST_CORRECT_SOLUTION));
        ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_NEGATIVE_GAME_RESULT));
        EXPECT_CALL(me_helper, constructor(_)).Times(Exactly(1));
    }
};

MATCHER_P(StartGameParamsEquals, params, "") {
    return (arg.code_size == params.code_size) && (arg.max_tries == params.max_tries);
}

MATCHER_P(GameResultEquals, params, "") {
    return (arg.valid == params.valid) &&
        (arg.pegs_in_right_place == params.pegs_in_right_place) &&
        (arg.pegs_in_right_color == params.pegs_in_right_color);
}

TEST_F(MastermindGameTest, ShouldCallTheEngineConstructorOnlyOnce) {
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };
}

TEST_F(MastermindGameTest, ShouldShowBoardAfterRun) {
    using ::testing::Exactly;

    ON_CALL(me_helper, get_status()).WillByDefault(Return(mastermind::game_status::ENDED));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };


    Expectation show_board = EXPECT_CALL(ui_mock, show_board()).Times(Exactly(1));
    Expectation get_status = EXPECT_CALL(me_helper, get_status()).After(show_board);
    EXPECT_CALL(ui_mock, ask_play_again()).After(show_board, get_status);
    game.run();
}

TEST_F(MastermindGameTest, ShouldSetGameStartParamsEnteredByUser) {
    using ::testing::Exactly;

    ON_CALL(me_helper, get_status()).WillByDefault(Return(mastermind::game_status::ENDED));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation get_start_params = EXPECT_CALL(ui_mock, get_start_params()).Times(Exactly(1));
    EXPECT_CALL(me_helper, start_game(StartGameParamsEquals(TEST_PARAMS))).After(get_start_params);
    game.run();
}

TEST_F(MastermindGameTest, ShouldGetStatusBeCalledAfterShowBoardAndStartGame) {
    using ::testing::AtLeast;

    ON_CALL(me_helper, get_status()).WillByDefault(Return(mastermind::game_status::ENDED));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation show_board = EXPECT_CALL(ui_mock, show_board()).Times(AtLeast(1));
    Expectation get_start_params = EXPECT_CALL(ui_mock, get_start_params()).Times(AtLeast(1));
    Expectation start_game = EXPECT_CALL(me_helper, start_game(_)).Times(AtLeast(1));;

    EXPECT_CALL(me_helper, get_status()).After(show_board, get_start_params, start_game);
    game.run();
}

TEST_F(MastermindGameTest, ShouldAskPlayAgainBeCalledAfterGetStatusReturnsNoInGame) {
    ON_CALL(me_helper, get_status()).WillByDefault(Return(mastermind::game_status::ENDED));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation get_status = EXPECT_CALL(me_helper, get_status());
    EXPECT_CALL(ui_mock, ask_play_again()).After(get_status);
    game.run();
}

TEST_F(MastermindGameTest, ShouldShowTriesLeftReceiveTriesLeftNumberFromTheEngine) {
    ON_CALL(me_helper, get_status()).WillByDefault(Return(mastermind::game_status::IN_GAME));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };
    
    Expectation get_tries_left = EXPECT_CALL(me_helper, get_tries_left());
    EXPECT_CALL(ui_mock, show_tries_left(TEST_TRIES_LEFT)).After(get_tries_left);
    game.run();
}

TEST_F(MastermindGameTest, ShouldCheckSolutionPassedByAskForSolution) {
    EXPECT_CALL(me_helper, get_status())
        .WillOnce(Return(mastermind::game_status::IN_GAME))
        .WillRepeatedly(Return(mastermind::game_status::ENDED));
    ON_CALL(ui_mock, ask_for_solution()).WillByDefault(Return(TEST_INCORRECT_SOLUTION));
    ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_NEGATIVE_GAME_RESULT));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation ask_for_solution = EXPECT_CALL(ui_mock, ask_for_solution());
    EXPECT_CALL(me_helper, check_solution(TEST_INCORRECT_SOLUTION)).After(ask_for_solution);
    game.run();
}

TEST_F(MastermindGameTest, ShouldShowWinningMessageAfterPassCorrectSolution) {
    EXPECT_CALL(me_helper, get_status())
        .WillOnce(Return(mastermind::game_status::IN_GAME))
        .WillRepeatedly(Return(mastermind::game_status::ENDED));
    ON_CALL(ui_mock, ask_for_solution()).WillByDefault(Return(TEST_CORRECT_SOLUTION));
    ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_POSITIVE_GAME_RESULT));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation check_solution = EXPECT_CALL(me_helper, check_solution(_));
    EXPECT_CALL(ui_mock, show_winning_message()).After(check_solution);
    game.run();
}

TEST_F(MastermindGameTest, ShouldAskPlayAgainAfterShowWinningMessage) {
    EXPECT_CALL(me_helper, get_status())
        .WillOnce(Return(mastermind::game_status::IN_GAME))
        .WillRepeatedly(Return(mastermind::game_status::ENDED));
    ON_CALL(ui_mock, ask_for_solution()).WillByDefault(Return(TEST_CORRECT_SOLUTION));
    ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_POSITIVE_GAME_RESULT));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation show_winning_message = EXPECT_CALL(ui_mock, show_winning_message());
    EXPECT_CALL(ui_mock, ask_play_again()).After(show_winning_message);
    game.run();
}

TEST_F(MastermindGameTest, ShouldShowStatusAfterPassIncorrectSolution) {
    EXPECT_CALL(me_helper, get_status())
        .WillOnce(Return(mastermind::game_status::IN_GAME))
        .WillRepeatedly(Return(mastermind::game_status::ENDED));
    ON_CALL(ui_mock, ask_for_solution()).WillByDefault(Return(TEST_INCORRECT_SOLUTION));
    ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_NEGATIVE_GAME_RESULT));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation check_solution = EXPECT_CALL(me_helper, check_solution(_));
    EXPECT_CALL(ui_mock, show_game_result(GameResultEquals(TEST_NEGATIVE_GAME_RESULT))).After(check_solution);
    game.run();
}

TEST_F(MastermindGameTest, ShouldShowLostMessageAfterPassIncorrectSolutionAtLastTry) {
    EXPECT_CALL(me_helper, get_status())
        .WillOnce(Return(mastermind::game_status::IN_GAME))
        .WillRepeatedly(Return(mastermind::game_status::ENDED));
    ON_CALL(ui_mock, ask_for_solution()).WillByDefault(Return(TEST_INCORRECT_SOLUTION));
    ON_CALL(me_helper, check_solution(_)).WillByDefault(Return(TEST_NEGATIVE_GAME_RESULT));
    mastermind::mastermind_game<MastermindEngineMock> game{ ui_mock, test_generator };

    Expectation check_solution = EXPECT_CALL(me_helper, check_solution(_));
    EXPECT_CALL(ui_mock, show_lost_message()).After(check_solution);
    game.run();
}
