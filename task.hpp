#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace remexec {

class Task {
private:
	std::string name;
	std::vector<std::string> args;
	std::string binary;
	std::string tmpath;
	std::size_t timeout;
public:
	Task(std::string _name, std::vector<std::string> _args, std::string _binary, std::string _tmpath, std::size_t _timeout);
	~Task();

	bool run(std::ostream &out, std::ostream &err);
};

}
