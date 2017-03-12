/*
 * tcpserver.cpp
 *
 *  Created on: 12 февр. 2017 г.
 *      Author: assasin
 */

#include "tcpserver.hpp"

#include "rxprotocol.hpp"
#include "logger.hpp"

namespace remexec {

using namespace sockets;

TCPServer::TCPServer(Config &conf, address_ip4 listen_addr) : srv(listen_addr), config(conf) {

}

TCPServer::~TCPServer() {

}

void TCPServer::start(){
	srv.listen(5);

	if (!srv.valid()){
		Log::error("Can't bind socket");
		return;
	}

	decltype(srv.accept()) cli;
	while (cli = srv.accept()){
		Log::debug("Client connected: ", cli->get_source_address().str());
		RXProtocol p(config);
		p.process(*cli, *cli);
		cli->close();
		Log::debug("Client disconnected: ", cli->get_source_address().str());
	}
}

void TCPServer::stop(){
	srv.close();
}

}
