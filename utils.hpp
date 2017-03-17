#pragma once

#include <string>
#include <iostream>

namespace remexec {

std::string realpath(std::string relpath);
std::string basename(std::string path);
std::string getcwd();
bool path_exists(std::string path);
int mkdir(std::string path);
int chdir(std::string path);

std::size_t bscopy(std::ostream &out, std::istream &in, std::size_t count);

std::string stohex(const std::string &s);
std::string hextos(const std::string &s);

}
