/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

#ifndef RTC_COMMON_H_
#define RTC_COMMON_H_

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

template<typename T> T FromString(std::string& str)
{
    std::stringstream ss;
    T retval;
    
    ss << str;
    ss >> retval;
    
    return retval;
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

template<typename T> T ToLower(T &s)
{
    std::string::iterator i = s.begin();
    std::string::iterator end = s.end();
    
    while (i != end) {
        *i = std::tolower((unsigned char)*i);
        ++i;
    }
    
    return s;
}

#define TOUPPERSTR(s) (ToUpper<std::string>(s))
#define TOLOWERSTR(s) (ToLower<std::string>(s))

#endif