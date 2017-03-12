#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include "config.hpp"

namespace remexec {

class RXProtocol {
private:
	class OutPackerBuf : public std::streambuf {
	private:
		int fd;
		char_type *buf;
		std::size_t bufsize;
		std::ostream *out;
	protected:
		virtual int overflow(int ch) override;
		virtual int sync() override;
	public:
		OutPackerBuf(int fd, std::size_t bufsize, std::ostream &out);
		virtual ~OutPackerBuf();
	};

	std::istream *in;
	std::ostream *out;
	Config &config;

	void response(std::string status, std::string info = "", std::unordered_map<std::string, std::string> params = {});
	void error(int code, std::string info);
public:
	RXProtocol(Config &conf);
	~RXProtocol();

	void process(std::istream &in, std::ostream &out);
};

}
