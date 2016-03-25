//
//  TaskThread.hpp
//  msnger-core
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef MessageLoop_h
#define MessageLoop_h

#include <stdio.h>
#include <mutex>
#include <thread>
#include <queue>

#include "Event.h"

using namespace std;

class MessageLoop
{
public:
    MessageLoop();
    virtual ~MessageLoop();
    
    void post(int message, void* ctx = NULL);

private:
    
    virtual void onMessage(int message, void* ctx) = 0;
    
    struct MessageContext
    {
        int msg;
        void *ctx;
    };
    
    static void runLoop(MessageLoop* self);
    void runLoopInternal();
    bool waitForTask();
    
    thread                  mThread;
    Event                   mEvent;
    recursive_mutex         mThreadMutex;
    queue<MessageContext*>  mMsgQueue;
    bool                    mStopped = false;
};


#endif /* MessageLoop_h */
