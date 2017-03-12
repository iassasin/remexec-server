/*
 * tcpserver.hpp
 *
 *  Created on: 12 февр. 2017 г.
 *      Author: assasin
 */

#pragma once

#include "cppsockets/sockets.hpp"
#include "config.hpp"

namespace remexec {

class TCPServer {
private:
	sockets::socket_tcp_server srv;
	Config &config;
public:
	TCPServer(Config &conf, sockets::address_ip4 listen_addr);
	~TCPServer();

	void start();
	void stop();
};

}
