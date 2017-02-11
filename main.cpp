#include <iostream>

#include "cppsockets/sockets.hpp"

#include "rxprotocol.hpp"
#include "logger.hpp"
#include "config.hpp"

using namespace std;
using namespace remexec;

int main(int argc, char **argv){
	Config::loadFromFile("remexec-server.conf");

	RXProtocol p;
	
	p.process(cin, cout);
		
	return 0;
}
