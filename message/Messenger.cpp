//
//  MessageFactory.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include "Messenger.h"
#include "core.h"
#include "Log.h"

#define TRANSACTION_TIMEOUT (30000) /*ms*/

enum MSG_ID
{
    MSG_ID_UNKNOWN = 0,
    MSG_ID_SEND    = 1,
    MSG_ID_PROCESS_NEXT_MESSAGE = 2,
    MSG_ID_ADDR_RESOLVED = 3,
    MSG_ID_INFOBIP_SEND_COMPLETE = 4,
    MSG_ID_TIMER_EXPIRED = 5,
    
    MSG_ID_LAST
};

class AddressResultHolder
{
public:
    AddressResultHolder(int e, const char* a): error(e), address(a) {}
    int    error;
    string address;
};

Messenger::~Messenger()
{
    delete mTimer;
}

// GisTransaction::IGisCallback
void Messenger::onAddressResolved(int error, const char* address)
{
    AddressResultHolder* holder = new AddressResultHolder(error, address);
    post(MSG_ID_ADDR_RESOLVED, holder);
}

// InfobipTransaction::IInfobipCallback
void Messenger::onInfobipMessageSent(int errorCode)
{
    int* error = new int;
    *error = errorCode;
    post(MSG_ID_INFOBIP_SEND_COMPLETE, error);
}

// Timer::ITimerCallback
void Messenger::onTimerExpired(void* context)
{
    int* msg = (int*)context;
    // take care of msg in onMessage
    post(MSG_ID_TIMER_EXPIRED, msg);
}

void Messenger::onTimerCancelled(void* context)
{
    int* msg = (int*)context;
    Log::i("Messenger", "timer cancelled for %d", *msg);
    delete msg;
}

void Messenger::postTimerForMessage(int message)
{
    int* ctx = new int;
    *ctx = message;
    if (mTimer != NULL)
    {
        delete mTimer;
    }
    mTimer = Timer::createAndStart(TRANSACTION_TIMEOUT, this, ctx);
}

void Messenger::send(Message& message, MessageResultListener* listener)
{
    MessageHolder *holder = new MessageHolder(message, listener);
    post(MSG_ID_SEND, holder);
}

void Messenger::onMsgSend(Message& message, MessageResultListener* listener)
{
    // add message to queue
    MessageHolder holder(message, listener);
    mMessages.push(holder);
    
    // if this is first message in queue - start processing
    // otherwise this message will be taken care of later
    if (mMessages.size() == 1)
    {
        post(MSG_ID_PROCESS_NEXT_MESSAGE, NULL);
    }
}

void Messenger::onMsgProcessNextMessage()
{
    if (mMessages.empty()) return;
    
    MessageHolder& h = mMessages.front();
    h.expectedNextState = MSG_ID_ADDR_RESOLVED;
    
    // schedule timer to handle timeouts
    postTimerForMessage(MSG_ID_ADDR_RESOLVED);
    
    // send request to resolve address
    mGis.resolveAddress(h.message.getLat(), h.message.getLon(), this);
}

void Messenger::onMsgGisAddressResolved(int error, const char* address)
{
    if (mMessages.empty()) return;
    
    if (mTimer != NULL) mTimer->cancel();
    
    MessageHolder &h = mMessages.front();
    if (h.expectedNextState != MSG_ID_ADDR_RESOLVED)
    {
        // unexpected state
        return;
    }
    h.expectedNextState = MSG_ID_INFOBIP_SEND_COMPLETE;
    
    // tbd: unicodes
    string saddress(address);
    wstring waddress(saddress.begin(), saddress.end());
    h.message.setAddress(waddress);
    
    // convert to JSON
    wstring json = h.message.toJSON();
    
    // schedule timer to handle timeouts
    postTimerForMessage(MSG_ID_INFOBIP_SEND_COMPLETE);
    
    // send request to infobip
    mInfobip.sendMessage(json, this);
}

void Messenger::onMsgInfobipSendComplete(int error)
{
    // TODO: implement delivery reports
    
    if (mMessages.empty()) return;
    
    if (mTimer != NULL) mTimer->cancel();
    
    MessageHolder& h = mMessages.front();
    if (h.expectedNextState != MSG_ID_INFOBIP_SEND_COMPLETE)
    {
        // unexpected state
        return;
    }
    
    
    h.listener->onMessageSent(error);
    mMessages.pop();
    
    post(MSG_ID_PROCESS_NEXT_MESSAGE);
}

void Messenger::onMsgTimerExpired(int msg)
{
    if (mMessages.empty()) return;
    
    MessageHolder& h = mMessages.front();
    if (h.expectedNextState != msg)
    {
        // timer expiration not expected in this state
        // ignore
        return;
    }
    
    switch (msg)
    {
        case MSG_ID_ADDR_RESOLVED:
        {
            Log::i("Messenger", "timer expired for MSG_ID_ADDR_RESOLVED");
            h.listener->onMessageSent(ERROR_GIS_TIMEOUT);
            break;
        }
            
        case MSG_ID_INFOBIP_SEND_COMPLETE:
        {
            Log::i("Messenger", "timer expired for MSG_ID_INFOBIP_SEND_COMPLETE");
            h.listener->onMessageSent(ERROR_INFOBIP_TIMEOUT);
            break;
        }
        
        default:
        {
            break;
        }
    }
    mMessages.pop();
}

void Messenger::onMessage(int message, void* ctx)
{
    switch (message)
    {
        case MSG_ID_SEND:
        {
            Log::i("Messenger", "MSG_ID_SEND");
            MessageHolder* h = (MessageHolder*)ctx;
            if (h != NULL)
            {
                onMsgSend(h->message, h->listener);
                delete h;
            }
            break;
        }
            
        case MSG_ID_ADDR_RESOLVED:
        {
            Log::i("Messenger", "MSG_ID_ADDR_RESOLVED");
            AddressResultHolder* a = (AddressResultHolder*)ctx;
            if (a != NULL)
            {
                onMsgGisAddressResolved(a->error, a->address.c_str());
                delete a;
            }
            break;
        }
            
        case MSG_ID_INFOBIP_SEND_COMPLETE:
        {
            Log::i("Messenger", "MSG_ID_INFOBIP_SENT");
            int* error = (int*)ctx;
            if (error != NULL)
            {
                onMsgInfobipSendComplete(*error);
                delete error;
            }
            break;
        }
            
        case MSG_ID_PROCESS_NEXT_MESSAGE:
        {
            Log::i("Messenger", "MSG_ID_PROCESS_NEXT_MESSAGE");
            onMsgProcessNextMessage();
            break;
        }
            
        case MSG_ID_TIMER_EXPIRED:
        {
            Log::i("Messenger", "MSG_ID_TIMER_EXPIRED");
            int* msg = (int*)ctx;
            if (msg != NULL)
            {
                onMsgTimerExpired(*msg);
                delete msg;
            }
            break;
        }
            
        default:
        {
            Log::i("Messenger", "message not handled");
            break;
        }
    }
}