//
//  TaskThread.cpp
//  msnger-core
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include "MessageLoop.h"

using namespace std;

MessageLoop::MessageLoop():
    mEvent(false),
    mThread(runLoop, this)
{
    
}

MessageLoop::~MessageLoop()
{
    mStopped = true;
    mEvent.set();
    mThread.join();
}

void MessageLoop::post(int message, void *ctx)
{
    lock_guard<recursive_mutex> lock(mThreadMutex);
    MessageContext *mctx = new MessageContext();
    mctx->msg = message;
    mctx->ctx = ctx;
    mMsgQueue.push(mctx);
    mEvent.set();
}

void MessageLoop::runLoop(MessageLoop* self)
{
    if (self != NULL)
    {
        self->runLoopInternal();
    }
}

void MessageLoop::runLoopInternal()
{
    do
    {
        mEvent.wait();

        unique_lock<recursive_mutex> lock(mThreadMutex);
        while (!mMsgQueue.empty())
        {
            MessageContext* mctx = mMsgQueue.front();
            mMsgQueue.pop();
            lock.unlock();
            if (mctx != NULL)
            {
                onMessage(mctx->msg, mctx->ctx);
                delete mctx;
            }
            lock.lock();
        }
        mEvent.unset();
    }
    while (!mStopped);
}