/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/*
 * shell.cpp
 *
 *      Author: rwolff
 */

#include "shell.h"
#include "rtc_common.h"

using namespace std;

Shell::Shell()
{
}

Shell::~Shell()
{
	// TODO Auto-generated destructor stub
}

//
//   T H E  L A N G U A G E  (The input format)
//
// <command> [naked-parameter] [<parameters - usually as name=value pairs> ....]
//
// QUIT|EXIT|^D
// - Exit shell parsing and quit.
//
// SIGNIN [servername[:port]]
//      command=signin, server=servername, serverport=port
//
// SIGNOUT
//      command=signout
//
// STUN <stun-name:portnum>     - Must be completed PRIOR to SIGNIN (during logged out state)
//      command=stun, stunserver=stun-name:portnum
//
// LIST     - List all peers currently connected.
//
// HANGUP   - If currently on a call, exit the call but stay logged in.
//
// CALL peer          - Implies 'adding a person' to an existing call as well as 
//                      initiating a call if one is not in progress.
//      command=call, peername=peer
//
// HELP     - The usage help. Basically this list.
//
// (later) INFO [ ]
//
// (later) DROP peername
//
// (later) RUNSCRIPT filename=<script_to_run_via_parser>
// - Open and read/parse filename - expected to be used for demo macros.
//
bool Shell::parseLine(istream& input, bool isScript)
{
	string line;
	stringstream linestream;
	string cmd, subcmd;
	stringstream attrstream;

    // Simply getting to EOF should not be a 'fail' for us. It'll get detected by the upper parser.
    if (!getline(input, line))
        return true;
    
    namevalues.clear();
    
    // Any line beginning with '#' is a comment.
    if (line[0]=='#')
        return true;

    //
    while (line=="" || line == "\n" || line == "\r" || line == "\n\r" || line == "\r\n")
        return true;

    if (isScript)
        cout << endl << "uBrain-Script > " << line << endl;
//		else
//			cout << "uBrain > " << flush;

//		cout << "line: " << line << endl;

    cmd = "";
    subcmd = "";
    
    linestream.clear();
    linestream.str(line);
//		cout << "linestream: " << linestream.str() << endl;
    //getline(linestream, word, ' ');
/*		while (!(linestream.rdstate() & ifstream::eofbit))
     {
     linestream >> word;
     cout << "word: " << word << endl;
     }
*/
    
    //
    // All commands are of the form <cmd> [<subcmd> [attributes]]
    //
    static char attr[100];
    
    linestream >> cmd;
    TOUPPERSTR(cmd);
    
    // Command with no subcommand nor attributes will be !.good()
    if (linestream.good())
    {
        // Read the sub command now.
        
        linestream >> subcmd;
        TOUPPERSTR(subcmd);
        
        // Again - subcommand with no attributes will be !.good() here
        if (linestream.good())
        {
            // read rest of line into attr for further processing.
            linestream.getline(attr, 99);
            
            // Knock out all white space prior to parsing attributes.
            int st=0;
            while (attr[st]==' ')
                st++;
            
            parseAttributes(&attr[st]);
        }
    }

//		cout << "Command: " << cmd << " -- SubCommand: " << subcmd << " -- Attributes: " << attributes << endl;

    // Now process.
    if (cmd=="EXIT" || cmd=="QUIT")
        return false;
    
    // for our purposes, we decided to put the cmd into namepairs["command"]
    namevalues["command"] = cmd;
    
    if (cmd!="")
        processCommand(cmd, subcmd);
    
    return true;
}

bool Shell::ProcessScript(const char* fname)
{
	ifstream fin;

	fin.open(fname);
	if (!fin.is_open())
	{
		cerr << "Error: Could not open script: '" << fname << "'" << endl;
		return false;
	}

	cout << "Executing Script:'" << fname << "' Please wait..." << endl;

	while (!fin.eof())
	{
		if (!parseLine(fin, true))
			break;

		sleep(1);		// Slow down script-running...
	}

	fin.close();
	return true;
}

///
/// \brief parseAttributes must take a stringstream and process its name=value pairs. This must
///			also take into consideration spaces before/after the '=' and the use of '"' as delimters on values.
///			Care is taken to parse odd situations, but this is intended to be a well behaved input either from
///			a machine-written xml generator or a careful human inputting line-oriented commands.
///
bool Shell::parseAttributes(const char* inputstr)
{
	string w;
	char buff[100];
	string name;
	string value;
	stringstream input;

	input << inputstr;
//	cout << "Input has contents:'" << input.str() << "'." << endl;

	// Erase all from the list.
	namevalues.clear();

	// Indicator that we're at the end.
	do
	{
		// Eat pre-run-whitespace
		while (input.peek()==' ')
			input.get();

		// Get first name (delimited by '=')
		// If not found eofbit will be set and we're in a syntax error state.
		input.getline(buff, 99, '=');

		// If we have a failure here, it may just mean we're in a non-sense attrib list or at the end of a list.
		// Dont fail...just break so that the length of the name/value pair list will determine success.

		if (input.eof())
			break;

//		cout << "RAW:'" << buff << "'" << endl;

		// Eat all trailing ' ' spaces.
		while (buff[strlen(buff)-1]==' ')
			buff[strlen(buff)-1]=0;
//		cout << "Processed:'" << buff << "'" << endl;

		name = buff;	// Copies buff string into name.

		// Moving on to the next part (the value - either quoted or not)
		// Eat pre-run-whitespace
		while (input.peek()==' ')
			input.get();

//		cout << "NEXTCHAR:'" << s.peek() << "'" << endl;

		// If next char is '"', we must eat all '"' if more than one and then read until '"'
		// else just read till next space.
		bool isSingleQuote=false;
		bool isDoubleQuote=false;
		char outerQuote;
		char innerQuote;

		//
		// We now support attribute values enclosed in double-quotes (") as well as single-quotes (')
		// Additionally, within the quoted string, the opposing quote style can be used as a double-quote
		// Examples:
		//  CMD SUBCMD extracommand="x and 1001 or 6000 with 'sess NameHere'"
		// is equal to...
        //  LOCAL CONTROLLERDIRECT command='x and 1001 or 6000 with "sess NameHere"'
		//
		if (input.peek()=='\"')
		{
		    isDoubleQuote = true;
		    outerQuote = '\"';
		    innerQuote = '\'';
		}
		else if (input.peek()=='\'')
        {
		    isSingleQuote = true;
		    outerQuote = '\'';
		    innerQuote = '\"';
        }
        
        (void)innerQuote;   // Set but not used avoidance. This code was borrowed from another internal project hence the issue.

		if (isDoubleQuote || isSingleQuote)
		{
			while (input.peek()==outerQuote)
			{
				input.get();
				if (input.eof())
					return false;
			}

			// Now first quote is read...now read till last matching quote is found.
			input.getline(buff, 99, outerQuote);

			// Eat all trailing ' ' spaces.
			while (buff[strlen(buff)-1]==' ')
				buff[strlen(buff)-1]=0;

			value = buff;

			// Now - only in the case of outer double-quotes do a search and replace of single quotes to double quotes.
			// Now look for single quotes and if found, substitute with double-quotes.
			while(isDoubleQuote)
			{
			    size_t index=0;

			    index = value.find("'", index);
			    if (index == string::npos)
			        break;
			    else
			        value[index] = '"';
			}

		}
		else
			input >> value;

//		cout << "RAW:'" << buff << "'" << endl;

//		cout << "Processed:'" << buff << "'" << endl;

		namevalues[name] = value;
	} 	while (input.good());


#if 0
	// Now iterate the list and print them.
	if (!namevalues.empty())
	{
		  attrNameValuePairs::iterator it;

		  for ( it=namevalues.begin() ; it != namevalues.end(); it++ )
		    cout << (*it).first << " => " << (*it).second << endl;

	}
#endif

	// If we have a name/value pair, then we assume we're good. At least one.
	return !namevalues.empty();
}

bool Shell::processCommand(string& cmd, string& subcmd)
{
	// Convert cmd and subcmd to upper case for further processing as XML elements.
	TOUPPERSTR(cmd);
	TOUPPERSTR(subcmd);

//	switch(cmd.c_str())
	{
		if (cmd=="HELP" || cmd=="?")
		{
		cout << endl << "Valid Commands are:" << endl << endl
            << "QUIT|EXIT" << endl
            << endl
            << "SIGNIN [servername[:port]]" << endl
            << endl
            << "SIGNOUT" << endl
            << endl
            << "STUN <stun-name:portnum>   (sets the stun server - only valid while signed OUT." << endl
            << endl
            << "LIST        (list all logged-in peers)" << endl
            << "CALLLIST    (list of peers on the current call)" << endl
            << endl
            << "HANGUP <peername>" << endl
            << endl
            << "CALL <peername>          - Implies 'adding a person' to an existing call as well as" << endl
            << "                     initiating a call if one is not in progress." << endl
            << endl
            << "HELP|?" << endl
            << endl
			<< "QUIT|EXIT" << endl
			<< endl;
		return true;
		}

		if (cmd[0]=='#')
			return true;

		if (cmd=="EXIT" || cmd=="QUIT")
            return true;
			//exit(0);

        if (cmd=="SIGNIN")
        {
            if (subcmd != "")
            {
                // Must pre-parse the subcmd as it can contain the server name
                // and also the port with a ":" separator.
                size_t pos;
                pos = subcmd.find(":");
                if (pos == string::npos)    // No colon found.
                    namevalues["server"] = subcmd;
                else
                {
                    namevalues["server"] = subcmd.substr(0, pos);
                    namevalues["serverport"] = subcmd.substr(pos+1);
                    cout << namevalues["server"] << " at port " << namevalues["serverport"] << endl;
                }
            }
            
            return true;
        }
        
        if (cmd=="CALL")
        {
            // Call requires a name following.
            if (subcmd=="")
            {
                cout << "Illegal form - argument required: CALL <peer>" << endl;
                return false;
            }
            else
            {
                namevalues["peername"] = subcmd;
                return true;
            }
        }
        
        if (cmd=="STUN")
        {
            // STUN requires a name following.
            if (subcmd=="")
            {
                cout << "Illegal form - argument required: STUN <stunservername:port>" << endl;
                return false;
            }
            else
            {
                namevalues["stunserver"] = subcmd;
                return true;
            }
            
        }
        
        if(cmd=="LIST")
        {
            return true;
        }
        
        if(cmd=="CALLLIST")
        {
            return true;
        }
        
        if(cmd=="SIGNOUT")
        {
            return true;
        }
        
        if(cmd=="HANGUP")
        {
            if(subcmd != "")
            {
                namevalues["peername"] = subcmd;
                return true;
            }
            
            cout << "No peer specified for hangup" << endl;
            return false;
        }

//	default:
		cout << endl << "ERROR: Command: " << cmd << " is unknown to the parser." << endl;
		return false;
	}

	// Should never reach here.
	assert(false);
	return true;
}

