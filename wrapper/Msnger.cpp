//
//  Msnger.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <map>
#include <string>

#include "Msnger.h"
#include "Messenger.h"
#include "MessageResultListener.h"

using namespace std;

class MsngerListener;

typedef struct MsngerCtx
{
    Messenger* m;
    map<MsngerListener*, onMessageSentCallback> callbacks;
} MsngerCtx;

class MsngerListener: public MessageResultListener
{
public:
    MsngerListener(MsngerCtx* ctx):
    mCtx(ctx)
    {
    }
    
    virtual void onMessageSent(int result)
    {
        if (mCtx->callbacks.find(this) != mCtx->callbacks.end())
        {
            onMessageSentCallback callback = mCtx->callbacks[this];
            if (callback) callback(result);
            mCtx->callbacks.erase(this);
        }
        
    }
    virtual void onMessageDelivered()
    {
        // TODO: implement
    }
    
private:
    MsngerCtx* mCtx;
};

Msnger createMsnger()
{
    MsngerCtx* ctx = new MsngerCtx;
    ctx->m = new Messenger();
    return (Msnger)ctx;
}

void releaseMsnger(Msnger msnger)
{
    MsngerCtx* ctx = (MsngerCtx*)msnger;
    delete ctx->m;
    delete ctx;
}

void sendMessage(Msnger msnger, char* number, char* message, double lat, double lon, onMessageSentCallback callback)
{
    MsngerCtx* ctx = (MsngerCtx*)msnger;
    MsngerListener* listener = new MsngerListener(ctx);
    ctx->callbacks[listener] = callback;
    
    string stext(message);
    string srecipient(number);
    wstring text(stext.begin(), stext.end());
    wstring recipient(srecipient.begin(), srecipient.end());
    
    Message msg(text.c_str(), recipient.c_str(), lat, lon);
    ctx->m->send(msg, listener);
}