#ifndef POSTMACHINE
#define POSTMACHINE

#include <cstdint>  // for uint64_t
#include <string>
#include <vector>

class PostMachine
{
private:
	uint64_t tape_length_ = 0;
	std::vector<std::string> program_;

public:
	// Constructor
	PostMachine(uint64_t tape_length, std::vector<std::string>& program)
		: tape_length_(tape_length), program_(program)
	{
	}

	uint64_t maxsteps_ = 0;
	uint64_t index_ = 0;

	void sortProgram(std::vector<std::pair<uint64_t, std::string>> &sorted_program);

	std::string calc(std::string const& init, uint64_t maxsteps);
};

#endif