//
//  main.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <iostream>
#include "Messenger.h"
#include "Msnger.h"
#include "event.h"

event messageEvent(false);

class MessageResultHandler: public MessageResultListener
{
public:
    void onMessageSent(int result) override
    {
        printf("Message sent: %d\n", result);
        messageEvent.set();
    }
    void onMessageDelivered() override
    {
        printf("Message delivered\n");
    }
};

void onMessageSent(int code)
{
    printf("Message sent (C-style): %d\n", code);
    messageEvent.set();
}

int main(int argc, const char * argv[])
{
    if (false)
    {
        // C++ interface
        Message msg(L"Check this out - C++!", L"79213276120", 59.902878, 30.398030);
        MessageResultHandler handler;
        Messenger messenger;
        messenger.send(msg, &handler);
        
        messageEvent.wait();
    }
    
    {
        // C interface
        Msnger msnger = createMsnger();
        sendMessage(msnger, "79213276120", "Check this out - C!", 59.902878, 30.398030, onMessageSent);
        
        messageEvent.wait();
        releaseMsnger(msnger);
    }
    
    return 0;
}
