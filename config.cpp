#include "config.hpp"

#include <vector>
#include <fstream>

#include "regex/regex.hpp"

#include "logger.hpp"
#include "utils.hpp"

namespace remexec {

using namespace std;
using namespace sinlib;

unordered_map<int, string> Config::_names {
	{Config::TASK_DIR, "TaskDir"},
	{Config::TEMP_DIR, "TempDir"},
	{Config::LISTEN_ADDRESS, "ListenAddress"},
	{Config::LISTEN_PORT, "ListenPort"},
	{Config::TASK_TIMEOUT, "TaskTimeout"},
};

unordered_map<int, string> Config::_defs {
	{Config::TASK_DIR, "./tasks/"},
	{Config::TEMP_DIR, "./tmp/"},
	{Config::LISTEN_ADDRESS, "127.0.0.1"},
	{Config::LISTEN_PORT, "3500"},
	{Config::TASK_TIMEOUT, "30000"},
};

Config::Config() : defaults(nullptr) {}
Config::Config(Config &defs) : defaults(&defs) {}
Config::~Config(){}

void Config::loadDefaultConfig(){
	_vals = _defs;
}

void Config::loadFromFile(string path){
	fstream conf(path);

	if (!conf.good()){
		Log::warn("Can't open config file, using defaults: ", path);
		return;
	}

	string line;
	while (getline(conf, line)){
		// recognize '#' as start of comment only after whitespaces
		// ('#' can be part of pathes)
		line = regex_replace(line, regex("^\\s*#.*$|^\\s+|\\s+#.*$|\\s+$"), "");

		if (line.empty())
			continue;

		vector<string> pars = regex_split(line, regex("\\s+"), 2);
		if (pars.size() == 2){
			EValue code = getParameterCode(pars[0]);
			if (code == INVALID){
				Log::warn("Skipped unknown parameter: ", pars[0]);
				continue;
			}

			_vals[code] = pars[1];

			Log::debug("Set param ", getParameterName(code), " to '", pars[1], "'");
		} else {
			Log::warn("Skipped broken config line: ", line);
		}
	}
}

bool Config::checkIsValid(){
	if (!path_exists(getString(TASK_DIR) + "/")){
		Log::error(getParameterName(TASK_DIR), " path not found: ", getString(TASK_DIR));
		return false;
	}

	if (!path_exists(getString(TEMP_DIR) + "/")){
		Log::error(getParameterName(TEMP_DIR), " path not found: ", getString(TEMP_DIR));
		return false;
	}

	return true;
}

string Config::getString(EValue val){
	auto it = _vals.find(val);
	if (it == _vals.end()){
		if (defaults){
			return defaults->getString(val);
		}
		throw std::runtime_error("Invalid config value: " + std::to_string(val));
	}

	return it->second;
}

int Config::getInteger(EValue val){
	return std::stoi(getString(val));
}

string Config::getParameterName(EValue v){
	auto it = _names.find(v);
	if (it == _names.end()){
		return "";
	}

	return it->second;
}

Config::EValue Config::getParameterCode(string name){
	for (auto &it : _names){
		if (it.second == name){
			return (EValue) it.first;
		}
	}

	return (EValue) -1;
}

}

