/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

// File: TestDefaults.h
// Project: cmdline_audio_peer

#ifndef cmdline_audio_peer_TestDefaults_h
#define cmdline_audio_peer_TestDefaults_h

#pragma once

#include <string>
#include "talk/base/basictypes.h"

extern std::string peername;
extern std::string mainserver;
extern int mainserver_port;

extern const char kAudioLabel[];
extern const char kVideoLabel[];
extern const char kStreamLabel[];
extern const uint16 xkDefaultServerPort;

std::string xGetEnvVarOrDefault(const char* env_var_name,
                               const char* default_value);
std::string xGetPeerConnectionString();
std::string xGetDefaultServerName();
std::string xGetPeerName();


#endif

