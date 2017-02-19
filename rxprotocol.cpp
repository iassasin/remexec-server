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
		
		string cmd = args[0], arg = args.size() > 1 ? args[1] : "";
		
		if (cmd == "EXIT"){
			Log::debug("Client exit by protocol");
			return;
		}
		else if (cmd == "EXEC"){
			unordered_map<string, string> params;
			string task = realpath(Config::getString(Config::TASK_DIR)) + "/" + arg;
			string tasktmp = realpath(Config::getString(Config::TEMP_DIR)) + "/" + to_string(time(NULL)) + "/";
			
			Log::debug("Task binary: ", task);
			Log::debug("Task tmp: ", tasktmp);
			
			bool valid = path_exists(task);
			if (valid){
				mkdir(tasktmp);
			}
			
			while (getline(in, line)){
				if (line.empty())
					break;
				
				vector<string> pars = regex_split(line, regex(":\\s*"), 2);
				if (pars.size() < 2){
					Log::warn("Non-parameter line, skipping: ", line);
					continue;
				}
				if (pars[0] == "File"){
					Log::debug("Appended file: ", pars[1]);
					
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
					Log::debug("Parameter: ", pars[0], ": ", pars[1]);
					params[pars[0]] = pars[1];
				}
			}
			
			if (valid){
				Task t(arg, task, tasktmp);
				out << "OK" << endl;
				t.run(out);
				
				Log::debug("rm -rf --one-file-system ", tasktmp);
				pstream prm("rm", {"rm", "-rf", "--one-file-system", tasktmp});
				prm.close();
				
				Log::debug("Cleanup complete");
			} else {
				out << "ERROR 2 Task not found: " << arg << endl;
			}
		}
		else {
			out << "ERROR 1 Unknown command: " << cmd << endl;
		}
		
		out << endl;
	}

	Log::debug("Client exit by end of stream");
}

}
