//
//  ThreadSafeMessageQueue.h
//  cmdline_audio_peer
//
//  Created by Manjesh Malavalli on 11/2/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#ifndef WebrtcPlugin_ThreadSafeMessageQueue_h
#define WebrtcPlugin_ThreadSafeMessageQueue_h

#include <deque>
#include <map>
#include <string>
#include <pthread.h>

namespace projectname
{
    class ThreadSafeMessageQueue
    {
    public:
        typedef std::map<std::string, std::string> ParsedCommand;
        
    private:
        std::deque<ParsedCommand> cmdQ;
        pthread_mutex_t qMutex;
        
    public:
        ThreadSafeMessageQueue();
        virtual ~ThreadSafeMessageQueue();
        void PostMessage(ParsedCommand& msg);
        ParsedCommand GetNextMessage(void);
    };
}

#endif