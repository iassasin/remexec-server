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
	Config defconf;
	defconf.loadDefaultConfig();

	Config config(defconf);
	config.loadFromFile("remexec-server.conf");
	if (!config.checkIsValid()){
		return 1;
	}

	address_ip4 addr(config.getString(Config::LISTEN_ADDRESS), config.getInteger(Config::LISTEN_PORT));
	Log::info("Listen on ", addr.str());

	TCPServer server(config, addr);
	server.start();
		
	return 0;
}
