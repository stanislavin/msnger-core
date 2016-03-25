//
//  HttpClient.hpp
//  msnger-core
//
//  Created by Stanislav Slavin on 22/03/16.
//  Copyright © 2016 Stanislav Slavin. All rights reserved.
//

#ifndef HttpClient_h
#define HttpClient_h

#include <stdio.h>
#include <string>

#include "IHTTPClient.h"

using namespace std;

class HttpClient: public IHTTPClient
{
public:
    void get(const char* url, IHTTPClient::IHTTPReceiver* receiver);
    void post(const char* url, const char* data, size_t data_len, const char* headers[], int nheaders, IHTTPClient::IHTTPReceiver* receiver);
    
private:
    static size_t httpCallbackStatic(void *contents, size_t size, size_t nmemb, void *userp);
    
    int curlCode2ErrorCode(int curl_code);
};

#endif /* HttpClient_h */
