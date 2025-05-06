#include "sort_tape.cpp"
#include "tape.cpp"

int main(){
    unsigned long long N ;
    unsigned long long M;

    std::string input_filename;
    std::string output_filename;

    std::cout << "Please, entere N of tape: ";
    std::cin >> N;
    std::cout << "\n";

    std::cout << "Please, entere M of tape: ";
    std::cin >> M;
    std::cout << "\n";
    
    std::cout << "Please, entere input_filename of tape: ";
    std::cin >> input_filename;
    std::cout << "\n";

    std::cout << "Please, entere output_filename of tape: ";
    std::cin >> output_filename;
    std::cout << "\n";

	InputDevices::Tape input_tape = InputDevices::Tape(N, input_filename);
    InputDevices::Tape output_tape = InputDevices::Tape(N, output_filename);

    SortTape sort_tape = SortTape(input_tape, output_tape, N, M);
    sort_tape.extends_sort();

    return 0;
}