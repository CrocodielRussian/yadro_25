#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <stdexcept>

#include "toml.hpp"

namespace InputDevices
{
    class Tape{
        public:
            Tape();
            Tape(unsigned long long N, const std::string& filename);
            ~Tape();
            
            Tape& operator=(const Tape& tape);

            int move_tape_forward();
            int move_tape_back();
            int read_value(std::vector<int>& buffer, unsigned long long buffer_size);
            int write_value(const std::vector<int> &buffer, unsigned long long buffer_size);
            void reset_position(unsigned long long buffer_size);

        private:
            unsigned long long N = 0; 

            std::string filename;
            
            bool start_file = true;

            unsigned long long cur_pos = 0;

            unsigned int delay_write = 0;
            unsigned int delay_read = 0;
            unsigned int delay_move_tape = 0;
            unsigned int delay_fast_forward_tape = 0;

            std::fstream file;
    };
    
}
