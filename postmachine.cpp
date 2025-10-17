#include "postmachine.h"
#include <iostream>
#include <algorithm>  // for std::sort for std::vector

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

void parseProgram(std::string &str, std::string &str_trim, uint64_t &number)
{
	// take the number of command
	int space_pos = static_cast<int>(str.find(' '));

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
}

void parseCommand(char state, std::string& str, uint64_t& new_command, uint64_t& new_command2)
{
	// take the number of next command
	int space_pos = static_cast<int>(str.find(' '));

	int end_pos = static_cast<int>(str.length());

	int comment_pos = static_cast<int>(str.find('%'));  // if not found: -1 (as int)
	if (comment_pos != -1) { end_pos = comment_pos - 2; }

	std::string str_command;

	if (state == '?')
	{
		int space_pos2 = static_cast<int>(str.find(' ', space_pos + 1));  // find the second occurence of space

		str_command = str.substr(space_pos + 1, space_pos2);

		new_command = strToULL(str_command);

		str_command = str.substr(space_pos2 + 1, end_pos);

		new_command2 = strToULL(str_command);
	}
	else
	{
		str_command = str.substr(space_pos + 1, end_pos);

		new_command = strToULL(str_command);

		new_command2 = new_command;
	}

}

void keySearch(std::vector<std::pair<uint64_t, std::string>> sorted_program, char& new_state, uint64_t& new_command_first, uint64_t& new_command_second)
{
	for (int i = 0; i < sorted_program.size(); i++)
	{
		if (sorted_program[i].first == new_command_first)
		{
			std::string str = sorted_program[i].second;

			new_state = str[0];

			if (new_state != '!')
			{
				parseCommand(new_state, str, new_command_first, new_command_second);
			}
			break;
		}
	}
}

bool errorCheck(std::vector<std::pair<uint64_t, std::string>> sorted_program)
{
	std::vector<uint64_t> command_numbers;
	std::vector<uint64_t> new_commands;
	for (int i = 0; i < sorted_program.size(); i++)
	{
		uint64_t command_number = sorted_program[i].first;
		command_numbers.push_back(command_number);

		if (i == 0) { new_commands.push_back(command_number); } 

		std::string str = sorted_program[i].second;
		char state = str[0];

		uint64_t new_command = 0;
		uint64_t new_command2 = 0;
		parseCommand(state, str, new_command, new_command2);

		if (new_command != 0 && std::find(new_commands.begin(), new_commands.end(), new_command) == new_commands.end())
		{
			new_commands.push_back(new_command);
		}

		if (new_command2 != 0 && std::find(new_commands.begin(), new_commands.end(), new_command2) == new_commands.end())
		{
			new_commands.push_back(new_command2);
		}
	}

	for (int i = 0; i < new_commands.size(); i++)
	{
		// there is no command number to refer to
		if (std::find(command_numbers.begin(), command_numbers.end(), new_commands[i]) == command_numbers.end())
		{
			return false;
		}
	}

	return true;
}

std::string PostMachine::calc(std::string const& init, uint64_t maxsteps)
{
	std::string res = init;

	std::vector<std::pair<uint64_t, std::string>> sorted_program;
	sortProgram(sorted_program);

	// check for errors in program: all command numbers could be called (exist in program)
	if (!errorCheck(sorted_program))
	{
		throw "Error";
	}

	char state = ' ';
	uint64_t new_command = 0;
	uint64_t new_command2 = 0;

	// set start state
	std::string str = sorted_program[0].second;

	state = str[0];

	if (state == '!') { return res; }

	parseCommand(state, str, new_command, new_command2);

	char new_state = ' ';

	uint64_t index = 0;

	bool loopBreak = false;
	while (maxsteps > 0)
	{
		--maxsteps;

		if (maxsteps == 0) {
			throw "Not applicable";
		}

		switch (state)
		{
		case 'V':
			// change symbol on the tape
			res[index] = '1';
			keySearch(sorted_program, new_state, new_command, new_command2);
			break;

		case 'X':
			res[index] = '0';
			keySearch(sorted_program, new_state, new_command, new_command2);
			break;

		case 'L':
			// move on the the circular tape
			if (index == 0)
			{
				index = res.length() - 1;
			}
			else
			{
				--index;
			}
			keySearch(sorted_program, new_state, new_command, new_command2);
			break;

		case 'R':
			if (index == res.length() - 1)
			{
				index = 0;
			}
			else
			{
				++index;
			}
			keySearch(sorted_program, new_state, new_command, new_command2);
			break;

		case '?':
			if (res[index] == '0')
			{
				keySearch(sorted_program, new_state, new_command, new_command2);
			}
			else
			{
				keySearch(sorted_program, new_state, new_command2, new_command);
			}
			break;

		default:
			loopBreak = true;
		}

		if (loopBreak || new_state == '!') { break; }

		// change state
		state = new_state;
	}

	maxsteps_ = maxsteps;
	index_ = index;

	return res;
}
