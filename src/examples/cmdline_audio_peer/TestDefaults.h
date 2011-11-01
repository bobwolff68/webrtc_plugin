//
//  TestDefaults.h
//  TestPeerConnectionClient
//
//  Created by Manjesh Malavalli on 10/14/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef TestPeerConnectionClient_TestDefaults_h
#define TestPeerConnectionClient_TestDefaults_h

#pragma once

#include <string>
#include "talk/base/basictypes.h"

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
