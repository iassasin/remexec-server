#include "regex/regex.hpp"
#include "pstream.h"

#include "rxprotocol.hpp"
#include "logger.hpp"
#include "config.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unistd.h>

namespace remexec {
	
using namespace std;
using namespace sinlib;
using namespace redi;

RXProtocol::RXProtocol()
{
}

RXProtocol::~RXProtocol()
{
}

void RXProtocol::process(istream &in, ostream &out){
	string line;
	while (getline(in, line)){
		if (line.empty())
			continue;
		
		vector<string> args = regex_split(line, regex("\\s+"), 2);
		if (args.size() < 2){
			out << "ERROR 1 Unknown command: " << line << endl << endl;
			continue;
		}
		
		string cmd = args[0], arg = args[1];
		
		unordered_map<string, string> params;
		
		if (cmd == "EXEC"){
			
		}
		else {
			out << "ERROR 1 Unknown command: " << cmd << endl << endl;
			continue;
		}
		
		while (getline(in, line)){
			if (line.empty())
				break;
			
			vector<string> pars = regex_split(line, regex(":\\s+"), 2);
			if (pars[0] == "File"){
				Log::log("Appended file: ", pars[1]);
			} else {
				params[pars[0]] = pars[1];
			}
		}
		
		out << "OK command " << cmd << endl;
		
		string task = Config::readString(Config::TASK_DIR) + arg;
		string tasktmp = Config::readString(Config::TEMP_DIR);
		
		char buf[2048];
		getcwd(buf, sizeof(buf));
		chdir(tasktmp.c_str());
		
		pstream p(task);
		string pline;
		while (getline(p, pline)){
			out << pline << endl;
		}
		p.close();
		
		chdir(buf);

		out << endl;
	}
}

}