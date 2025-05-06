#include "tape.hpp"

namespace InputDevices {
    Tape::Tape() = default;

    Tape::Tape(unsigned long long N, const std::string& filename)
        : N(N), filename(filename), cur_pos(0) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.open(this->filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
            if (!file.is_open()) throw std::runtime_error("Unable to open or create file: " + filename);
        }

        try {
            std::ifstream ifs("tape.toml");
            if (!ifs.is_open()) {
                throw std::runtime_error("Unable to open tape.toml");
            }
            auto config = toml::parse(ifs);
            unsigned int delay_write = config["physical_setting"]["delay_write"].value_or(0);
            unsigned int delay_read = config["physical_setting"]["delay_read"][0].value_or(0);
            unsigned int delay_move_tape = config["physical_setting"]["delay_move_tape"].value_or(0);
            unsigned int delay_fast_forward_tape = config["physical_setting"]["delay_fast_forward_tape"].value_or(0);

            this->delay_write = delay_write;
            this->delay_read = delay_read;
            this->delay_move_tape = delay_move_tape;
            this->delay_fast_forward_tape = delay_fast_forward_tape;

        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to parse tape.toml: " + std::string(e.what()));
        }
    }

    Tape::~Tape() {
        if (file.is_open()) {
            file.close();
        }
        if (!filename.empty() && filename.find("tmp/") == 0) {
            std::remove(filename.c_str());
        }
    }

    Tape &Tape::operator=(const Tape &tape)
    {
        if (this == &tape) {
            return *this;
        }

        if (file.is_open()) {
            file.close();
        }

        N = tape.N;
        filename = tape.filename;
        cur_pos = tape.cur_pos;
        delay_write = tape.delay_write;
        delay_read = tape.delay_read;
        delay_move_tape = tape.delay_move_tape;
        delay_fast_forward_tape = tape.delay_fast_forward_tape;
        start_file = tape.start_file;

        if (!filename.empty()) {
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                file.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
                if (!file.is_open()) {
                    throw std::runtime_error("Unable to open or create file: " + filename);
                }
            }
            file.seekg(cur_pos);
            file.seekp(cur_pos);
        }

        return *this;
    }


    int Tape::move_tape_forward() {
        if (cur_pos < N * 4) {
            cur_pos += sizeof(int);
            std::chrono::milliseconds timespan(delay_move_tape);
            std::this_thread::sleep_for(timespan);
            return 0;
        }
        return 1;
    }

    int Tape::move_tape_back() {
        if (cur_pos > sizeof(int)) {
            cur_pos -= sizeof(int);
            std::chrono::milliseconds timespan(delay_move_tape);
            std::this_thread::sleep_for(timespan);
            return 0;
        }
        return 1;
    }

    int Tape::read_value(std::vector<int>& buffer, unsigned long long buffer_size) {
        if (!file.good()) {
            return 1;
        }
        for (int i = 0; i < buffer_size; i++) {
            if(file.peek()==EOF){
                this->cur_pos = this->file.tellg();
                return 1;
            } 
            int num;
            std::chrono::milliseconds timespan(delay_read + delay_move_tape);
            std::this_thread::sleep_for(timespan);
            file >> num;
            buffer.push_back(num);
            cur_pos = file.tellg();
        }
        return file.good() ? 0 : 1;
    }

    int Tape::write_value(const std::vector<int> &buffer, unsigned long long buffer_size)
    {
        file.clear();
        file.seekp(0, std::ios::end);

        for (size_t i = 0; i < buffer.size(); i++) {
            if(!start_file) file << " "; else start_file = false;

            std::chrono::milliseconds timespan(delay_write + delay_move_tape);
            std::this_thread::sleep_for(timespan);
            file << buffer[i];
        }

        this->file.flush();

        return 0;
    }

    void Tape::reset_position(unsigned long long buffer_size) {
        file.clear();
        file.seekg(0);
        cur_pos = 0;
        std::chrono::milliseconds timespan(delay_fast_forward_tape * buffer_size);
        std::this_thread::sleep_for(timespan);
    }
}