#include "regex/regex.hpp"

#include "rxprotocol.hpp"
#include "logger.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace remexec {
	
using namespace std;
using namespace sinlib;

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
		out << "Here executed job '" << arg << "' with parameters:" << endl;
		for (auto &it : params){
			out << it.first << ": " << it.second << endl;
		}
		out << endl;
	}
}

}