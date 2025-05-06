#pragma once

#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>

#include "tape.hpp"

struct HeapNode {
    int value;
    int tape_index;
    unsigned long long buffer_index;

    bool operator>(const HeapNode& other) const {
        return value > other.value;
    }
};

class SortTape{
    private:
        unsigned long long N;
        unsigned long long M;
        InputDevices::Tape input_tape;
        InputDevices::Tape output_tape;
        std::map<int, std::unique_ptr<InputDevices::Tape>> chunk_data(InputDevices::Tape& input_tape, unsigned long long N, unsigned long long buffer_size);
        void merge(std::map<int, std::unique_ptr<InputDevices::Tape>>& temp_tapes, InputDevices::Tape& output_tape, unsigned long long N, unsigned long long M);
    public:
        SortTape();
        SortTape(InputDevices::Tape& input_tape, InputDevices::Tape& output_tape, unsigned long long N, unsigned long long M);
        ~SortTape();

        void extends_sort();
};