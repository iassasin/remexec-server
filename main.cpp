#include <iostream>

#include "cppsockets/sockets.hpp"
#include "pstream.h"

#include "rxprotocol.hpp"
#include "logger.hpp"

using namespace std;
using namespace redi;
using namespace remexec;

void process_test(){
	pstream p("/bin/sh", {"sh", "-c", "ls -laF"});
	
	string line;
	while (getline(p, line)){
		Log::log(line);
	}
}

int main(int argc, char **argv){
	RXProtocol p;
	
	p.process(cin, cout);
		
	return 0;
}
