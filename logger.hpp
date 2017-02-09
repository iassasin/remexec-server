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
	public:
		static void init(std::ostream &l){ _log = &l; }
		static void log(std::string message){
			std::ostream *s = &std::cout;
			if (_log){
				s = _log;
			}
			
			*s << date("[%Y-%m-%d %H:%M:%S] ") << message << std::endl;
		}
};

}
