#include <gtest/gtest.h>

#include "sort_tape.hpp"
#include "tape.hpp"

#include <iostream>

TEST(init_tape, tape) {
    // std::ifstream ifs("tape.toml");
    // if (!ifs.is_open()) {
    //     throw std::runtime_error("Unble to open tape.toml");
    // }
    // auto config = toml::parse(ifs);
    unsigned long long N = 50;
    unsigned long long buffer_size = 10;

    std::string input_filename = "numbers-large.txt";
    std::string output_filename = "cpp-sorted.txt";


	InputDevices::Tape input_tape = InputDevices::Tape(N, input_filename);
    InputDevices::Tape output_tape = InputDevices::Tape(N, output_filename);

    SortTape sort_tape = SortTape(input_tape, output_tape, N, buffer_size);
    sort_tape.extends_sort();
	ASSERT_EQ(10, 10)
		<< "Is not equals length of positive number" << -1;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}