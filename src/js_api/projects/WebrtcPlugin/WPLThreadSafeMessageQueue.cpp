/*
 *  Copyright (c) 2011 GoCast. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the 
 *  AUTHORS file in the root of the source tree.
 */

//  File: WPLThreadSafeMessageQueue.cpp
//  Project: WebrtcPlugin

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
        // If we get a non-zero back from the mutex lock, don't push and don't unlock after...
        if (pthreadRet==0)
        {
            cmdQ.push_back(msg);
            pthreadRet = pthread_mutex_unlock(&qMutex);
            assert(0 == pthreadRet);
        }
    }

    ThreadSafeMessageQueue::ParsedMessage ThreadSafeMessageQueue::GetNextMessage(void)
    {
        ParsedMessage nextMsg;
        int pthreadRet = pthread_mutex_lock(&qMutex);
        assert(0 == pthreadRet);
        // If we get a non-zero back from the mutex lock, don't push and don't unlock after...
        if (pthreadRet==0)
        {
            if(!cmdQ.empty())
            {
                nextMsg = cmdQ.front();
                cmdQ.pop_front();
            }
            
            pthreadRet = pthread_mutex_unlock(&qMutex);
            assert(0 == pthreadRet);
        }
        else
            nextMsg["bad"] = "bad";
        
        return nextMsg;
    }
}

