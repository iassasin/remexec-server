#include <iostream>

#include "cppsockets/sockets.hpp"

#include "rxprotocol.hpp"
#include "logger.hpp"

using namespace std;
using namespace remexec;

int main(int argc, char **argv){
	RXProtocol p;
	
	p.process(cin, cout);
		
	return 0;
}
