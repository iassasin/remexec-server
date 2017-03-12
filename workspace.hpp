/*
 * workspace.hpp
 *
 *  Created on: 22 февр. 2017 г.
 *      Author: assasin
 */

#pragma once

#include <string>

#include "config.hpp"

namespace remexec {

class Workspace {
	std::string tmpath;
	bool created;
	Config &config;

	void cleanup();
public:
	Workspace(Config &conf);
	~Workspace();

	std::string getWorkdir();
};

}
