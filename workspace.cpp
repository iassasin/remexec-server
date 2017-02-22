/*
 * workspace.cpp
 *
 *  Created on: 22 февр. 2017 г.
 *      Author: assasin
 */

#include "workspace.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "utils.hpp"

#include "pstream.h"

namespace remexec {

using namespace std;
using namespace redi;

Workspace::Workspace(){
	created = false;
	tmpath = realpath(Config::getString(Config::TEMP_DIR)) + "/" + to_string(time(NULL)) + "/";
}

Workspace::~Workspace(){
	if (created){
		cleanup();
	}
}

string Workspace::getWorkdir(){
	if (!created){
		Log::debug("Create path: ", tmpath);
		mkdir(tmpath);
		created = true;
	}

	return tmpath;
}

void Workspace::cleanup(){
	Log::debug("rm -rf --one-file-system ", tmpath);

	pstream prm("rm", {"rm", "-rf", "--one-file-system", tmpath});
	prm.close();
	created = false;

	Log::debug("Cleanup complete");
}

}
