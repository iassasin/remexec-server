#pragma once

#include <iostream>

namespace remexec {

class RXProtocol {
public:
	RXProtocol();
	~RXProtocol();
	
	void process(std::istream &in, std::ostream &out);
};

}
