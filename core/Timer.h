//
//  timer.h
//  msnger-core
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include <list>
#include <thread>
#include <mutex>

#include "Event.h"

using namespace std;

class Timer
{
public:
    class ITimerCallback
    {
    public:
        virtual void onTimerExpired(void* context) = 0;
        virtual void onTimerCancelled(void* context) = 0;
    };
    
    static Timer* createAndStart(int miliseconds, ITimerCallback* callback, void* context = NULL)
    {
        return new Timer(miliseconds, callback, context);
    }
    
    ~Timer()
    {
        cancel(false);
    }
    
    void cancel(bool notify = true)
    {
        evt.set();
        if (th.joinable())
        {
            th.join();
        }
        if (notify && callback != NULL)
        {
            callback->onTimerCancelled(callback_ctx);
        }
    }
    
private:
    
    Timer(int ms, ITimerCallback* c, void* ctx):
    callback(c),
    callback_ctx(ctx),
    evt(false),
    delay_ms(ms),
    th(run, this)
    {}
    
    static void run(Timer* ctx)
    {
        if (ctx != NULL)
        {
            ctx->_run();
        }
    }
    
    void _run()
    {
        if (evt.waitFor(delay_ms))
        {
            if (callback != NULL)
            {
                callback->onTimerExpired(callback_ctx);
            }
        }
    }
    
    ITimerCallback* callback;
    void*           callback_ctx;
    Event           evt;
    thread          th;
    int             delay_ms;
};

#endif /* Timer_h */
