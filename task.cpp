#include "task.hpp"

#include "logger.hpp"
#include "utils.hpp"

#include "pstream.h"

#include <thread>

namespace remexec {

using namespace std;
using namespace redi;

Task::Task(string name, string binary, string tmpath)
{
	this->name = name;
	this->binary = binary;
	this->tmpath = tmpath;
}

Task::~Task()
{
}

void Task::run(ostream &out, ostream &err){
	string wd = getcwd();
	chdir(tmpath);
	
	pstream p(binary, pstreams::pstdout | pstreams::pstderr);

	chdir(wd);

	if (p.good()){
		char buf[32];
		size_t n;

		bool done[2] {false, false};

		while (!done[0] && !done[1]){
			for (int i = 0; i < 2; ++i){
				if (!done[i]){
					while ((n = (i == 0 ? p.out() : p.err()).readsome(buf, sizeof(buf))) > 0){
						(i == 0 ? out : err).write(buf, n);
					}

					if (p.eof()){
						done[i] = true;
						p.clear();
					}
				}
			}

			usleep(100000); //100ms
		}

		p.close();
	}
}

}
