#include "rxprotocol.hpp"

#include "logger.hpp"
#include "config.hpp"
#include "task.hpp"
#include "utils.hpp"
#include "workspace.hpp"

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

#ifdef REGEX_BOOST
using boost::regex_replace;
#endif

RXProtocol::RXProtocol(Config &conf) : in(nullptr), out(nullptr), config(conf) {}
RXProtocol::~RXProtocol(){}

void RXProtocol::response(string status, string info, unordered_map<string, string> params){
	*out << status;
	if (!info.empty())
		*out << " " << info;
	*out << endl;

	for (auto &p : params){
		*out << p.first << ": " << p.second << endl;
	}

	*out << endl;
}

void RXProtocol::error(int code, string info){
	response(string("ERROR ") + to_string(code), info);
}

void RXProtocol::process(istream &in, ostream &out){
	this->in = &in;
	this->out = &out;

	Workspace workspace(config);

	string line;
	while (getline(in, line)){
		if (line.empty())
			continue;
		
		vector<string> args = regex_split(line, regex("\\s+"), 2);
		unordered_map<string, string> params;
		
		//command and argument (CMD arg)
		string cmd = args[0], arg = args.size() > 1 ? args[1] : "";
		
		//read parameters loop
		while (getline(in, line)){
			if (line.empty())
				break;

			vector<string> pars = regex_split(line, regex(":\\s*"), 2);
			if (pars.size() < 2){
				Log::warn("Non-parameter line, skipping: ", line);
				continue;
			}

			Log::debug("Parameter: ", pars[0], ": ", pars[1]);
			params[pars[0]] = pars[1];
		}

		if (cmd == "EXIT"){
			Log::debug("Client exit by protocol");
			return;
		}
		else if (cmd == "FILE"){
			auto pname = params.find("Name");
			auto psize = params.find("Size");
			if (pname != params.end() && psize != params.end()){
				string tmpath = workspace.getWorkdir();
				string name = regex_replace(regex_replace(pname->second, regex("\\\\"), "/"), regex("\\.{1,2}/"), "");
				size_t size = stoull(psize->second);

				Log::debug("Appended file: ", name, " ", size);

				ofstream of(tmpath + name, ios::binary);
				bscopy(of, in, size);
				of.close();

				in.get(); in.get(); // '\n\n'

				response("OK");
			} else {
				error(3, "Name or Size parameter not set");
			}
		}
		else if (cmd == "EXEC"){
			string task = realpath(config.getString(Config::TASK_DIR)) + "/" + arg;
			string taskconf = realpath(config.getString(Config::TASKCONF_DIR)) + "/" + arg + ".conf";
			string tasktmp = workspace.getWorkdir();
			
			Config tconf(config);
			if (!tconf.loadFromFile(taskconf)){
				Log::debug("No config file for task ", arg);
			}

			size_t ttimeout = tconf.getInteger(Config::TASK_TIMEOUT);

			Log::debug("Task binary: ", task);
			Log::debug("Task tmp: ", tasktmp);
			Log::debug("Task timeout: ", ttimeout);

			if (path_exists(task)){
				OutPackerBuf obuf(1, 128, out), ebuf(2, 128, out);
				ostream osbuf(&obuf), esbuf(&ebuf);

				Task t(arg, task, tasktmp, ttimeout);
				response("OK");
				t.run(osbuf, esbuf);

				osbuf.flush();
				esbuf.flush();

				response("END");
			} else {
				error(2, string("Task not found: ") + arg);
			}
		}
		else if (cmd == "FETCH"){
			string tmpath = workspace.getWorkdir();
			string path = tmpath + "/" + regex_replace(arg, regex("\\.{1,2}/"), "");
			ifstream file(path);

			if (file.good()){
				string name = basename(path);
				size_t sz;

				file.seekg(0, ios::end);
				sz = file.tellg();
				file.seekg(0, ios::beg);

				Log::debug("Fetched file (size: ", sz, "): ", path);

				response("FILE", "", {
					{"Name", name},
					{"Size", to_string(sz)},
				});

				bscopy(out, file, sz);
				out << endl << endl;
			} else {
				error(4, "File not found");
			}
		}
		else {
			error(1, string("Unknown command: ") + cmd);
		}
		
	}

	Log::debug("Client exit by end of stream");
}

RXProtocol::OutPackerBuf::OutPackerBuf(int f, size_t bsize, ostream& o){
	fd = f;
	bufsize = bsize;
	out = &o;

	buf = new char_type[bsize];
	setp(buf, buf + (bsize - 1));
}

RXProtocol::OutPackerBuf::~OutPackerBuf(){
	delete[] buf;
}

int RXProtocol::OutPackerBuf::overflow(int ch){
	if (*out && ch != EOF){
		*pptr() = ch;
		pbump(1);
		return sync() == 0 ? ch : EOF;
	}

	return EOF;
}

int RXProtocol::OutPackerBuf::sync(){
	if (pptr() == pbase())
		return 0;

	auto sz = (size_t) (pptr() - pbase());

	*out << "STREAM " << fd << endl
			<< "Size: " << sz << endl
			<< endl;
	out->write(pbase(), sz);
	*out << endl << endl;

	pbump(-sz);

	return out ? 0 : -1;
}

}
