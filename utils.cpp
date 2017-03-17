#include "utils.hpp"

#include <string>
#include <cstring>
#include <cmath>

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

string basename(std::string path){
	return string(::basename(path.c_str()));
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

size_t bscopy(ostream &out, istream &in, size_t count){
	char buf[4096];
	size_t csize = count;

	while (csize > 0 && out.good() && in.read(buf, min(sizeof(buf), csize))){
		size_t gc = in.gcount();
		csize -= gc;
		out.write(buf, gc);
	}

	return count - csize;
}

string stohex(const string &s){
	static const char HEX_ALPHA[] = "0123456789ABCDEF";

	string res(s.size() * 2, '\0');
	for (size_t i = 0; i < s.size(); ++i){
		res[i*2] = HEX_ALPHA[s[i] >> 4];
		res[i*2+1] = HEX_ALPHA[s[i] & 0xf];
	}
	return move(res);
}

string hextos(const string &s){
	auto char2code = [](char c) -> int {
		if (c >= '0' && c <= '9'){
			return c - '0';
		}
		else if (c >= 'a' && c <= 'f'){
			return c - 'a' + 10;
		}
		else if (c >= 'A' && c <= 'F'){
			return c - 'A' + 10;
		}

		return 0;
	};

	string res(s.size() / 2, '\0');
	for (size_t i = 0; i < s.size(); ++i){
		res[i] = (char2code(s[i*2]) << 4) | char2code(s[i*2+1]);
	}

	return move(res);
}

}
