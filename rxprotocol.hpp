#pragma once

#include <string>
#include <iostream>

namespace remexec {

class RXProtocol {
private:
	std::istream *in;
	std::ostream *out;

	void response(std::string status, std::string info = "");
	void error(int code, std::string info);
public:
	RXProtocol();
	~RXProtocol();

	void process(std::istream &in, std::ostream &out);
};

}
