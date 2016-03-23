//
//  MessageFactory.hpp
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef Messenger_h
#define Messenger_h

#include <stdio.h>
#include <queue>
#include <mutex>
#include <string>

#include "MessageLoop.h"
#include "Message.h"
#include "MessageResultListener.h"
#include "InfobipTransaction.h"
#include "GisTransaction.h"
#include "Timer.h"

using namespace std;

class Messenger:
public MessageLoop,
public GisTransaction::IGisCallback,
public InfobipTransaction::IInfobipCallback,
public Timer::ITimerCallback
{
public:
    virtual ~Messenger();
    
    void send(Message& message, MessageResultListener* listener);
    
private:
    
    class MessageHolder
    {
    public:
        MessageHolder(Message& m, MessageResultListener* l): message(m), listener(l), expectedNextState(0) {}
        Message                message;
        MessageResultListener* listener;
        int                    expectedNextState;
    };
    
    // MessageLoop
    void onMessage(int message, void* ctx) override;
    
    // GisTransaction::IGisCallback
    void onAddressResolved(int error, const char* address) override;
    
    // InfobipTransaction::IInfobipCallback
    void onInfobipMessageSent(int errorCode) override;
    
    // Message Loop processing
    void onMsgSend(Message& message, MessageResultListener* listener);
    void onMsgProcessNextMessage();
    void onMsgGisAddressResolved(int error, const char* address);
    void onMsgInfobipSendComplete(int error);
    void onMsgTimerExpired(int msg);
    
    // Timer::ITimerCallback
    void onTimerExpired(void* context) override;
    void onTimerCancelled(void* context) override;
    
    void postTimerForMessage(int message);
    
    queue<MessageHolder> mMessages;
    InfobipTransaction   mInfobip;
    GisTransaction       mGis;
    Timer*               mTimer = NULL;
};

#endif /* Messenger_h */
