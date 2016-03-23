//
//  MessageResultListener.h
//  msngerTest
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef MessageResultListener_h
#define MessageResultListener_h

class MessageResultListener
{
public:
    virtual void onMessageSent(int result) = 0;
    virtual void onMessageDelivered() = 0;
};

#endif /* MessageResultListener_h */
