#include "utils.hpp"

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

namespace remexec {

using namespace std;
	
string realpath(string relpath){
	char buf[PATH_MAX];
	::realpath(relpath.c_str(), buf);
	return string(buf);
}

string getcwd(){
	char buf[PATH_MAX];
	::getcwd(buf, sizeof(buf));
	return string(buf);
}

bool path_exists(string path){
	struct stat st;
	return stat(path.c_str(), &st) != -1;
}

int mkdir(string path){
	return ::mkdir(path.c_str(), 00700);
}

int chdir(string path){
	return ::chdir(path.c_str());
}

}