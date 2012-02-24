/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/*
 * parsecmd.cpp
 *
 *      Author: rwolff
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rtc_common.h"
#include "anyoption/anyoption.h"
#include "TestDefaults.h"

#include <strings.h>
#include <string.h>

#define MAINSERVER_PORT_DEFAULT 8888
#define STUNSERVER_DEFAULT "STUN stun.l.google.com:19302"

using namespace std;

string peername = "none";

string mainserver = "localhost";
int mainserver_port = MAINSERVER_PORT_DEFAULT;

string stunserver = STUNSERVER_DEFAULT;
string startupscript = "";
string logfile = "";

/**
	Utilizing AnyOption class, take the command line and parse it for valid options. Handle usage printout as well.
	@param argc argc from main()
	@param char**argv array of char* directly from main()
	@returns If an error occurs, return false. Currently no errors. Always returns true.
 */
bool parsecmd(int argc, char**argv)
{
    stringstream str;
    // Setup all default values if not already set by simple strings and values above.

    if (getenv("WEBRTC_SERVER"))
        mainserver = xGetDefaultServerName();

    if (getenv("WEBRTC_CONNECT"))
    {
        stunserver = "STUN ";
        stunserver += xGetPeerConnectionString();
    }
    
    peername = xGetPeerName();
    
	AnyOption *opt = new AnyOption();

	opt->addUsage("Usage: ");
	opt->addUsage("");
	opt->addUsage(" -h  --help                    Prints this help ");
    opt->addUsage(" --server <servername|IP>[:<serverport>]      Main sever and optional port.");
	opt->addUsage(
			" --stun <stunserver|IP>[:<port>]      STUN server (and optionally port).\n   Default is " STUNSERVER_DEFAULT);
	opt->addUsage(
			" --peername <Name>     Use this name as my client/peer name online.");
	opt->addUsage("");
    opt->addUsage("Environment variables can be used as defaults as well.");
    opt->addUsage("  WEBRTC_SERVER - Main server name.");
    opt->addUsage("  WEBRTC_CONNECT - STUN server name with port.");
    opt->addUsage("  USERNAME - Peer user name.");
	opt->addUsage("");

	opt->setFlag("help", 'h');
	opt->setOption("server");
	opt->setOption("stun");
	opt->setOption("peername");

	opt->processCommandArgs(argc, argv);

	/* 6. GET THE VALUES */
	if (opt->getFlag("help") || opt->getFlag('h'))
	{
		opt->printUsage();
		exit(1);
	}

    if (opt->getValue("server"))
    {
        cout << "New main server is " << opt->getValue("server") << endl;
        
        string serverloc = opt->getValue("server");
        size_t colonpos = serverloc.find(':');
        mainserver = serverloc.substr(0,colonpos);
        
        if(colonpos != string::npos)
        {
            stringstream sstrm;
            sstrm << serverloc.substr(colonpos+1);
            sstrm >> mainserver_port;
        }
    }

    if (opt->getValue("stun"))
    {
        stunserver = "STUN ";
        stunserver += opt->getValue("stun");
        cout << "New STUN server command: " << stunserver << endl;
    }
    
    if (opt->getValue("peername"))
    {
        peername = opt->getValue("peername");
        cout << "New peer name is: " << peername << endl;
    }

//    if (opt->getValue("script") != NULL)
//    {
//        startupscript = opt->getValue("script");
//        cout << "Initial script is: " << startupscript << endl;
//    }

	delete opt;
    
	return true;
}

