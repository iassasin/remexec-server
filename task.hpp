#pragma once

#include <string>
#include <iostream>

namespace remexec {

class Task {
private:
	std::string name;
	std::string binary;
	std::string tmpath;
public:
	Task(std::string name, std::string binary, std::string tmpath);
	~Task();

	void run(std::ostream &out, std::ostream &err);
};

}
