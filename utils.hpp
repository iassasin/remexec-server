#pragma once

#include <string>

namespace remexec {

std::string realpath(std::string relpath);
std::string getcwd();
bool path_exists(std::string path);
int mkdir(std::string path);
int chdir(std::string path);

}
