#include <iostream>
#include <istream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "assert.h"

template<typename T> std::string ToString(T type)
{
    std::stringstream ss;

    ss << type;
    return ss.str();
}

template<typename T> T ToUpper(T &s)
{
      std::string::iterator i = s.begin();
      std::string::iterator end = s.end();

      while (i != end) {
        *i = std::toupper((unsigned char)*i);
        ++i;
      }

      return s;
}

#define TOUPPERSTR(s) (ToUpper<string>(s))

