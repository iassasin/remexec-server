#pragma once

#include <string>
#include <iostream>

namespace remexec {

class Task {
private:
	std::string name;
	std::string binary;
	std::string tmpath;
	std::size_t timeout;
public:
	Task(std::string _name, std::string _binary, std::string _tmpath, std::size_t _timeout);
	~Task();

	void run(std::ostream &out, std::ostream &err);
};

}
