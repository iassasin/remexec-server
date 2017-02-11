#pragma once

#include <iostream>

namespace remexec {

class RXProtocol {
private:

public:
	RXProtocol();
	~RXProtocol();

	void process(std::istream &in, std::ostream &out);
};

}
