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
	};
private:
	Config(){}
	~Config(){}

	static std::unordered_map<int, std::string> _vals;
	static std::unordered_map<int, std::string> _names;
public:
	static void loadFromFile(std::string path);
	static bool checkIsValid();

	static std::string getParameterName(EValue v){
		auto it = _names.find(v);
		if (it == _names.end()){
			return "";
		}

		return it->second;
	}

	static EValue getParameterCode(std::string name){
		for (auto &it : _names){
			if (it.second == name){
				return (EValue) it.first;
			}
		}

		return (EValue) -1;
	}

	static std::string getString(EValue val){
		auto it = _vals.find(val);
		if (it == _vals.end()){
			throw std::runtime_error("Invalid config value: " + std::to_string(val));
		}
		
		return it->second;
	}

	static int getInteger(EValue val){
		return std::stoi(getString(val));
	}
};

}

