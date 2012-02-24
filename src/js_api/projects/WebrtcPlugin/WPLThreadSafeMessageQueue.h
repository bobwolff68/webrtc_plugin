/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLThreadSafeMessageQueue.h
//  Project: WebrtcPlugin

#ifndef WebrtcPlugin_WPLThreadSafeMessageQueue_h
#define WebrtcPlugin_WPLThreadSafeMessageQueue_h

#include <deque>
#include <map>
#include <string>
#include <pthread.h>

namespace GoCast
{
    /**
    	Thread-safe message queue class.
     */
    class ThreadSafeMessageQueue
    {
    public:
        typedef std::map<std::string, std::string> ParsedMessage;
        
    private:
        /**
        	Message queue.
         */
        std::deque<ParsedMessage> cmdQ;
        
        /**
        	Mutex to regulate access to message queue.
         */
        pthread_mutex_t qMutex;
        
    public:
        /**
        	Constructor.
        	@returns N/A.
         */
        ThreadSafeMessageQueue();
        
        /**
        	Destructor.
        	@returns N/A.
         */
        virtual ~ThreadSafeMessageQueue();
        
        /**
        	Post new message.
        	@param msg Message to be posted.
         */
        void PostMessage(ParsedMessage& msg);
        
        /**
        	Get next message to be processed.
        	@returns ParsedMessage: parsed message.
         */
        ParsedMessage GetNextMessage(void);
    };
}

#endif

