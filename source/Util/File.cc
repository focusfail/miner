#include "Util/File.hh"
#include <fstream>
#include <sstream>

std::filesystem::path ExpandUser(const std::filesystem::path& p)
{
    auto s = p.string();

    if (s == "~" || s.starts_with("~/")) {
        if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / s.substr(2);
        }
    }

    return p;
}

std::string ReadFile(const std::filesystem::path &filename) {
  std::ifstream f(ExpandUser(filename));
  std::ostringstream sstr;
  sstr << f.rdbuf();
  return sstr.str();
}