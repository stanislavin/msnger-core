//
//  event.h
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef event_h
#define event_h

#include <mutex>

using namespace std;

class event
{
public:
    event(bool set):
    signaled(set)
    {
    }
    
    void set()
    {
        {
            unique_lock<mutex> lock(mtx);
            signaled = true;
        }
        var.notify_all();
    }
    void unset()
    {
        unique_lock<mutex> lock(mtx);
        signaled = false;
    }
    void wait()
    {
        unique_lock<mutex> lock(mtx);
        while(!signaled)
        {
            var.wait(lock);
        }
    }
    bool waitFor(int ms)
    {
        unique_lock<mutex> lock(mtx);
        bool timeout = false;
        while(!signaled)
        {
            timeout = var.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::timeout;
            if (timeout) break;
        }
        return timeout;
    }
    bool isSet()
    {
        return signaled;
    }
    
private:
    condition_variable  var;
    mutex               mtx;
    bool                signaled = false;
};

#endif /* event_h */
