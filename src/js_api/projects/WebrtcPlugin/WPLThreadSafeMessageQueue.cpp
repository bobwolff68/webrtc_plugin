//
//  File: WPLThreadSafeMessageQueue.cpp
//  Project: WebrtcPlugin
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

    void ThreadSafeMessageQueue::PostMessage(ParsedMessage& msg)
    {
        int pthreadRet = pthread_mutex_lock(&qMutex);
        assert(0 == pthreadRet);
        cmdQ.push_back(msg);
        pthreadRet = pthread_mutex_unlock(&qMutex);
        assert(0 == pthreadRet);
    }

    ThreadSafeMessageQueue::ParsedMessage ThreadSafeMessageQueue::GetNextMessage(void)
    {
        ParsedMessage nextMsg;
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