#include <iostream>

#include "cppsockets/sockets.hpp"
#include "pstream.h"

#include "logger.hpp"

using namespace std;
using namespace redi;
using namespace remexec;

int main(int argc, char **argv){
	pstream p("/bin/sh", {"sh", "-c", "ls -laF"});
	
	string line;
	while (getline(p, line)){
		Log::log(line);
	}
		
	return 0;
}
