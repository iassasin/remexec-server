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
		*out << p.first << ": " << encodeParameterValue(p.second) << endl;
	}

	*out << endl;
}

void RXProtocol::error(int code, string info){
	response("ERROR " + to_string(code), info);
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
				string name = regex_replace(regex_replace(decodeParameterValue(pname->second), regex("\\\\"), "/"), regex("\\.{1,2}/"), "");
				size_t size = stoull(psize->second);

				Log::debug("Appended file: ", name, " ", size);

				ofstream of(tmpath + name, ios::binary);
				size_t copyed = 0;
				if (of.good()){
					copyed = bscopy(of, in, size);
				}
				of.close();

				if (copyed == size){
					in.get(); in.get(); // '\n\n'

					response("OK");
				} else {
					//TODO: implement in sockets seekoff and seekpos
					class : public streambuf {
					protected:
						virtual int overflow(int c){ return c; }
					} nullbuf;
					ostream null(&nullbuf);

					bscopy(null, in, size - copyed + 2);

					error(5, "Can't create file: " + name);
				}
			} else {
				error(3, "Name or Size parameter not set");
			}
		}
		else if (cmd == "EXEC"){
			arg = decodeParameterValue(arg);
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

			vector<string> targs;
			auto pargs = params.find("Arguments");
			if (pargs != params.end()){
				targs = regex_split(pargs->second, regex("\\s*;\\s*"));
				for (auto &targ : targs){
					targ = decodeParameterValue(targ);
				}
			}

			if (path_exists(task)){
				OutPackerBuf obuf(1, 128, out), ebuf(2, 128, out);
				ostream osbuf(&obuf), esbuf(&ebuf);

				response("OK");

				Task t(arg, targs, task, tasktmp, ttimeout);
				bool ok = t.run(osbuf, esbuf);

				osbuf.flush();
				esbuf.flush();

				if (ok){
					response("END");
				} else {
					error(6, "Task execution timeout exceeded");
				}
			} else {
				error(2, "Task not found: " + arg);
			}
		}
		else if (cmd == "FETCH"){
			arg = decodeParameterValue(arg);
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
			error(1, "Unknown command: " + cmd);
		}
		
	}

	Log::debug("Client exit by end of stream");
}

string RXProtocol::encodeParameterValue(const string &val){
	string res;
	res.reserve(val.size());

	for (auto &c : val){
		if ("%; \t\n\r"s.find(c) != string::npos){
			res += "%";
			res += stohex(string(1, c));
		} else {
			res += c;
		}
	}

	return move(res);
}

string RXProtocol::decodeParameterValue(const string &val){
	string res;
	res.reserve(val.size());

	for (size_t i = 0; i < val.size(); ++i){
		if (val[i] == '%' && i+2 < val.size()){
			res += hextos(val.substr(i+1, 2));
			i += 2;
		} else {
			res += val[i];
		}
	}

	return move(res);
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
	if (out->good() && ch != EOF){
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

	return out->good() ? 0 : -1;
}

}
