//
//  Infobip.hpp
//  msnger-core
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef InfobipTransaction_h
#define InfobipTransaction_h

#include <stdio.h>
#include <string>

#include "IHTTPClient.h"

class InfobipTransaction: public IHTTPClient::IHTTPReceiver
{
public:
    class IInfobipCallback
    {
    public:
        virtual void onInfobipMessageSent(int errorCode) = 0;
    };
    
    InfobipTransaction();
    ~InfobipTransaction();
    
    void sendMessage(const std::wstring& message, IInfobipCallback* callback);
    
private:
    void onHTTPResponse(int errorCode, const char* response);
    
    IInfobipCallback* mCallback = NULL;
    IHTTPClient*      mHttp = NULL;
};

#endif /* InfobipTransaction_h */
