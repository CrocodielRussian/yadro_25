#include "sort_tape.hpp"

SortTape::SortTape() = default;
SortTape::SortTape(InputDevices::Tape& input_tape, InputDevices::Tape& output_tape, unsigned long long N, unsigned long long M)
{
    this->input_tape = input_tape;
    this->output_tape = output_tape;
    this->N = N; 
    this->M = M;
}
SortTape::~SortTape() = default;

std::map<int, std::unique_ptr<InputDevices::Tape>> SortTape::chunk_data(InputDevices::Tape &input_tape, unsigned long long N, unsigned long long buffer_size)
{
    std::map<int, std::unique_ptr<InputDevices::Tape>> temp_tapes;
    unsigned long long count_temp_tapes = ceil((double)N / buffer_size);

    system("mkdir tmp");
    for (int i = 0; i < count_temp_tapes; i++) {
        std::string prefix = std::to_string(i);
        temp_tapes[i] = std::make_unique<InputDevices::Tape>(N, "tmp/" + prefix + ".txt");
    }

    std::cout << "start chunking" << "\n";
    std::vector<int> buffer;
    for (int i = 0; i < count_temp_tapes; i++) {
        input_tape.read_value(buffer, buffer_size);
        std::sort(buffer.begin(), buffer.end());
        temp_tapes[i]->write_value(buffer, buffer_size);
        temp_tapes[i]->reset_position(buffer_size);
        buffer.clear();
    }
    std::cout << "chunking done" << "\n";
    return temp_tapes;
}
void SortTape::merge(std::map<int, std::unique_ptr<InputDevices::Tape>>& temp_tapes, InputDevices::Tape& output_tape, unsigned long long N, unsigned long long M) {
    unsigned long long count_temp_tapes = ceil((double)N / M);
    unsigned long long buffer_size = N / (count_temp_tapes + 1);
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> min_heap;
    std::vector<std::vector<int>> buffers(temp_tapes.size());
    std::vector<bool> tape_active(temp_tapes.size(), true);
    std::vector<int> output_buffer;
    output_buffer.reserve(buffer_size);

    for (const auto& [index, tape] : temp_tapes) {
        if (tape->read_value(buffers[index], buffer_size) == 0) {
            min_heap.push({buffers[index][0], index, 0});
        } else {
            tape_active[index] = false;
        }
    }

    unsigned long long cnt = 0;
    while (!min_heap.empty()) {
        HeapNode node = min_heap.top();
        min_heap.pop();
        int tape_index = node.tape_index;
        unsigned long long buffer_index = node.buffer_index;

        output_buffer.push_back(node.value);


        if (output_buffer.size() >= buffer_size) {
            output_tape.write_value(output_buffer, output_buffer.size());
            output_buffer.clear();
        }

        cnt++;

        if (buffer_index + 1 < buffers[tape_index].size()) {
            min_heap.push({buffers[tape_index][buffer_index + 1], tape_index, buffer_index + 1});
        } else {
            buffers[tape_index].clear();
            if (tape_active[tape_index] && temp_tapes[tape_index]->read_value(buffers[tape_index], buffer_size)) {
                if (!buffers[tape_index].empty()) {
                    min_heap.push({buffers[tape_index][0], tape_index, 0});
                }
            } else {
                tape_active[tape_index] = false;
            }
        }
    }

    if (!output_buffer.empty()) {
        output_tape.write_value(output_buffer, output_buffer.size());
    }
}

void SortTape::extends_sort()
{
    auto temp_tapes = chunk_data(input_tape, N, M);
    merge(temp_tapes, output_tape, N, M);
}
