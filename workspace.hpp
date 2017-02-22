/*
 * workspace.hpp
 *
 *  Created on: 22 февр. 2017 г.
 *      Author: assasin
 */

#pragma once

#include <string>

namespace remexec {

class Workspace {
	std::string tmpath;
	bool created;

	void cleanup();
public:
	Workspace();
	~Workspace();

	std::string getWorkdir();
};

}
