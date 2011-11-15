//
//  ThreadSafeMessageQueue.cpp
//  cmdline_audio_peer
//
//  Created by Manjesh Malavalli on 11/2/11.
//  Copyright 2011 XVDTH. All rights reserved.
//

#include <assert.h>
#include "WPLThreadSafeMessageQueue.h"

namespace GoCast
{
    ThreadSafeMessageQueue::ThreadSafeMessageQueue()
    {
        pthread_mutex_init(&qMutex, NULL);
    }

    ThreadSafeMessageQueue::~ThreadSafeMessageQueue()
    {
        pthread_mutex_destroy(&qMutex);
    }

    void ThreadSafeMessageQueue::PostMessage(ParsedCommand& msg)
    {
        int pthreadRet = pthread_mutex_lock(&qMutex);
        assert(0 == pthreadRet);
        cmdQ.push_back(msg);
        pthreadRet = pthread_mutex_unlock(&qMutex);
        assert(0 == pthreadRet);
    }

    ThreadSafeMessageQueue::ParsedCommand ThreadSafeMessageQueue::GetNextMessage(void)
    {
        ParsedCommand nextMsg;
        int pthreadRet = pthread_mutex_lock(&qMutex);
        assert(0 == pthreadRet);
        
        if(!cmdQ.empty())
        {
            nextMsg = cmdQ.front();
            cmdQ.pop_front();
        }
        
        pthreadRet = pthread_mutex_unlock(&qMutex);
        assert(0 == pthreadRet);
        
        return nextMsg;
    }
}