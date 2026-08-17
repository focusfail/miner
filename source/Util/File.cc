#include "Util/File.hh"
#include <fstream>
#include <sstream>

std::string ReadFile(const std::string &filename) {
  std::ifstream f(filename);
  std::ostringstream sstr;
  sstr << f.rdbuf();
  return sstr.str();
}