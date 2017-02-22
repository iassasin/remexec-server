#pragma once

#include <string>
#include <iostream>

namespace remexec {

std::string realpath(std::string relpath);
std::string getcwd();
bool path_exists(std::string path);
int mkdir(std::string path);
int chdir(std::string path);

std::size_t bscopy(std::istream &in, std::ostream &out, std::size_t count);

}
