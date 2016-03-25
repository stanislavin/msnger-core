//
//  IHTTPClient.h
//  msnger-core
//
//  Created by Stanislav Slavin on 23/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef IHTTPClient_h
#define IHTTPClient_h

class IHTTPClient
{
public:
    class IHTTPReceiver
    {
    public:
        virtual void onHTTPResponse(int errorCode, const char* response) = 0;
    };
    
    virtual ~IHTTPClient() {};
    
    virtual void get(const char* url, IHTTPReceiver* receiver) = 0;
    virtual void post(const char* url, const char* data, size_t data_len, const char* headers[], int nheaders, IHTTPReceiver* receiver) = 0;
};

#endif /* IHTTPClient_h */
