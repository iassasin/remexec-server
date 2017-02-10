#pragma once

#include <unordered_map>
#include <exception>

namespace remexec {

class Config {
	public:
		enum EValue : int {
			TASK_DIR = 0,
			TEMP_DIR,
		};
	private:
		Config(){}
		~Config(){}
		
		static std::unordered_map<int, std::string> _vals;
	public:
		
		static std::string readString(EValue val){
			auto it = _vals.find(val);
			if (it == _vals.end()){
				throw std::runtime_error("Invalid config value: " + std::to_string(val));
			}
			
			return it->second;
		}
};

}

