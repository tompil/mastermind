#include "../include/mastermind_engine.hpp"
#include "gmock/gmock.h"
#include <type_traits>


using ::testing::Exactly;


class MockGenerator {
public:
    MOCK_CONST_METHOD0(function_operator, std::vector<int>());
    std::vector<int> operator()(size_t i) const { return function_operator(); }
};


class MastermindEngineTest : public ::testing::Test {
public:
    const size_t PATTERN_SIZE{ 5 };
    std::vector<int> test_solution;
    MockGenerator generator_mock;
    mastermind::mastermind_engine<int>::code_gen_type test_generator;
    mastermind::mastermind_engine<int> engine;

    MastermindEngineTest() :
        test_solution{ { 1, 2, 3, 4, 5 } },
        generator_mock{},
        test_generator{ std::bind(&MockGenerator::operator(), &generator_mock, std::placeholders::_1) },
        engine{ test_generator }
    {
        using ::testing::Return; 
        ON_CALL(generator_mock, function_operator()).WillByDefault(Return(test_solution));
    }
};


TEST_F(MastermindEngineTest, ShouldNotBeDefaultConstructible) {
    EXPECT_FALSE(std::is_default_constructible<mastermind::mastermind_engine<int>>::value);
}

TEST_F(MastermindEngineTest, ShouldNotBeCopyConstructible) {
    EXPECT_FALSE(std::is_copy_constructible<mastermind::mastermind_engine<int>>::value);
}

TEST_F(MastermindEngineTest, ShouldBeMoveConstructible) {
    EXPECT_TRUE(std::is_move_constructible<mastermind::mastermind_engine<int>>::value);
}

TEST_F(MastermindEngineTest, ShouldGameObjectBeCorrectlyMoveConstructed) {
    engine.start_game({ PATTERN_SIZE, 8 });
    engine.check_solution(std::vector<int>{ { 1, 3, 4, 6, 5 } });
    auto origin_tries_left = engine.get_tries_left();
    auto origin_status = engine.get_status();
    auto origin_correct_solution = engine.get_correct_solution().value().get();

    mastermind::mastermind_engine moved_game{ std::move(engine) };
    auto current_correct_solution = moved_game.get_correct_solution();

    EXPECT_EQ(moved_game.get_tries_left(), origin_tries_left);
    EXPECT_EQ(moved_game.get_status(), origin_status);
    ASSERT_TRUE(current_correct_solution.has_value());
    EXPECT_EQ(current_correct_solution.value().get(), origin_correct_solution);

    EXPECT_EQ(engine.get_tries_left(), 0);
    EXPECT_EQ(engine.get_status(), mastermind::game_status::NOT_INITIALIZED);
    EXPECT_EQ(engine.get_correct_solution(), std::nullopt);
}

TEST_F(MastermindEngineTest, ShouldGameObjectBeCorrectlyMoveAssigned) {
    engine.start_game({ PATTERN_SIZE, 8 });
    engine.check_solution(std::vector<int>{ { 1, 3, 4, 6, 5 } });
    auto origin_tries_left = engine.get_tries_left();
    auto origin_status = engine.get_status();
    auto origin_correct_solution = engine.get_correct_solution().value().get();

    mastermind::mastermind_engine moved_game{ test_generator };
    moved_game = std::move(engine);
    auto current_correct_solution = moved_game.get_correct_solution();

    EXPECT_EQ(moved_game.get_tries_left(), origin_tries_left);
    EXPECT_EQ(moved_game.get_status(), origin_status);
    ASSERT_TRUE(current_correct_solution.has_value());
    EXPECT_EQ(current_correct_solution.value().get(), origin_correct_solution);

    EXPECT_EQ(engine.get_tries_left(), 0);
    EXPECT_EQ(engine.get_status(), mastermind::game_status::NOT_INITIALIZED);
    EXPECT_EQ(engine.get_correct_solution(), std::nullopt);
}

TEST_F(MastermindEngineTest, ShouldCorrectAnswerBeNulloptBeforeStart) {
    EXPECT_EQ(engine.get_correct_solution(), std::nullopt);
}

TEST_F(MastermindEngineTest, ShouldCorrectAnswerBeTheSameAsGenerated) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));

    engine.start_game({ PATTERN_SIZE, 8 });
    auto result = engine.get_correct_solution();

    ASSERT_TRUE(result.has_value());
    EXPECT_THAT(result.value().get(), ::testing::ContainerEq(test_solution));
}

TEST_F(MastermindEngineTest, ShouldTriesLeftReturnMaxTriesJustAfterStart) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    const int MAX_TRIES{ 8 };

    engine.start_game({ PATTERN_SIZE, MAX_TRIES });

    EXPECT_EQ(engine.get_tries_left(), MAX_TRIES);
}

TEST_F(MastermindEngineTest, ShouldStartGameThrowIndistinctValuesErrorForPatternWithRepeatedElements) {
    using ::testing::Return;
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    mastermind::mastermind_engine me(test_generator);
    ON_CALL(generator_mock, function_operator()).WillByDefault(Return(std::vector<int>{ { 1, 2, 2, 4, 3 } }));

    EXPECT_THROW(me.start_game({ PATTERN_SIZE, 6 }), mastermind::indistinct_values_error);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionThrowIncorrectCodeSizeErrorForInvalidSizedSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    mastermind::mastermind_engine me(test_generator);
    me.start_game({ PATTERN_SIZE, 6 });

    EXPECT_THROW(me.check_solution(std::vector<int>{ {1, 2, 3, 4, 5, 6} }), mastermind::incorrect_code_size_error);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionThrowIndistinctValuesErrorForCodeWithRepeatedElements) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    mastermind::mastermind_engine me(test_generator);
    me.start_game({ PATTERN_SIZE, 6 });

    EXPECT_THROW(me.check_solution(std::vector<int>{ {1, 2, 3, 5, 5} }), mastermind::indistinct_values_error);
}

TEST_F(MastermindEngineTest, ShouldGetStatusReturnNotInitializedStatusIfGameIsNotEnded) {
    EXPECT_EQ(engine.get_status(), mastermind::game_status::NOT_INITIALIZED);
}

TEST_F(MastermindEngineTest, ShouldGetStatusReturnInGameStatusIfGameIsNotEnded) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));

    engine.start_game({ PATTERN_SIZE, 8 });

    EXPECT_EQ(engine.get_status(), mastermind::game_status::IN_GAME);
}

TEST_F(MastermindEngineTest, ShouldGetStatusReturnEndedStatusWithCorrectAnswer) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));

    engine.start_game({ PATTERN_SIZE, 8 });
    auto result = engine.check_solution(test_solution);

    EXPECT_EQ(engine.get_status(), mastermind::game_status::ENDED);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnEndedStatusAfterLost) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    std::vector<int> incorrect_solution{ { 1, 3, 4, 5, 6 } };
    const size_t MAX_TRIES{ 2 };
    engine.start_game({ PATTERN_SIZE, MAX_TRIES });

    for (size_t i{ 0 }; i < MAX_TRIES; ++i) {
        engine.check_solution(incorrect_solution);
    }

    EXPECT_EQ(engine.get_status(), mastermind::game_status::ENDED);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnFalseValidValueInIncorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto incorrect_answer_result = engine.check_solution(std::vector<int>{ { 1, 3, 4, 6, 5 } });
    ASSERT_TRUE(incorrect_answer_result.has_value());
    EXPECT_FALSE(incorrect_answer_result.value().valid);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnTrueValidValueInCorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto correct_answer_result = engine.check_solution(test_solution);
    ASSERT_TRUE(correct_answer_result.has_value());
    EXPECT_TRUE(correct_answer_result.value().valid);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnCorrectNumberOfPegsInRightPlaceInIncorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto incorrect_answer_result = engine.check_solution(std::vector<int>{ { 1, 3, 4, 6, 5 } });
    ASSERT_TRUE(incorrect_answer_result.has_value());
    EXPECT_EQ(incorrect_answer_result.value().pegs_in_right_place, 2);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnMaxNumberOfPegsInRightPlaceInCorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto correct_answer_result = engine.check_solution(test_solution);
    ASSERT_TRUE(correct_answer_result.has_value());
    EXPECT_EQ(correct_answer_result.value().pegs_in_right_place, test_solution.size());
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnCorrectNumberOfPegsInRightColorInIncorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto incorrect_answer_result = engine.check_solution(std::vector<int>{ { 1, 3, 4, 5, 6 } });
    ASSERT_TRUE(incorrect_answer_result.has_value());
    EXPECT_EQ(incorrect_answer_result.value().pegs_in_right_color, 3);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnCorrectZeroPegsInRightColorInCorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    engine.start_game({ PATTERN_SIZE, 8 });

    auto correct_answer_result = engine.check_solution(test_solution);
    ASSERT_TRUE(correct_answer_result.has_value());
    EXPECT_EQ(correct_answer_result.value().pegs_in_right_color, 0);
}

TEST_F(MastermindEngineTest, ShouldCheckSolutionReturnNulloptBeforeStart) {
    EXPECT_EQ(engine.check_solution({}), std::nullopt);
}

TEST_F(MastermindEngineTest, ShouldGetTriesLeftReturnZeroBeforeStart) {
    ASSERT_EQ(engine.get_status(), mastermind::game_status::NOT_INITIALIZED);
    EXPECT_EQ(engine.get_tries_left(), 0);
}

TEST_F(MastermindEngineTest, ShouldGetTriesLeftReturnMaxValueAfterStart) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    const size_t MAX_TRIES{ 6 };

    engine.start_game({ PATTERN_SIZE, MAX_TRIES });

    ASSERT_EQ(engine.get_status(), mastermind::game_status::IN_GAME);
    EXPECT_EQ(engine.get_tries_left(), MAX_TRIES);
}

TEST_F(MastermindEngineTest, ShouldGetTriesLeftReturnZeroAfterEnd) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));

    engine.start_game({ PATTERN_SIZE, 6 });
    engine.check_solution(test_solution);

    ASSERT_EQ(engine.get_status(), mastermind::game_status::ENDED);
    EXPECT_EQ(engine.get_tries_left(), 0);
}

TEST_F(MastermindEngineTest, ShouldTriesLeftDecrementAfterIncorrectSolution) {
    EXPECT_CALL(generator_mock, function_operator()).Times(Exactly(1));
    const size_t MAX_TRIES{ 6 };

    engine.start_game({ PATTERN_SIZE, MAX_TRIES });
    engine.check_solution(std::vector<int>{ { 1, 3, 4, 5, 6 } });

    ASSERT_EQ(engine.get_status(), mastermind::game_status::IN_GAME);
    EXPECT_EQ(engine.get_tries_left(), MAX_TRIES - 1);
}

TEST_F(MastermindEngineTest, ShouldStartGameThrowZeroMaxTriesValueErrorIfMaxTriesIsZero) {
    ASSERT_THROW(engine.start_game({ PATTERN_SIZE, 0 }), mastermind::zero_max_tries_value_error);
}
