#pragma once

#include <string>
#include <iostream>
#include <ctime>

namespace remexec {

class Log {
private:
	Log();
	~Log();

	static std::ostream *_log;

	static std::string date(const std::string &format){
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[format.size()*2];

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		strftime(buffer, format.size()*2, format.c_str(), timeinfo);
		
		return buffer;
	}

	template<typename T, typename ...Args>
	static void do_log(std::ostream &s, const T &arg, const Args &...args){
		s << arg;
		do_log(s, args...);
	}

	template<typename T>
	static void do_log(std::ostream &s, const T &arg){
		s << arg << std::endl;
	}
public:
	static void init(std::ostream &l){ _log = &l; }

	template<typename ...Args>
	static void log(const Args &...args){
		std::ostream *s = &std::cout;
		if (_log){
			s = _log;
		}
		
		*s << date("[%Y-%m-%d %H:%M:%S] ");
		
		do_log(*s, args...);
	}
};

}
