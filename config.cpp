#include "config.hpp"

namespace remexec {

using namespace std;

std::unordered_map<int, string> Config::_vals {
	{Config::TASK_DIR, "./tasks/"},
	{Config::TEMP_DIR, "./tmp/"},
};

}

