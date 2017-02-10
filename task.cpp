#include "task.hpp"

#include "pstream.h"

#include "utils.hpp"

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

void Task::run(ostream &out){
	string wd = getcwd();
	chdir(tmpath);
	
	pstream p(binary);
	
	if (p.good()){
		string pline;
		while (getline(p, pline)){
			out << pline << endl;
		}
		p.close();
	}
	
	chdir(wd);
}

}
