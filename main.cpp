#include <iostream>

#include "cppsockets/sockets.hpp"

#include "rxprotocol.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "tcpserver.hpp"

using namespace std;
using namespace remexec;
using namespace sockets;

int main(int argc, char **argv){
	Config::loadFromFile("remexec-server.conf");
	if (!Config::checkIsValid()){
		return 1;
	}

	address_ip4 addr(Config::getString(Config::LISTEN_ADDRESS), Config::getInteger(Config::LISTEN_PORT));
	TCPServer server(addr);
	server.start();
		
	return 0;
}
