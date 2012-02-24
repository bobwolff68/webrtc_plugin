/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

// File: TestDefaults.cpp
// Project: cmdline_audio_peer

#include "TestDefaults.h"
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include "talk/base/common.h"

const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
const uint16 xkDefaultServerPort = 8888;

std::string xGetEnvVarOrDefault(const char* env_var_name,
                               const char* default_value)
{
    std::string value;
    const char* env_var = getenv(env_var_name);
    if (env_var)
        value = env_var;
    
    if (value.empty())
        value = default_value;
    
    return value;
}

std::string xGetPeerConnectionString()
{
    return xGetEnvVarOrDefault("WEBRTC_CONNECT", "STUN stun.l.google.com:19302");
}

std::string xGetDefaultServerName()
{
    return xGetEnvVarOrDefault("WEBRTC_SERVER", "localhost");
}

std::string xGetPeerName()
{
    char computer_name[256];
    if (gethostname(computer_name, ARRAY_SIZE(computer_name)) != 0)
        strcpy(computer_name, "host");
    std::string ret(xGetEnvVarOrDefault("USERNAME", "user"));
    ret += '@';
    ret += computer_name;
    return ret;
}

