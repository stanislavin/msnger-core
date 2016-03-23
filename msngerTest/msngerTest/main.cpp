//
//  main.cpp
//  msngerTest
//
//  Created by Stanislav Slavin on 21/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#include <iostream>
#include "Messenger.h"
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

int main(int argc, const char * argv[])
{
    Message msg(L"Check this out!", L"79213276120", 59.902878, 30.398030);
    MessageResultHandler handler;
    
    Messenger messenger;
    messenger.send(msg, &handler);
    
    messageEvent.wait();
    
    return 0;
}
