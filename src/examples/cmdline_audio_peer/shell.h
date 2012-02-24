/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

/**
 * @file shell.h
 *
 *  @brief Parse and process inbound keyboard commands (or scripts).
 *      @author rwolff
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <map>

#include <string.h>

/**
	Command line processor for an interactive shell. Commands are of the form:
        CMD [[name=value] ...]
    The processor winds up producing a 'cmd', and a name/value pair map<>
    which is of type map<std::string, std::string>
 */
class Shell
{
public:
    
	Shell();
	virtual ~Shell();
/**
	Take a stream-based input and parse a single line to obtain the command, 
    and any name/value pairs. Note that this stream can be from stdin (cin) or could be
    from a script file.
	@param input stream from which to take the line for processing.
	@param isScript=false The default is to process from the keyboard cin input. But this
            flag notes that it's really a script. When it's a script, we don't really want
            prompts to be sent back between every command being processed. Process in batch.
	@returns returns true in all cases except when EXIT or QUIT is found.
 */
    bool parseLine(std::istream& input, bool isScript=false);
	/**
		Enable a file to be used as the stream input in parseLine().
		@param fname File name to be used as the script input.
		@returns Returns true when complete. Returns false if file name cannot be opened.
	 */
	bool ProcessScript(const char* fname);
    
    /**
        Ability for calling entity to get back the name value pairs from outside the object.
     */
    std::map<std::string, std::string>& getPairs(void) { return namevalues; };

protected:
/// \brief parseAttributes must take a stringstream and process its name=value pairs. This must
///			also take into consideration spaces before/after the '=' and the use of '"' as delimters on values.
///			Care is taken to parse odd situations, but this is intended to be a well behaved input either from
///			a machine-written xml generator or a careful human inputting line-oriented commands.
	/**
                    
		@param inputstr The portion of the command line which contains a list of name=value pairings.
		@returns true if there are namevalue pairs. false if there are no pairs or if there was a syntactic error parsing.
	 */
	bool parseAttributes(const char* inputstr);
	/**
		This is were the organized decisions get made for taking action based on a command already parsed.
		@param cmd The main command given.
		@param subcmd The subcommand if any.
		@returns true if everything including the command execution goes well. false if required parameters are missing.
                false if command fails to execute.
	 */
	bool processCommand(std::string& cmd, std::string& subcmd);

private:
    /** The definition of what we use to store all name/value pairs */
	typedef std::map<std::string, std::string> attrNameValuePairs;
	/**
		The list of names and value combination parsed from the command line.
	 */
	attrNameValuePairs namevalues;

};

#endif /* SHELL_H_ */

