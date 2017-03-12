#pragma once

#include <unordered_map>
#include <exception>

namespace remexec {

class Config {
public:
	enum EValue : int {
		INVALID = -1,
		TASK_DIR = 0,
		TEMP_DIR,
		LISTEN_ADDRESS,
		LISTEN_PORT,
		TASK_TIMEOUT,
	};
private:
	std::unordered_map<int, std::string> _vals;
	Config *defaults;

	static std::unordered_map<int, std::string> _defs;
	static std::unordered_map<int, std::string> _names;
public:
	Config();
	Config(Config &def);
	~Config();

	void loadDefaultConfig();
	void loadFromFile(std::string path);
	bool checkIsValid();

	std::string getString(EValue val);
	int getInteger(EValue val);

	static std::string getParameterName(EValue v);
	static EValue getParameterCode(std::string name);
};

}

