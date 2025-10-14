#include <fstream>
#include <iostream>
#include "postmachine.h"

int main(int argc, char* argv[]) {
	
	if (argc != 4)
	{
		std::cout << "Usage: ./post-machine progr_file input_file output_file";
		return 1;
	}

	char* input_name = argv[2];
	char* progr_name = argv[1];

	std::ifstream input(input_name);
	std::ifstream progr(progr_name);

	if ((input.is_open() | progr.is_open()) == 0)
	{
		std::cout << "Couldn't open files";
		return 2;
	}

	uint64_t tape_length = 0;
	uint64_t maxsteps = 0;
	std::string init;

	input >> tape_length;
	input >> maxsteps;
	std::getline(input, init);

	std::string init_trim = init.substr(1, init.length());  // trim space at beginning

	input.close();

	std::string tape(tape_length-(init_trim.length()), '0');
	tape.insert(0, init_trim);

	// read file by lines
	std::vector<std::string> program;
	std::string line;

	while (std::getline(progr, line))
	{
		program.push_back(line);
	}

	progr.close();

	PostMachine postmachine(tape_length, program);

	try 
	{
		std::string res = postmachine.calc(tape, maxsteps);

		std::string ans;

		uint64_t steps_left = postmachine.maxsteps_;
		uint64_t start_index = postmachine.index_;
		uint64_t last_index = start_index;  // will be changed

		// Find the last '1' in the tape. Wrap around if necessary
		for (uint64_t i = 0; i < tape_length; ++i) {
			if (res[(start_index + i) % tape_length] == '1') {
				last_index = (start_index + i) % tape_length;
			}
		}

		// Create a string: tape from start_index to last_index. Wrap around if necessary
		for (uint64_t i = 0; i < tape_length; ++i) {
			uint64_t current_index = (start_index + i) % tape_length;
			ans.push_back(res[current_index]);
			if (current_index == last_index) {
				break;
			}
		}

		std::ofstream output(argv[3]);
		output << ans;
		output.close();
	} 
	catch (const char* error)
	{
		std::ofstream output(argv[3]);
		output << error;
		output.close();
	}

	return 0;
}