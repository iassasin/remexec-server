#pragma once

#include <string>
#include <iostream>

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

	void response(std::string status, std::string info = "");
	void error(int code, std::string info);
public:
	RXProtocol();
	~RXProtocol();

	void process(std::istream &in, std::ostream &out);
};

}
