#include <iostream>
#include <unordered_map>
#include <functional>  // for std::function
#include "postmachine.h"

uint64_t strToULL(std::string str) {
	uint64_t res = 0;
	size_t str_length = str.length();
	for (int i = 0; i < str_length; ++i) {
		char c = str[i];
		if (std::isdigit(c) == 0) {
			break;
		}
		res = res * 10 + (c - '0');
	}
	return res;
}

void parseProgram(std::string str, std::string &str_trim, uint64_t &number)
{
	// take the number of command
	int space_pos = str.find(' ');

	std::string str_number = str.substr(0, space_pos - 1);  // remove dot before space

	str_trim = str.substr(space_pos + 1, str.length());

	number = strToULL(str_number);
}

void PostMachine::sortProgram(std::vector<std::pair<uint64_t, std::string>> &sorted_program)
{
	// process program lines; %-\n is a comment
	for (int i = 0; i < program_.size(); i++)
	{
		// line is a comment
		if (program_[i][0] == '%') { continue; }

		std::string str = program_[i];
		std::string str_trim;
		uint64_t number;

		parseProgram(str, str_trim, number);

		sorted_program.push_back({ number, str_trim });
	}

	// sort commands ascendingly
	for (int i = 0; i < sorted_program.size()-1; i++)
	{
		if (sorted_program[i + 1].first < sorted_program[i].first)
		{
			std::pair<uint64_t, std::string> temp = sorted_program[i];
			sorted_program[i] = sorted_program[i + 1];
			sorted_program[i + 1] = temp;
		}
	}

	/*for (int i = 0; i < sorted_program.size(); i++)
	{
		std::cout << sorted_program[i].first << ' ' << sorted_program[i].second << '\n';
	}*/
}

void keySearch(std::vector<std::pair<uint64_t, std::string>> sorted_program, char& new_state, uint64_t& new_command, uint64_t& new_command2)
{
	for (int i = 0; i < sorted_program.size(); i++)
	{
		if (sorted_program[i].first == new_command)
		{
			std::string str = sorted_program[i].second;

			new_state = str[0];

			if (new_state != '!')
			{
				// take the number of next command
				int space_pos = str.find(' ');

				int end_pos = str.length();

				int comment_pos = str.find('%');  // if not found: -1
				if (comment_pos != -1) { end_pos = comment_pos - 2; }

				std::string str_command;

				if (new_state == '?')
				{
					int space_pos2 = str.find(' ', space_pos + 1);  // find the second occurence of space

					str_command = str.substr(space_pos + 1, space_pos2);

					new_command = strToULL(str_command);

					str_command = str.substr(space_pos2 + 1, end_pos);

					new_command2 = strToULL(str_command);
				}
				else
				{
					str_command = str.substr(space_pos + 1, end_pos);

					new_command = strToULL(str_command);
				}
			}

			break;
		}
	}
}

std::string PostMachine::calc(std::string const& init, int maxsteps)
{
	std::string res = init;

	std::vector<std::pair<uint64_t, std::string>> sorted_program;
	sortProgram(sorted_program);

	char state = ' ';
	uint64_t new_command;
	uint64_t new_command2;

	// set start state
	std::string str = sorted_program[0].second;

	state = str[0];

	if (state == '!') { return res; }

	// take the number of next command
	int space_pos = str.find(' ');
		
	int end_pos = str.length();

	int comment_pos = str.find('%');  // if not found: -1
	if (comment_pos != -1) { end_pos = comment_pos - 2; }

	std::string str_command;

	if (state == '?') 
	{ 
		int space_pos2 = str.find(' ', space_pos + 1);  // find the second occurence of space

		str_command = str.substr(space_pos + 1, space_pos2);
			
		new_command = strToULL(str_command);

		str_command = str.substr(space_pos2 + 1, end_pos);

		new_command2 = strToULL(str_command);
	}
	else
	{
		str_command = str.substr(space_pos + 1, end_pos);

		new_command = strToULL(str_command);
	}

	char new_state = ' ';

	uint64_t index = 0;

	bool forBreak = false;
	bool loopBreak = false;
	while (maxsteps > 0)
	{
		// read the tape
		for (int i = 0; i < res.length(); i++)
		{
			switch (state)
			{
			case 'V':
				// change symbol on the tape
				res[i] = 1;
				keySearch(sorted_program, new_state, new_command, new_command2);
				break;

			case 'X':
				res[i] = 0;
				keySearch(sorted_program, new_state, new_command, new_command2);
				break;

			case 'L':
				// move on the the circular tape
				if (i <= 0)
				{
					index = res.length();
				}
				else
				{
					--index;
				}
				keySearch(sorted_program, new_state, new_command, new_command2);
				break;

			case 'R':
				if (i > res.length())
				{
					index = 0;
				}
				else
				{
					++index;
				}
				keySearch(sorted_program, new_state, new_command, new_command2);
				break;

			case 'Q':
				if (res[i] == '0')
				{
					keySearch(sorted_program, new_state, new_command, new_command2);
				}
				else
				{
					keySearch(sorted_program, new_state, new_command, new_command2);
				}
				break;

			// includes case '!': stop status
			default:
				forBreak = true;
			}

			if (forBreak) { loopBreak = true; break; }

			// change state
			state = new_state;

			--maxsteps;

			if (maxsteps == 0) {
				res = "Not applicable";
				loopBreak = true;
				break;
			}
		}
		if (loopBreak) { break; }
	}

	index_ = index;

	return res;
}
