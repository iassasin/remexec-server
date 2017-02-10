#include "rxprotocol.hpp"

#include "logger.hpp"
#include "config.hpp"
#include "task.hpp"
#include "utils.hpp"

#include "regex/regex.hpp"
#include "pstream.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cmath>

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
		
		if (cmd == "EXEC"){
			unordered_map<string, string> params;
			string task = realpath(Config::readString(Config::TASK_DIR)) + "/" + arg;
			string tasktmp = realpath(Config::readString(Config::TEMP_DIR)) + "/" + to_string(time(NULL)) + "/";
			
			Log::log("Task binary: ", task);
			Log::log("Task tmp: ", tasktmp);
			
			bool valid = path_exists(task);
			if (valid){
				mkdir(tasktmp);
			}
			
			while (getline(in, line)){
				if (line.empty())
					break;
				
				vector<string> pars = regex_split(line, regex(":\\s*"), 2);
				if (pars[0] == "File"){
					Log::log("Appended file: ", pars[1]);
					
					vector<string> filev = regex_split(pars[1], regex("\\s+", 2));
					size_t fsz = stoll(filev[0]);
					string fn = regex_replace(filev[1], regex("\\.{1,2}/"), "");
					
					ofstream of(tasktmp + fn, ios::binary);
					char buf[64];
					while (fsz > 0 && in.read(buf, min(sizeof(buf), fsz))){
						size_t gc = in.gcount();
						fsz -= gc;
						of.write(buf, gc);
					}
					of.close();
					
					in.get(); //get '\n'
				} else {
					Log::log("Parameter: ", pars[0], ": ", pars[1]);
					params[pars[0]] = pars[1];
				}
			}
			
			if (valid){
				Task t(arg, task, tasktmp);
				out << "OK" << endl;
				t.run(out);
				
				Log::log("rm -rf --one-file-system ", tasktmp);
				pstream prm("rm", {"rm", "-rf", "--one-file-system", tasktmp});
				prm.close();
				
				Log::log("Cleanup complete");
			} else {
				out << "ERROR 2 Task not found: " << arg << endl;
			}
		}
		else {
			out << "ERROR 1 Unknown command: " << cmd << endl;
		}
		
		out << endl;
	}
}

}